/*
 * circular_char_buffer.h
 *
 *  Created on: Jun 10, 2025
 *      Author: Admin
 */

#ifndef CIRCULAR_CHAR_BUFFER_H_
#define CIRCULAR_CHAR_BUFFER_H_


#include <stdint.h>
#include <stdbool.h>

/* Cấu trúc Circular Buffer */
typedef struct {
    uint8_t *buffer;    /* Con trỏ đến mảng tĩnh do người dùng cung cấp */
    uint32_t max_items;  /* Số lượng phần tử tối đa */
    uint32_t head;      /* Vị trí đầu (để thêm phần tử) */
    uint32_t tail;      /* Vị trí đuôi (để lấy phần tử) */
    uint32_t count;     /* Số lượng phần tử hiện tại trong bộ đệm */
} circular_char_buffer_t;

/* Khởi tạo bộ đệm, yêu cầu người dùng cung cấp con trỏ đến mảng tĩnh */
uint32_t circular_char_buffer_init(circular_char_buffer_t * const me, uint8_t *static_buffer,  uint32_t max_items) ;

/* Đưa một phần tử vào bộ đệm */
uint32_t circular_char_buffer_push(circular_char_buffer_t * const me, uint8_t const item) ;

/* Lấy một phần tử ra khỏi bộ đệm */
uint32_t circular_char_buffer_pop(circular_char_buffer_t * const me, uint8_t * const item) ;

/* Kiểm tra bộ đệm rỗng */
bool circular_char_buffer_is_empty(circular_char_buffer_t * const me);

/* Kiểm tra bộ đệm đầy */
bool circular_char_buffer_is_full(circular_char_buffer_t * const me);
/* Lấy số lượng phần tử còn trống trong bộ đệm */
uint32_t circular_char_buffer_get_free_space(circular_char_buffer_t const * const me) ;


#endif /* CIRCULAR_CHAR_BUFFER_H_ */
