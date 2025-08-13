/*
 * bsp_board_EXP_1_0.c
 *
 *  Created on: Jun 12, 2025
 *      Author: Admin
 */

#include "bsp_board_EXP_1_2_0.h"

#include "sst.h"







/* SST callbacks ===========================================================*/
void SST_onStart(void) {
	  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //start SysTick Interrupt
}

void SST_onIdleCond(void) { /* NOTE: called with interrupts DISABLED */
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
    SST_PORT_CRIT_EXIT(); /* NOTE: enable interrupts for SS0 */

    __WFI();
}
