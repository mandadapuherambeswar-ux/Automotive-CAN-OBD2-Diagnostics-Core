/**
 * @file mcp2515_can.c
 * @brief MCP2515 Standalone CAN Controller Driver Implementation
 * @author Herambeswar Mandadapu
 */

#include "mcp2515_can.h"
#include <stdio.h>
#include <string.h>

bool MCP2515_Init(uint32_t baudrate)
{
    (void)baudrate;
    /*
     * 1. Software Reset
     * 2. Set CNF1, CNF2, CNF3 for 500 kbps @ 16MHz Crystal
     * 3. Set Filter 0 & Mask 0 for Functional Broadcast (0x7DF) & Physical (0x7E0)
     * 4. Switch to Normal Mode
     */
    return true;
}

bool MCP2515_SetMode(Mcp2515Mode_t mode)
{
    (void)mode;
    return true;
}

bool MCP2515_SendMessage(const CanMessage_t *msg)
{
    if (!msg) return false;
    /* Write to TXB0 (Transmit Buffer 0) and trigger Request-To-Send pin */
    return true;
}

bool MCP2515_ReadMessage(CanMessage_t *msg)
{
    if (!msg) return false;
    /* Read from RXB0 / RXB1 buffer */
    return false;
}
