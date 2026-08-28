/**
 * @file iso_tp.h
 * @brief ISO 15765-2 (ISO-TP) Transport Protocol Stack Header
 * @author Herambeswar Mandadapu
 */

#ifndef ISO_TP_H
#define ISO_TP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CAN Identifier Defaults */
#define ISOTP_DEFAULT_REQ_ID    0x7DF    /* Functional Broadcast Request */
#define ISOTP_DEFAULT_PHY_REQ   0x7E0    /* Physical ECU Request */
#define ISOTP_DEFAULT_RESP_ID   0x7E8    /* ECU 1 Diagnostic Response */

/* Maximum Buffer Sizes */
#define ISOTP_MAX_MSG_SIZE      4095
#define CAN_FRAME_MAX_DLC       8

/* Frame Types (PCI: Protocol Control Information) */
typedef enum {
    ISOTP_PCI_TYPE_SF = 0x00,  /* Single Frame */
    ISOTP_PCI_TYPE_FF = 0x01,  /* First Frame */
    ISOTP_PCI_TYPE_CF = 0x02,  /* Consecutive Frame */
    ISOTP_PCI_TYPE_FC = 0x03   /* Flow Control */
} IsoTpPciType_t;

/* Flow Control Status */
typedef enum {
    ISOTP_FC_STATUS_CTS = 0x00,      /* Continue To Send */
    ISOTP_FC_STATUS_WAIT = 0x01,     /* Wait */
    ISOTP_FC_STATUS_OVERFLOW = 0x02  /* Buffer Overflow */
} IsoTpFlowStatus_t;

/* Transceiver State Machine */
typedef enum {
    ISOTP_STATE_IDLE = 0,
    ISOTP_STATE_SENDING_CF,
    ISOTP_STATE_WAIT_FC,
    ISOTP_STATE_RECEIVING_CF
} IsoTpState_t;

/* Raw CAN Frame */
typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[CAN_FRAME_MAX_DLC];
    bool is_extended;
} CanMessage_t;

/* ISO-TP Context */
typedef struct {
    uint32_t tx_id;
    uint32_t rx_id;
    IsoTpState_t state;
    uint8_t rx_buffer[ISOTP_MAX_MSG_SIZE];
    uint16_t rx_total_len;
    uint16_t rx_received_len;
    uint8_t rx_expected_sn;
    uint8_t tx_buffer[ISOTP_MAX_MSG_SIZE];
    uint16_t tx_total_len;
    uint16_t tx_sent_len;
    uint8_t tx_sn;
    uint8_t block_size;
    uint8_t st_min_ms;
} IsoTpLink_t;

/* Function Prototypes */
void IsoTp_Init(IsoTpLink_t *link, uint32_t tx_id, uint32_t rx_id);
bool IsoTp_ReceiveCanFrame(IsoTpLink_t *link, const CanMessage_t *can_msg, uint8_t *out_payload, uint16_t *out_len);
bool IsoTp_Transmit(IsoTpLink_t *link, const uint8_t *payload, uint16_t length, bool (*send_can_cb)(const CanMessage_t *msg));
void IsoTp_Poll(IsoTpLink_t *link, uint32_t delta_ms, bool (*send_can_cb)(const CanMessage_t *msg));

#ifdef __cplusplus
}
#endif

#endif /* ISO_TP_H */
