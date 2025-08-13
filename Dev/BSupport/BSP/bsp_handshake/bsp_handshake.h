/*
 * hand_shake.h
 *
 *  Created on: Jul 21, 2025
 *      Author: HTSANG
 */

#ifndef APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_
#define APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_

#include "board.h"


#define MinBusy_GPIO_Port		GPIOB
#define MinBusy_Pin				LL_GPIO_PIN_11

#define DReady_GPIO_Port		GPIOC
#define DReady_Pin				LL_GPIO_PIN_8

#define ReadDone_GPIO_Port		GPIOC
#define ReadDone_Pin			LL_GPIO_PIN_9

#define Log_OverFlow_GPIO_Port	GPIOB
#define	Log_OverFlow_Pin		LL_GPIO_PIN_10

#define bsp_handshake_min_ready()	(LL_GPIO_ResetOutputPin(MinBusy_GPIO_Port, MinBusy_Pin))
#define bsp_handshake_min_busy()	(LL_GPIO_SetOutputPin(MinBusy_GPIO_Port, MinBusy_Pin))

#define bsp_handshake_spi_ready()	(LL_GPIO_ResetOutputPin(DReady_GPIO_Port, DReady_Pin))
#define bsp_handshake_spi_busy()	(LL_GPIO_SetOutputPin(DReady_GPIO_Port, DReady_Pin))

#define bsp_handshake_spi_check_finish()  (LL_GPIO_IsInputPinSet(ReadDone_GPIO_Port, ReadDone_Pin))

#define bsp_handshake_spi_clear_notification()	(LL_GPIO_SetOutputPin(Log_OverFlow_GPIO_Port, Log_OverFlow_Pin))
#define bsp_handshake_spi_buffer_full_notify()	(LL_GPIO_ResetOutputPin(Log_OverFlow_GPIO_Port, Log_OverFlow_Pin))

#endif /* APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_ */
