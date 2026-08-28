/**
 * @file obd2.c
 * @brief OBD-II Diagnostic Service Dispatcher Implementation
 * @author Herambeswar Mandadapu
 */

#include "obd2.h"
#include <string.h>

static VehicleState_t s_vehicle_state;

void OBD2_Init(VehicleState_t *initial_state)
{
    if (initial_state) {
        memcpy(&s_vehicle_state, initial_state, sizeof(VehicleState_t));
    } else {
        memset(&s_vehicle_state, 0, sizeof(VehicleState_t));
        s_vehicle_state.engine_rpm = 2450.0f;
        s_vehicle_state.vehicle_speed_kmh = 68;
        s_vehicle_state.coolant_temp_c = 88;
        s_vehicle_state.throttle_position_pct = 22.4f;
        s_vehicle_state.calculated_engine_load_pct = 34.0f;
        s_vehicle_state.maf_air_flow_gps = 14.8f;
        s_vehicle_state.active_dtc_count = 1;
        strcpy(s_vehicle_state.active_dtcs[0].dtc_string, "P0301");
        s_vehicle_state.active_dtcs[0].raw_code = 0x0301;
    }
}

void OBD2_UpdateState(const VehicleState_t *new_state)
{
    if (new_state) {
        memcpy(&s_vehicle_state, new_state, sizeof(VehicleState_t));
    }
}

uint16_t OBD2_ProcessRequest(const uint8_t *req, uint16_t req_len, uint8_t *resp, uint16_t max_resp_len)
{
    if (!req || req_len == 0 || !resp || max_resp_len < 8) return 0;

    uint8_t mode = req[0];

    switch (mode) {
        case OBD2_MODE_SHOW_CURRENT_DATA: {
            if (req_len < 2) return 0;
            uint8_t pid = req[1];
            resp[0] = (uint8_t)(mode + OBD2_RESPONSE_OFFSET);
            resp[1] = pid;

            switch (pid) {
                case PID_SUPPORTED_PIDS_1_20:
                    /* Bitmask for supported PIDs: 01, 04, 05, 0C, 0D, 11 */
                    resp[2] = 0xBE;
                    resp[3] = 0x3E;
                    resp[4] = 0xB8;
                    resp[5] = 0x11;
                    return 6;

                case PID_ENGINE_RPM: {
                    /* Formula: RPM = ((A * 256) + B) / 4 */
                    uint16_t raw_rpm = (uint16_t)(s_vehicle_state.engine_rpm * 4.0f);
                    resp[2] = (uint8_t)(raw_rpm >> 8);
                    resp[3] = (uint8_t)(raw_rpm & 0xFF);
                    return 4;
                }

                case PID_VEHICLE_SPEED:
                    /* Speed in km/h: A */
                    resp[2] = s_vehicle_state.vehicle_speed_kmh;
                    return 3;

                case PID_ENGINE_COOLANT_TEMP:
                    /* Formula: Temp = A - 40 */
                    resp[2] = (uint8_t)(s_vehicle_state.coolant_temp_c + 40);
                    return 3;

                case PID_THROTTLE_POSITION:
                    /* Formula: Throttle = (A * 100) / 255 */
                    resp[2] = (uint8_t)((s_vehicle_state.throttle_position_pct * 255.0f) / 100.0f);
                    return 3;

                case PID_CALCULATED_ENGINE_LOAD:
                    /* Formula: Load = (A * 100) / 255 */
                    resp[2] = (uint8_t)((s_vehicle_state.calculated_engine_load_pct * 255.0f) / 100.0f);
                    return 3;

                default:
                    /* Unsupported PID Negative Response */
                    resp[0] = OBD2_NEGATIVE_RESPONSE;
                    resp[1] = mode;
                    resp[2] = 0x12; /* SubFunctionNotSupported */
                    return 3;
            }
        }

        case OBD2_MODE_SHOW_STORED_DTCS: {
            resp[0] = (uint8_t)(mode + OBD2_RESPONSE_OFFSET);
            resp[1] = s_vehicle_state.active_dtc_count;
            uint16_t offset = 2;
            for (uint8_t i = 0; i < s_vehicle_state.active_dtc_count && i < 4; i++) {
                resp[offset++] = (uint8_t)(s_vehicle_state.active_dtcs[i].raw_code >> 8);
                resp[offset++] = (uint8_t)(s_vehicle_state.active_dtcs[i].raw_code & 0xFF);
            }
            return offset;
        }

        case OBD2_MODE_CLEAR_DTCS: {
            s_vehicle_state.active_dtc_count = 0;
            resp[0] = (uint8_t)(mode + OBD2_RESPONSE_OFFSET);
            return 1;
        }

        default:
            resp[0] = OBD2_NEGATIVE_RESPONSE;
            resp[1] = mode;
            resp[2] = 0x11; /* ServiceNotSupported */
            return 3;
    }
}
