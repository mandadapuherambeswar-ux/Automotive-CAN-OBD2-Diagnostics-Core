/**
 * @file iso_tp.c
 * @brief ISO 15765-2 (ISO-TP) Transport Protocol Implementation
 * @author Herambeswar Mandadapu
 */

#include "iso_tp.h"
#include <string.h>

void IsoTp_Init(IsoTpLink_t *link, uint32_t tx_id, uint32_t rx_id)
{
    if (!link) return;
    memset(link, 0, sizeof(IsoTpLink_t));
    link->tx_id = tx_id;
    link->rx_id = rx_id;
    link->state = ISOTP_STATE_IDLE;
    link->block_size = 8;
    link->st_min_ms = 10;
}

bool IsoTp_ReceiveCanFrame(IsoTpLink_t *link, const CanMessage_t *can_msg, uint8_t *out_payload, uint16_t *out_len)
{
    if (!link || !can_msg || !out_payload || !out_len) return false;
    if (can_msg->id != link->rx_id && can_msg->id != ISOTP_DEFAULT_REQ_ID) return false;
    if (can_msg->dlc == 0) return false;

    uint8_t pci_type = (can_msg->data[0] >> 4) & 0x0F;

    switch (pci_type) {
        case ISOTP_PCI_TYPE_SF: {
            uint8_t sf_len = can_msg->data[0] & 0x0F;
            if (sf_len > (can_msg->dlc - 1) || sf_len > 7) return false;
            memcpy(out_payload, &can_msg->data[1], sf_len);
            *out_len = sf_len;
            return true;
        }

        case ISOTP_PCI_TYPE_FF: {
            uint16_t ff_len = (uint16_t)(((can_msg->data[0] & 0x0F) << 8) | can_msg->data[1]);
            if (ff_len > ISOTP_MAX_MSG_SIZE) return false;

            link->rx_total_len = ff_len;
            link->rx_received_len = can_msg->dlc - 2;
            memcpy(link->rx_buffer, &can_msg->data[2], link->rx_received_len);
            link->rx_expected_sn = 1;
            link->state = ISOTP_STATE_RECEIVING_CF;
            return false;
        }

        case ISOTP_PCI_TYPE_CF: {
            if (link->state != ISOTP_STATE_RECEIVING_CF) return false;

            uint8_t sn = can_msg->data[0] & 0x0F;
            if (sn != (link->rx_expected_sn & 0x0F)) {
                link->state = ISOTP_STATE_IDLE;
                return false;
            }

            uint16_t remaining = link->rx_total_len - link->rx_received_len;
            uint8_t copy_bytes = (remaining > (can_msg->dlc - 1)) ? (can_msg->dlc - 1) : (uint8_t)remaining;

            memcpy(&link->rx_buffer[link->rx_received_len], &can_msg->data[1], copy_bytes);
            link->rx_received_len += copy_bytes;
            link->rx_expected_sn = (link->rx_expected_sn + 1) & 0x0F;

            if (link->rx_received_len >= link->rx_total_len) {
                memcpy(out_payload, link->rx_buffer, link->rx_total_len);
                *out_len = link->rx_total_len;
                link->state = ISOTP_STATE_IDLE;
                return true;
            }
            return false;
        }

        case ISOTP_PCI_TYPE_FC: {
            if (link->state == ISOTP_STATE_WAIT_FC) {
                uint8_t fs = can_msg->data[0] & 0x0F;
                if (fs == ISOTP_FC_STATUS_CTS) {
                    link->block_size = can_msg->data[1];
                    link->st_min_ms = can_msg->data[2];
                    link->state = ISOTP_STATE_SENDING_CF;
                } else {
                    link->state = ISOTP_STATE_IDLE;
                }
            }
            return false;
        }

        default:
            return false;
    }
}

bool IsoTp_Transmit(IsoTpLink_t *link, const uint8_t *payload, uint16_t length, bool (*send_can_cb)(const CanMessage_t *msg))
{
    if (!link || !payload || length == 0 || length > ISOTP_MAX_MSG_SIZE || !send_can_cb) return false;

    CanMessage_t msg;
    memset(&msg, 0, sizeof(CanMessage_t));
    msg.id = link->tx_id;

    if (length <= 7) {
        msg.dlc = 8;
        msg.data[0] = (uint8_t)(ISOTP_PCI_TYPE_SF << 4 | length);
        memcpy(&msg.data[1], payload, length);
        for (uint8_t i = (uint8_t)(length + 1); i < 8; i++) msg.data[i] = 0xAA; /* Padding */
        return send_can_cb(&msg);
    } else {
        /* Multi-frame transmission: First Frame */
        memcpy(link->tx_buffer, payload, length);
        link->tx_total_len = length;
        link->tx_sent_len = 6;
        link->tx_sn = 1;

        msg.dlc = 8;
        msg.data[0] = (uint8_t)((ISOTP_PCI_TYPE_FF << 4) | ((length >> 8) & 0x0F));
        msg.data[1] = (uint8_t)(length & 0xFF);
        memcpy(&msg.data[2], payload, 6);

        link->state = ISOTP_STATE_WAIT_FC;
        return send_can_cb(&msg);
    }
}
