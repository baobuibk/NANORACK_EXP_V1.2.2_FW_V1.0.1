#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/* Cấu trúc Circular Buffer */
typedef struct {
    uint8_t *buffer;    /* Con trỏ đến mảng tĩnh do người dùng cung cấp */
    uint32_t buffer_size; /* Kích thước của mảng tĩnh (byte) */
    uint32_t item_size;  /* Kích thước của mỗi phần tử (byte) */
    uint32_t max_items;  /* Số lượng phần tử tối đa */
    uint32_t head;      /* Vị trí đầu (để thêm phần tử) */
    uint32_t tail;      /* Vị trí đuôi (để lấy phần tử) */
    uint32_t count;     /* Số lượng phần tử hiện tại trong bộ đệm */
} circular_buffer_t;

/* Khởi tạo bộ đệm, yêu cầu người dùng cung cấp con trỏ đến mảng tĩnh */
uint32_t circular_buffer_init(circular_buffer_t * const me, uint8_t *static_buffer, uint32_t buffer_size, uint32_t max_items, uint32_t item_size);

/* Đưa một phần tử vào bộ đệm */
uint32_t circular_buffer_push(circular_buffer_t * const me, void const * const itemt) ;

/* Lấy một phần tử ra khỏi bộ đệm */
uint32_t circular_buffer_pop(circular_buffer_t * const me, void *  item) ;

/* Kiểm tra bộ đệm rỗng */
bool circular_buffer_is_empty(circular_buffer_t * const me);

/* Kiểm tra bộ đệm đầy */
bool circular_buffer_is_full(circular_buffer_t * const me);

#endif /* CIRCULAR_BUFFER_H */
