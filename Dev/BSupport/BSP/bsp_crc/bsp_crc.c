/*
 * bsp_crc.c
 *
 *  Created on: Jul 28, 2025
 *      Author: HTSANG
 */


#include "bsp_crc.h"

uint32_t CRC_HW_Calculation(uint8_t *data_buffer, uint32_t length)
{
    if (length == 0) return 0;
    uint8_t* p_data = data_buffer;

    CRC->CR = CRC_CR_RESET;

    for (uint32_t i = 0; i < length; i ++)
    {
    	CRC->DR = *p_data;
    	p_data ++ ;
    }
    return CRC->DR;
}
