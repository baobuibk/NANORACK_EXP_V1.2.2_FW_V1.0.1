/*
 * bsp_debug.c
 *
 *  Created on: Jun 9, 2025
 *      Author: Admin
 */



void DBC_fault_handler(char const *const module, int const label) {
	/*
	 * NOTE: add here your application-specific error handling
	 */
	(void) module;
	(void) label;

	/* set PRIMASK to disable interrupts and stop SST right here */
	__asm volatile ("cpsid i");

//	set_red_LED_duty(0u); /*turn off red led*/

#ifndef NDEBUG
	/* blink LED*/
#endif
//	NVIC_SystemReset();
}
