/*
 * min_shell_command.h
 *
 *  Created on: Jun 13, 2025
 *      Author: Admin
 */

#ifndef APP_MIN_SHELL_MIN_SHELL_COMMAND_H_
#define APP_MIN_SHELL_MIN_SHELL_COMMAND_H_


#include "min_shell.h"
#include "bsp_handshake.h"

#include <stdint.h>

// =================================================================
// Command IDs (Maximum ID: 63)
// =================================================================
#define PLEASE_RESET_CMD                  0x00
#define PLEASE_RESET_ACK                  0x01

#define TEST_CONNECTION_CMD               0x02
#define TEST_CONNECTION_ACK               0x03

#define SET_WORKING_RTC_CMD               0x04
#define SET_WORKING_RTC_ACK               0x05

#define REVERSE_6                         0x06
#define REVERSE_7                         0x07

#define SET_NTC_CONTROL_CMD               0x08
#define SET_NTC_CONTROL_ACK               0x09

#define SET_TEMP_PROFILE_CMD              0x0A
#define SET_TEMP_PROFILE_ACK              0x0B

#define START_TEMP_PROFILE_CMD            0x0C
#define START_TEMP_PROFILE_ACK            0x0D

#define STOP_TEMP_PROFILE_CMD             0x0E
#define STOP_TEMP_PROFILE_ACK             0x0F

#define SET_OVERRIDE_TEC_PROFILE_CMD      0x10
#define SET_OVERRIDE_TEC_PROFILE_ACK      0x11

#define START_OVERRIDE_TEC_PROFILE_CMD    0x12
#define START_OVERRIDE_TEC_PROFILE_ACK    0x13

#define STOP_OVERRIDE_TEC_PROFILE_CMD     0x14
#define STOP_OVERRIDE_TEC_PROFILE_ACK     0x15

#define SET_PDA_PROFILE_CMD               0x16
#define SET_PDA_PROFILE_ACK               0x17

#define SET_LASER_INTENSITY_CMD           0x18
#define SET_LASER_INTENSITY_ACK           0x19

#define SET_POSITION_CMD                  0x1A
#define SET_POSITION_ACK                  0x1B

#define START_SAMPLE_CYCLE_CMD            0x1C
#define START_SAMPLE_CYCLE_ACK            0x1D

#define GET_INFO_SAMPLE_CMD               0x1E
#define GET_INFO_SAMPLE_ACK               0x1F

#define GET_CHUNK_CMD                     0x20
#define GET_CHUNK_ACK                     0x21

#define GET_CHUNK_CRC_CMD                 0x22
#define GET_CHUNK_CRC_ACK                 0x23

#define GET_LASER_CURRENT_DATA_CMD        0x24
#define GET_LASER_CURRENT_DATA_ACK        0x25

#define GET_LASER_CURRENT_CRC_CMD         0x26
#define GET_LASER_CURRENT_CRC_ACK         0x27


#define MANUAL_SET_LASER_INTENSITY_CMD                 0x28
#define MANUAL_SET_LASER_INTENSITY_ACK                 0x29

#define MANUAL_TURN_ON_LASER_CMD                       0x2A
#define MANUAL_TURN_ON_LASER_ACK                       0x2B

#define MANUAL_TURN_OFF_LASER_CMD                      0x2C
#define MANUAL_TURN_OFF_LASER_ACK                      0x2D

#define SET_LASER_INT_CMD                 0x2E
#define SET_LASER_INT_ACK                 0x2F

#define SET_LASER_EXT_CMD                 0x30
#define SET_LASER_EXT_ACK                 0x31

#define GET_LASER_CURRENT_CMD             0x32
#define GET_LASER_CURRENT_ACK             0x33

#define GET_LOG_CMD							0x34
#define GET_LOG_ACK							0x35


#define CUSTOM_COMMAND_CMD					0x36
#define CUSTOM_COMMAND_ACK					0x37
//----
#define PING_CMD                                    0x38  ///< Ping request
#define PONG_CMD                                    0x39  ///< Pong response
//----
#define	MIN_RESP_NAK		                        0x3A
#define	MIN_RESP_ACK		                        0x3B
//----
#define	MIN_RESP_WRONG		                        0x3C
#define	MIN_RESP_DONE		                        0x3D
//----
#define MIN_RESP_FAIL		                        0x3E
#define	MIN_RESP_OK			                        0x3F
//----
#define MIN_ERROR_OK								0xFF


/**
 * @brief Command handler function type.
 * @param ctx Pointer to the MIN context.
 * @param payload Pointer to the received payload data.
 * @param len Length of the payload.
 */
typedef void (*MIN_CommandHandler)(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len);

/**
 * @brief Structure to map command IDs to their handlers.
 */
typedef struct {
    uint8_t id;                    ///< Command ID
    MIN_CommandHandler handler;    ///< Handler function for the command
} MIN_Command_t;

/**
 * @brief Gets the command table.
 * @return Pointer to the command table.
 */
const MIN_Command_t *MIN_GetCommandTable(void);

/**
 * @brief Gets the size of the command table.
 * @return Number of entries in the command table.
 */
int MIN_GetCommandTableSize(void);

#endif /* APP_MIN_SHELL_MIN_SHELL_COMMAND_H_ */
