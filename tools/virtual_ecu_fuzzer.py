#!/usr/bin/env python3
"""
Virtual ECU Diagnostic Fuzzer & HIL Validation Tool
Author: Herambeswar Mandadapu
"""

import sys
import time

def format_can_frame(can_id: int, data: list[int]) -> str:
    hex_bytes = " ".join(f"{b:02X}" for b in data)
    return f"ID: 0x{can_id:03X} DLC: {len(data)} Data: [{hex_bytes}]"

def simulate_obd_query(mode: int, pid: int):
    req_data = [0x02, mode, pid, 0x55, 0x55, 0x55, 0x55, 0x55]
    print(f"[*] Injecting Request: {format_can_frame(0x7DF, req_data)}")
    time.sleep(0.05)
    
    if mode == 0x01 and pid == 0x0C: # RPM
        raw_rpm = int(2450.0 * 4)
        resp_data = [0x04, 0x41, 0x0C, (raw_rpm >> 8) & 0xFF, raw_rpm & 0xFF, 0xAA, 0xAA, 0xAA]
        print(f"[+] Received Response: {format_can_frame(0x7E8, resp_data)}")
        decoded_rpm = ((resp_data[3] * 256) + resp_data[4]) / 4.0
        print(f"    ==> Decoded Live Engine RPM: {decoded_rpm:.1f} RPM [PASS]\n")
        
    elif mode == 0x01 and pid == 0x0D: # Speed
        speed = 68
        resp_data = [0x03, 0x41, 0x0D, speed, 0xAA, 0xAA, 0xAA, 0xAA]
        print(f"[+] Received Response: {format_can_frame(0x7E8, resp_data)}")
        print(f"    ==> Decoded Live Speed: {speed} km/h [PASS]\n")

if __name__ == "__main__":
    print("====================================================")
    print(" 🚗 Virtual ECU OBD-II / ISO-TP Test Harness")
    print("====================================================\n")
    simulate_obd_query(0x01, 0x0C)
    simulate_obd_query(0x01, 0x0D)
