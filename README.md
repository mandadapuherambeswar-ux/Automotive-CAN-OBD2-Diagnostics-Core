# 🚗 Automotive ISO-TP & OBD-II Diagnostics Core

[![Language](https://img.shields.io/badge/Language-C99%20%2F%20C%2B%2B-00599C?style=flat-square&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![Standard](https://img.shields.io/badge/Standard-ISO%2015765--2%20%2F%20SAE%20J1979-E0234E?style=flat-square)](https://www.iso.org/)
[![Automotive](https://img.shields.io/badge/Domain-CAN%20Bus%20Diagnostics-238636?style=flat-square)](https://en.wikipedia.org/wiki/CAN_bus)
[![License](https://img.shields.io/badge/License-MIT-blue?style=flat-square)](LICENSE)

A high-performance, MISRA-C compliant **Automotive Diagnostics Protocol Stack** implementing **ISO 15765-2 (ISO-TP)** multi-frame network transport and **SAE J1979 / OBD-II** diagnostic service handling for microcontrollers (STM32, ESP32, NXP S32K) and CAN transceivers.

---

## 🏛️ System Architecture

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

## ⚡ Core Features

1. **Full ISO 15765-2 Transport Protocol**:
   - Handles **Single Frames (SF)**, **First Frames (FF)**, **Consecutive Frames (CF)**, and **Flow Control (FC)** with configurable Block Size (`BS`) and Separation Time (`STmin`).
   - Supports segmented payloads up to **4,095 bytes** with deterministic buffer bounds and timeout guards.
2. **SAE J1979 Diagnostic Service Handlers**:
   - **Mode 01**: Live sensor telemetry readouts (Engine RPM, Speed, Coolant Temp, Throttle Position, Engine Load, MAF).
   - **Mode 03**: Reading confirmed Diagnostic Trouble Codes (DTCs) e.g., `P0301`, `P0420`.
   - **Mode 04**: Clearing DTCs and resetting MIL status.
   - **Mode 09**: Vehicle identification metadata.
3. **Hardware Agnostic & Mocking Suite**:
   - Zero hardcoded hardware dependencies. Runs seamlessly over hardware CAN controllers (MCP2515, STM32 bxCAN, SocketCAN) or virtual software buses.
   - Python-based test harness (`tools/virtual_ecu_fuzzer.py`) for automated fuzzing and regression testing.

---

## 🛠️ Build & Verification

```bash
# Build with GCC / Clang
gcc -Wall -Wextra -Iinclude src/iso_tp.c src/obd2.c src/main.c -o obd_diag_core

# Execute the diagnostic stack simulation
./obd_diag_core

# Run the Python virtual test harness
python tools/virtual_ecu_fuzzer.py
```

---

## 👤 Author
* **Herambeswar Mandadapu** – [@mandadapuherambeswar-ux](https://github.com/mandadapuherambeswar-ux)
* **LinkedIn:** [Herambeswar Mandadapu](https://linkedin.com/in/herambeswar-mandadapu-5a977a385)
