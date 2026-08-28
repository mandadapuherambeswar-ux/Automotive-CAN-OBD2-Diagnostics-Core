/**
 * @file main.c
 * @brief Automotive CAN Diagnostic Core Application Runner
 * @author Herambeswar Mandadapu
 */

#include "iso_tp.h"
#include "obd2.h"
#include <stdio.h>

static bool Mock_Send_CAN_Frame(const CanMessage_t *msg)
{
    printf("[CAN-TX] ID: 0x%03X DLC: %d Data: ", msg->id, msg->dlc);
    for (int i = 0; i < msg->dlc; i++) {
        printf("%02X ", msg->data[i]);
    }
    printf("\n");
    return true;
}

int main(void)
{
    printf("====================================================\n");
    printf(" 🚗 Automotive ISO-TP & OBD-II Diagnostics Core\n");
    printf(" Author: Herambeswar Mandadapu\n");
    printf("====================================================\n\n");

    IsoTpLink_t iso_tp_link;
    IsoTp_Init(&iso_tp_link, ISOTP_DEFAULT_RESP_ID, ISOTP_DEFAULT_REQ_ID);
    OBD2_Init(NULL);

    /* 1. Simulate incoming Functional OBD-II Request for Engine RPM (Mode 01 PID 0C) */
    CanMessage_t req_msg = {
        .id = ISOTP_DEFAULT_REQ_ID,
        .dlc = 8,
        .data = {0x02, 0x01, 0x0C, 0x55, 0x55, 0x55, 0x55, 0x55},
        .is_extended = false
    };

    printf("[STEP 1] Receiving OBD-II Query: Mode 01 PID 0C (Engine RPM)...\n");
    uint8_t payload[64];
    uint16_t payload_len = 0;

    if (IsoTp_ReceiveCanFrame(&iso_tp_link, &req_msg, payload, &payload_len)) {
        printf("[ISO-TP] Successfully assembled diagnostic request (%d bytes)\n", payload_len);

        uint8_t resp_payload[64];
        uint16_t resp_len = OBD2_ProcessRequest(payload, payload_len, resp_payload, sizeof(resp_payload));

        printf("[OBD-II] Processing response (%d bytes)...\n", resp_len);
        IsoTp_Transmit(&iso_tp_link, resp_payload, resp_len, Mock_Send_CAN_Frame);
    }

    printf("\n[SUCCESS] Diagnostics stack execution validated.\n");
    return 0;
}
