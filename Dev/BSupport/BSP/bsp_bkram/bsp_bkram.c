/*
 * bsp_bkram.c
 *
 *  Created on: Jul 28, 2025
 *      Author: HTSANG
 */

#include "bsp_bkram.h"
#include "main.h"

void EnableBackupRAM(void)
{
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKPSRAM_CLK_ENABLE();
    HAL_PWREx_EnableBkUpReg();
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_BRR));
}

void DisableBackupRAM(void)
{
    HAL_PWREx_DisableBkUpReg();
    __HAL_RCC_BKPSRAM_CLK_DISABLE();
    HAL_PWR_DisableBkUpAccess();
}
