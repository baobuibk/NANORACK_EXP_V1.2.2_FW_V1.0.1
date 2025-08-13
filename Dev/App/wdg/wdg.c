/*
 * wdg.c
 *
 *  Created on: Jul 21, 2025
 *      Author: Admin
 */

#include <string.h>
#include "wdg.h"
#include "bsp_watchdog.h"
#include "stm32f7xx_ll_iwdg.h"
#include "sst.h"
#include "configs.h"
#include "error_codes.h"
#include "bsp_bkram.h"

struct soft_wdg
{
    uint32_t period_ms;
    uint32_t last_feed_time_ms;
};
struct wdg_state
{
    struct soft_wdg soft_wdgs[WDG_NUM_WDGS];
    wdg_triggered_cb triggered_cb;
};
struct wdg_no_init_vars {
    uint32_t magic;
    uint32_t reset_cause;
    uint32_t reset_wdg_id;
};

//static void EnableBackupRAM(void);
//static void DisableBackupRAM(void);

void validate_no_init_vars(void);

struct wdg_no_init_vars no_init_vars __attribute__((section (".bkpram")));

#define WDG_NO_INIT_VARS_MAGIC 0xdeaddead


struct wdg_state state;

int32_t wdg_init()
{
    memset(&state, 0, sizeof(state));
    return 0;
}

int32_t wdg_register(uint32_t wdg_id, uint32_t period_ms)
{
    struct soft_wdg * soft_wdg;

    if (wdg_id >= WDG_NUM_WDGS)
        return ERROR_INVALID_PARAM;

    soft_wdg = &state.soft_wdgs[wdg_id];
    soft_wdg->last_feed_time_ms = SST_getTick();
    soft_wdg->period_ms = period_ms;
    return 0;
}

int32_t wdg_unregister(uint32_t wdg_id)
{
    struct soft_wdg * soft_wdg;

    if (wdg_id >= WDG_NUM_WDGS)
        return ERROR_INVALID_PARAM;

    soft_wdg = &state.soft_wdgs[wdg_id];
    soft_wdg->period_ms = 0;
    return 0;
}

int32_t wdg_feed(uint32_t wdg_id)
{
    if (wdg_id >= WDG_NUM_WDGS)
        return ERROR_INVALID_PARAM;
    state.soft_wdgs[wdg_id].last_feed_time_ms = SST_getTick();
    return 0;
}

int32_t wdg_register_triggered_cb(wdg_triggered_cb triggered_cb)
{
    state.triggered_cb = triggered_cb;
    return 0;
}
void wdg_start_init_hdw_wdg(void)
{
//    wdg_start_hdw_wdg(CONFIG_WDG_INIT_TIMEOUT_MS);

}

void wdg_init_successful(void)
{

}

int32_t wdg_start_hdw_wdg(uint32_t timeout_ms)
{
    int32_t ctr;

    #define SANITY_CTR_LIMIT 		1000000
    #define LSI_FREQ_HZ 			32000
    #define WDG_PRESCALE 			64
    #define WDG_PRESCALE_SETTING 	LL_IWDG_PRESCALER_64
    #define WDG_CLK_FREQ_HZ 		(LSI_FREQ_HZ/WDG_PRESCALE)
    #define WDG_MAX_RL 				0xfff
    #define MS_PER_SEC 				1000
    #define WDG_MS_TO_RL(ms)		(((ms) * WDG_CLK_FREQ_HZ + MS_PER_SEC/2)/MS_PER_SEC - 1)

    ctr = WDG_MS_TO_RL(timeout_ms);
    if (ctr < 0)
        ctr = 0;
    else if (ctr > WDG_MAX_RL)
        return ERROR_INVALID_PARAM;

    LL_IWDG_Enable(IWDG);
    LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, WDG_PRESCALE_SETTING);
    LL_IWDG_SetReloadCounter(IWDG, ctr);
    for (ctr = 0; ctr < SANITY_CTR_LIMIT; ctr++) {
        if (LL_IWDG_IsReady(IWDG))
            break;
    }
    if (ctr >= SANITY_CTR_LIMIT)
        return ERROR_NOT_READY;

    // Stop the watchdog counter when the debugger stops the MCU.
    #ifdef DBGMCU_APB1FZR1_DBG_IWDG_STOP_Msk
        DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_IWDG_STOP_Msk;
    #elif defined DBGMCU_APB1_FZ_DBG_IWDG_STOP_Msk
        DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP_Msk;
    #endif

    return 0;
}
void wdg_feed_hdw(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}


void wdg_update()
{
    int32_t idx;
    struct soft_wdg* soft_wdg;
    bool wdg_triggered = false;

    for (idx = 0, soft_wdg = &state.soft_wdgs[0];
         idx < WDG_NUM_WDGS;
         idx++, soft_wdg++)
    {
        if (soft_wdg->period_ms != 0)
        {
            // We have to careful with race conditions, especially for
            // watchdogs fed from interrupt handlers.

            uint32_t last_feed_time_ms = soft_wdg->last_feed_time_ms;
            if (SST_getTick() - last_feed_time_ms > soft_wdg->period_ms)
            {
                wdg_triggered = true;
                update_no_init_vars(WDG_CAUSE_RESET_OTA, idx);
                if (state.triggered_cb != NULL)
                {
                    // This function will normally not return.
                    state.triggered_cb(idx);
                }
            }
        }
    }

    if (!wdg_triggered)
    {
		wdg_feed_hdw();
		WGD_TPL5010_PULSE_100NS();
		return ;
	}
}


//static void EnableBackupRAM(void)
//{
//    HAL_PWR_EnableBkUpAccess();
//    __HAL_RCC_BKPSRAM_CLK_ENABLE();
//    HAL_PWREx_EnableBkUpReg();
//    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_BRR));
//}
//
//static void DisableBackupRAM(void)
//{
//    HAL_PWREx_DisableBkUpReg();
//    __HAL_RCC_BKPSRAM_CLK_DISABLE();
//    HAL_PWR_DisableBkUpAccess();
//}

void update_no_init_vars(uint32_t reset_cause, uint32_t WDG_ID)
{
	EnableBackupRAM();
    memset(&no_init_vars, 0, sizeof(no_init_vars));
    no_init_vars.magic = WDG_NO_INIT_VARS_MAGIC;
    no_init_vars.reset_cause = reset_cause;
    no_init_vars.reset_wdg_id = WDG_ID;
    DisableBackupRAM();
}

void validate_no_init_vars(void)
{
	EnableBackupRAM();
    if (no_init_vars.magic != WDG_NO_INIT_VARS_MAGIC)
    {
        memset(&no_init_vars, 0, sizeof(no_init_vars));
        no_init_vars.magic = WDG_NO_INIT_VARS_MAGIC;
        no_init_vars.reset_cause = WDG_CAUSE_RESET_NORMAL;
        no_init_vars.reset_wdg_id = 0xFF;
    }

    DisableBackupRAM();
    return;
}

uint8_t System_On_Bootloader_Reset(void)
{
	update_no_init_vars(WDG_CAUSE_RESET_OTA, 0);

    __disable_irq();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    HAL_DeInit();
    NVIC_SystemReset();
    return 0;
}
