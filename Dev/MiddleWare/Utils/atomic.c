/*
 * atomic.c
 *
 *  Created on: Apr 29, 2025
 *      Author: Admin
 */


#include "atomic.h"
//#include "stm32f4xx.h"

// Biến đếm để theo dõi số lần lồng nhau của critical section
volatile uint32_t critical_nest_count = 0;

// Biến lưu trạng thái interrupt trước khi vào critical section
volatile uint32_t primask_state = 0;

uint32_t atomic_inc(volatile uint32_t *var) {
    uint32_t old_val;
    ENTER_CRITICAL();
    old_val = *var;
    *var = old_val + 1;
    EXIT_CRITICAL();
    return old_val;
}

uint32_t atomic_dec(volatile uint32_t *var) {
    uint32_t old_val;
    ENTER_CRITICAL();
    old_val = *var;
    *var = old_val - 1;
    EXIT_CRITICAL();
    return old_val;
}

uint32_t atomic_set_and_get_old(volatile uint32_t *var, uint32_t new_val) {
    uint32_t old_val;
    ENTER_CRITICAL();
    old_val = *var;
    *var = new_val;
    EXIT_CRITICAL();
    return old_val;
}

uint32_t atomic_compare_and_swap(volatile uint32_t *var, uint32_t old_val, uint32_t new_val) {
    uint32_t current_val;
    ENTER_CRITICAL();
    current_val = *var;
    if (current_val == old_val) {
        *var = new_val;
    }
    EXIT_CRITICAL();
    return current_val;
}
