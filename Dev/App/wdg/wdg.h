/*
 * wdg.h
 *
 *  Created on: Jul 21, 2025
 *      Author: Admin
 */

#ifndef APP_WDG_WDG_H_
#define APP_WDG_WDG_H_
#include "stdint.h"


typedef void (*wdg_triggered_cb)(uint32_t wdg_id);

// Core module interface functions.
int32_t wdg_init(void);
int32_t wdg_start(void);
void wdg_update(void);

// Other APIs.
int32_t wdg_register(uint32_t wdg_id, uint32_t period_ms);
int32_t wdg_unregister(uint32_t wdg_id);
int32_t wdg_feed(uint32_t wdg_id);
int32_t wdg_register_triggered_cb(wdg_triggered_cb triggered_cb);
void wdg_start_init_hdw_wdg(void);
void wdg_init_successful(void);
int32_t wdg_start_hdw_wdg(uint32_t timeout_ms);
void wdg_feed_hdw(void);

void update_no_init_vars(uint32_t reset_cause, uint32_t WDG_ID);
uint8_t System_On_Bootloader_Reset(void);

#endif /* APP_WDG_WDG_H_ */
