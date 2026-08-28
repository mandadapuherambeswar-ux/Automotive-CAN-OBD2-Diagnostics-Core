/**
 * @file obd2.h
 * @brief OBD-II (SAE J1979) / UDS Diagnostic Services Layer
 * @author Herambeswar Mandadapu
 */

#ifndef OBD2_H
#define OBD2_H

#include "iso_tp.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OBD-II Service Modes */
#define OBD2_MODE_SHOW_CURRENT_DATA     0x01
#define OBD2_MODE_SHOW_FREEZE_FRAME     0x02
#define OBD2_MODE_SHOW_STORED_DTCS      0x03
#define OBD2_MODE_CLEAR_DTCS            0x04
#define OBD2_MODE_VEHICLE_INFO          0x09
#define OBD2_RESPONSE_OFFSET            0x40
#define OBD2_NEGATIVE_RESPONSE          0x7F

/* Common Standard PIDs (Mode 01) */
#define PID_SUPPORTED_PIDS_1_20         0x00
#define PID_MONITOR_STATUS              0x01
#define PID_FREEZE_DTC                  0x02
#define PID_FUEL_SYSTEM_STATUS          0x03
#define PID_CALCULATED_ENGINE_LOAD      0x04
#define PID_ENGINE_COOLANT_TEMP         0x05
#define PID_SHORT_TERM_FUEL_TRIM_1      0x06
#define PID_LONG_TERM_FUEL_TRIM_1       0x07
#define PID_ENGINE_RPM                  0x0C
#define PID_VEHICLE_SPEED               0x0D
#define PID_INTAKE_AIR_TEMP             0x0F
#define PID_MAF_AIR_FLOW_RATE           0x10
#define PID_THROTTLE_POSITION           0x11
#define PID_SUPPORTED_PIDS_21_40        0x20

/* Diagnostic Trouble Code (DTC) Structure */
typedef struct {
    char dtc_string[6]; /* e.g. "P0300" */
    uint16_t raw_code;
    uint8_t status_byte;
} DiagnosticTroubleCode_t;

/* Simulated Vehicle Live Metrics */
typedef struct {
    float engine_rpm;
    uint8_t vehicle_speed_kmh;
    int8_t coolant_temp_c;
    float throttle_position_pct;
    float calculated_engine_load_pct;
    float maf_air_flow_gps;
    uint8_t active_dtc_count;
    DiagnosticTroubleCode_t active_dtcs[8];
} VehicleState_t;

/* Diagnostic Handler Prototypes */
void OBD2_Init(VehicleState_t *initial_state);
void OBD2_UpdateState(const VehicleState_t *new_state);
uint16_t OBD2_ProcessRequest(const uint8_t *req, uint16_t req_len, uint8_t *resp, uint16_t max_resp_len);

#ifdef __cplusplus
}
#endif

#endif /* OBD2_H */
