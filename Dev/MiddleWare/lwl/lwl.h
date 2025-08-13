#ifndef _LWL_H_
#define _LWL_H_

/*
 * @brief Interface declaration of lwl module.
 *
 * See implementation file for information about this module.
 *
 * MIT License
 * 
 * Copyright (c) 2021 Eugene R Schroeder
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Common macros
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Public (global) externs
////////////////////////////////////////////////////////////////////////////////

// Following variable is global to allow efficient access by macros,
// but it is considered private.
// Macros for logging
#define LWL_1(a) (uint32_t)(a)
#define LWL_2(a) (uint32_t)(a), (uint32_t)(a) >> 8
#define LWL_3(a) (uint32_t)(a), (uint32_t)(a) >> 8, (uint32_t)(a) >> 16
#define LWL_4(a) (uint32_t)(a), (uint32_t)(a) >> 8, (uint32_t)(a) >> 16, (uint32_t)(a) >> 24


////////////////////////////////////////////////////////////////////////////////
// Public (global) function declarations
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    LWL_EXP_INVALID = 0,                  // ID 0: Reserved
    LWL_EXP_TIMESTAMP,                    // ID 1: System timestamp log

    LWL_EXP_TEMP_SINGLE_NTC,              // ID 2: Log single NTC value
    LWL_EXP_TEMP_PROFILE_SET,            // ID 3: Set temperature control profile

    LWL_EXP_TEMP_MANUAL_MODE,            // ID 4: Manual temperature control mode
    LWL_EXP_TEMP_AUTO_MODE,              // ID 5: Automatic temperature control mode
    LWL_EXP_TEMP_COOLING,                // ID 6: Cooling active
    LWL_EXP_TEMP_HEATING,                // ID 7: Heating active
    LWL_EXP_TEMP_ERROR,                  // ID 8: NTC error (primary/secondary)

    LWL_EXP_TEMP_TEC_OVERRIDE_PROFILE,   // ID 9: TEC override profile configuration
    LWL_EXP_TEMP_TEC_OVERRIDE_ON,        // ID 10: Enable TEC override
    LWL_EXP_TEMP_TEC_OVERRIDE_OFF,       // ID 11: Disable TEC override

    LWL_EXP_TEC_MANUAL_ON,               // ID 12: Manually turn on TEC
    LWL_EXP_TEC_MANUAL_OFF,              // ID 13: Manually turn off TEC
    LWL_EXP_TEC_AUTO_ON,                 // ID 14: Automatically turn on TEC
    LWL_EXP_TEC_AUTO_OFF,                // ID 15: Automatically turn off TEC

    LWL_EXP_HEATER_MANUAL_ON,            // ID 16: Manually turn on heater
    LWL_EXP_HEATER_MANUAL_OFF,           // ID 17: Manually turn off heater
    LWL_EXP_HEATER_AUTO_ON,              // ID 18: Automatically turn on heater
    LWL_EXP_HEATER_AUTO_OFF,             // ID 19: Automatically turn off heater

    LWL_EXP_LASER_INT_MANUAL_ON,         // ID 20: Internal laser manual ON
    LWL_EXP_LASER_INT_MANUAL_OFF,        // ID 21: Internal laser manual OFF
    LWL_EXP_LASER_INT_SAMPLE_ON,         // ID 22: Internal laser sampling ON
    LWL_EXP_LASER_INT_SAMPLE_OFF,        // ID 23: Internal laser sampling OFF

    LWL_EXP_LASER_EXT_MANUAL_ON,         // ID 24: External laser manual ON
    LWL_EXP_LASER_EXT_MANUAL_OFF,        // ID 25: External laser manual OFF
    LWL_EXP_LASER_EXT_SAMPLE_ON,         // ID 26: External laser sampling ON
    LWL_EXP_LASER_EXT_SAMPLE_OFF,        // ID 27: External laser sampling OFF

    LWL_EXP_PHOTO_SAMPLE_ON,             // ID 28: Start sampling photodiode (photo)
    LWL_EXP_PHOTO_SAMPLE_OFF,            // ID 29: Stop sampling photodiode (photo)

    LWL_EXP_SET_PHOTO_PROFILE,           // ID 30: Configure photo sampling profile
    LWL_EXP_SET_LASER_INTENSITY,         // ID 31: Set laser intensity
    LWL_EXP_SET_LASER_PHOTO_INDEX,       // ID 32: Set laser and photo sensor index
    LWL_EXP_START,                       // ID 33: Start experiment
    LWL_EXP_STOP,                        // ID 34: Stop experiment

    LWL_EXP_SYS_RESET_OTA,              // ID 35: OTA system reset trigger
	LWL_EXP_SET_RTC,                    // ID 36:

	LWL_EXP_TRANS_PHOTO_DATA,           // ID 37:
	LWL_EXP_TRANS_CURRENT_DATA,         // ID 38:
	LWL_EXP_TRANS_LOG_DATA,             // ID 39:

    // OBC STM32 block
    OBC_STM32_________________LOG,      // ID 40
    OBC_STM32_TEST_LOG,                 // ID 41
    OBC_STM32_STARTUP,                  // ID 42
	OBC_STM32_BOOTING,					// ID 43
	OBC_STM32_LOGTEST,					// ID 44

	OBC_STM32_INIT_STEP,				// ID 45
	OBC_STM32_MISS_HEARTBEAT,			// ID 46

	OBC_STM32_RELEASE,					// ID 47
	OBC_STM32_LOCKIN,					// ID 48
	OBC_STM32_CLEAN,					// ID 49

	OBC_STM32_CM4_SCRIP_NAK,			// ID 50
	OBC_STM32_WRITE_FATFS_FAIL,			// ID 51

	OBC_STM32_RTC_SCRIPT_SET,			// ID 52
	OBC_STM32_TIME_POINT_START,			// ID 53
	OBC_STM32_ROUTINE_INIT_STEP,		// ID 54
	OBC_STM32_ROUTINE_DLS_STEP,			// ID 55
	OBC_STM32_ROUTINE_CAM_STEP,			// ID 56
	OBC_STM32_ROUTINE_RETURN, 			// ID 57

	OBC_STM32_MIN_CALLBACK,				// ID 58
	OBC_STM32_MODFSP_CALLBACK			// ID 59
} log_msg_id_t;


void lwl_start(void);

// Other APIs.
void lwl_rec(uint8_t id, int32_t num_arg_bytes, ...);
void lwl_enable(bool on);
void lwl_dump(void);
uint8_t* lwl_get_buffer(uint32_t* len);
void LWL(uint8_t id, ...) ;
// The special __COUNTER__ macro (not official C but supported by many
// compilers) is used to generate LWL IDs.

void lwl_buffer_full_notify(void);
void lwl_clear_notification(void);

uint16_t * lwl_get_full_buffer_addr(void);
uint32_t lwl_log_send_to_spi(void);

#endif // _LWL_H_
