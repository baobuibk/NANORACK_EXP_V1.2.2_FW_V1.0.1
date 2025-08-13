/*
 * bsp_watchdog.c
 *
 *  Created on: Jul 23, 2025
 *      Author: Admin
 */
#include "bsp_watchdog.h"
#include "wdg.h"



void bsp_tpl5010_feed()
{
	WGD_TPL5010_PULSE_100NS();
}

void bsp_enable_backup_sram(void) {
    /* Bật quyền truy cập vào Backup Domain */
    HAL_PWR_EnableBkUpAccess();

    /* Bật đồng hồ cho Backup SRAM */
    __HAL_RCC_BKPSRAM_CLK_ENABLE();

    /* Bật Backup Regulator */
    HAL_PWREx_EnableBkUpReg();

    /* Tùy chọn: Vô hiệu hóa quyền truy cập vào Backup Domain sau khi cấu hình */
    __HAL_RCC_BKPSRAM_CLK_DISABLE();
    HAL_PWR_DisableBkUpAccess();
}
