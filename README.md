# 🚗 Automotive ISO-TP & OBD-II Diagnostics Core

[![Language](https://img.shields.io/badge/Language-C99%20%2F%20C%2B%2B-00599C?style=flat-square&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![Standard](https://img.shields.io/badge/Standard-ISO%2015765--2%20%2F%20SAE%20J1979-E0234E?style=flat-square)](https://www.iso.org/)
[![Automotive](https://img.shields.io/badge/Domain-CAN%20Bus%20Diagnostics-238636?style=flat-square)](https://en.wikipedia.org/wiki/CAN_bus)
[![License](https://img.shields.io/badge/License-MIT-blue?style=flat-square)](LICENSE)

A high-performance, MISRA-C compliant **Automotive Diagnostics Protocol Stack** implementing **ISO 15765-2 (ISO-TP)** multi-frame network transport and **SAE J1979 / OBD-II** diagnostic service handling for microcontrollers (STM32, ESP32, NXP S32K) and CAN transceivers.

---

## 🔌 Hardware Circuit Diagram & Automotive OBD-II Interface

```
 +---------------------------------------------------------------------------------------------------+
 |                                   AUTOMOTIVE CAN BUS WIRING HARNESS                                |
 +---------------------------------------------------------------------------------------------------+

     [ Microcontroller (STM32/ESP32) ]
              |          |
         (SPI SCK)   (SPI MOSI)
              |          |
              v          v
     +-------------------------------+               +-------------------------------+
     |      MCP2515 CAN Controller   |               |     TJA1050 / MCP2551         |
     |   (Stand-Alone SPI Controller)|               |     High-Speed Transceiver    |
     |                               |               |                               |
     |                           TXD |-------------->| TXD                           |
     |                           RXD |<--------------| RXD            [ 120Ω Term ]  |
     |                           INT |               |                 Resistor      |
     |                           CS  |               |                     |         |
     +-------------------------------+               |           CANH -----+--------( Pin 6: CAN High )
                                                     |           CANL -----+--------( Pin 14: CAN Low )
                                                     +-------------------------------+
                                                                                            |
                                                                             +--------------v--------------+
                                                                             |   SAE J1962 OBD-II PORT     |
                                                                             |                             |
                                                                             |  Pin 4: Chassis GND         |
                                                                             |  Pin 5: Signal GND          |
                                                                             |  Pin 6: CAN-H (500 kbps)    |
                                                                             |  Pin 14: CAN-L (500 kbps)   |
                                                                             |  Pin 16: +12V Battery Power |
                                                                             +-----------------------------+
```

---

## 🏛️ System Architecture & Frame Flow

```mermaid
sequenceDiagram
    autonumber
    participant Tester as Diagnostic Tool / Scanner (0x7DF)
    participant Stack as ISO-TP Transport Layer
    participant OBD as OBD-II Service Layer (0x7E8)

    Tester->>Stack: Single Frame (SF): Mode 01 PID 0C (Engine RPM)
    Stack->>OBD: Dispatch Payload [01 0C]
    OBD->>OBD: Compute Live Metric: ((A*256)+B)/4
    OBD-->>Stack: Diagnostic Response [41 0C 26 48]
    Stack-->>Tester: CAN Response Frame (0x7E8): [04 41 0C 26 48 AA AA AA]
```

---

## ⚡ Supported Diagnostic Modes & PIDs

| Service Mode | PID | Name | Standard Formula | Unit |
| :--- | :--- | :--- | :--- | :--- |
| **Mode 01** | `0x0C` | Engine RPM | `((A * 256) + B) / 4` | RPM |
| **Mode 01** | `0x0D` | Vehicle Speed | `A` | km/h |
| **Mode 01** | `0x05` | Engine Coolant Temp | `A - 40` | °C |
| **Mode 01** | `0x11` | Throttle Position | `(A * 100) / 255` | % |
| **Mode 01** | `0x04` | Calculated Engine Load | `(A * 100) / 255` | % |
| **Mode 03** | - | Stored Trouble Codes | Read active DTCs (`P0301`, `P0420`) | 2-byte DTC |
| **Mode 04** | - | Clear Diagnostic Codes | Reset DTCs & MIL Check Engine Lamp | Acknowledged |

---

## 🛠️ Build & Verification

```bash
# Build diagnostic stack
gcc -Wall -Wextra -Iinclude src/iso_tp.c src/obd2.c src/mcp2515_can.c src/main.c -o obd_diag_core

# Execute the diagnostic stack simulation
./obd_diag_core

# Run the Python virtual test harness
python tools/virtual_ecu_fuzzer.py
```

---

## 👤 Author
* **Herambeswar Mandadapu** – [@mandadapuherambeswar-ux](https://github.com/mandadapuherambeswar-ux)
* **LinkedIn:** [Herambeswar Mandadapu](https://linkedin.com/in/herambeswar-mandadapu-5a977a385)
