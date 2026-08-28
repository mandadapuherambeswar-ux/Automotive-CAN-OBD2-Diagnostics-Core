/**
 * @file mcp2515_can.h
 * @brief MCP2515 Standalone CAN Controller Driver Header
 * @author Herambeswar Mandadapu
 */

#ifndef MCP2515_CAN_H
#define MCP2515_CAN_H

#include "iso_tp.h"
#include <stdint.h>
#include <stdbool.h>

#define CAN_BAUD_500KBPS_16MHZ  0x00, 0xF0, 0x86
#define CAN_BAUD_500KBPS_8MHZ   0x00, 0x90, 0x82

typedef enum {
    CAN_MODE_NORMAL = 0x00,
    CAN_MODE_SLEEP  = 0x20,
    CAN_MODE_LOOP   = 0x40,
    CAN_MODE_LISTEN = 0x60,
    CAN_MODE_CONFIG = 0x80
} Mcp2515Mode_t;

bool MCP2515_Init(uint32_t baudrate);
bool MCP2515_SetMode(Mcp2515Mode_t mode);
bool MCP2515_SendMessage(const CanMessage_t *msg);
bool MCP2515_ReadMessage(CanMessage_t *msg);

#endif /* MCP2515_CAN_H */
