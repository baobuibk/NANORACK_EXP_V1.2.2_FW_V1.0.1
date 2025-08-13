#include "circular_buffer.h"
#include <string.h>
#include "atomic.h"
#include "DBC_assert.h"
#include "error_codes.h"

DBC_MODULE_NAME("circular_buffer")
/* Khởi tạo bộ đệm */
uint32_t circular_buffer_init(circular_buffer_t * const me, uint8_t *static_buffer, uint32_t buffer_size, uint32_t max_items, uint32_t item_size) {
    if (me == NULL || static_buffer == NULL || max_items == 0 || item_size == 0 || buffer_size == 0) {
        return ERROR_INVALID_PARAM;
    }

    /* Kiểm tra xem bộ đệm tĩnh có đủ kích thước không */
    if (max_items * item_size > buffer_size) {
        return ERROR_OUT_OF_MEMORY;
    }

    me->buffer = static_buffer;
    me->buffer_size = buffer_size;
    me->item_size = item_size;
    me->max_items = max_items;
    me->head = 0;
    me->tail = 0;
    me->count = 0;

    return ERROR_OK;
}

/* Đưa một phần tử vào bộ đệm */
uint32_t circular_buffer_push(circular_buffer_t * const me, void const * const item) {
    DBC_ASSERT(1u, me != NULL);
    DBC_ASSERT(2u, item != NULL);
    uint32_t result;
    ENTER_CRITICAL();
	if (me == NULL || item == NULL ) {
        result = ERROR_INVALID_PARAM;
    }
    else if (circular_buffer_is_full(me)) {
        result = ERROR_BUFFER_FULL;
    }
	else
	{
		/* Tính vị trí trong bộ đệm */
		uint8_t *dest = me->buffer + (me->head * me->item_size);
		memcpy(dest, item, me->item_size);

		/* Cập nhật head và count */
		me->head = (me->head + 1) % me->max_items;
		me->count++;
		result = ERROR_OK;

	}
	EXIT_CRITICAL();
    return result;
}

/* Lấy một phần tử ra khỏi bộ đệm */
uint32_t circular_buffer_pop(circular_buffer_t * const me, void * item) {
	DBC_ASSERT(3u, me != NULL);
	DBC_ASSERT(4u, item != NULL);
    uint32_t result;
    ENTER_CRITICAL();
	if (me == NULL || item == NULL ) {
		result = ERROR_INVALID_PARAM;
    }
    else if (circular_buffer_is_empty(me)) {
        result = ERROR_BUFFER_EMPTY;
    }
	else
	{

		/* Tính vị trí trong bộ đệm */
		uint8_t *src = me->buffer + (me->tail * me->item_size);
		uint8_t *des = (uint8_t *)item;
		memcpy(des, src, me->item_size);


		/* Cập nhật tail và count */
		me->tail = (me->tail + 1) % me->max_items;
		me->count--;

		result = ERROR_OK;
	}
    EXIT_CRITICAL();
    return result;
}

/* Kiểm tra bộ đệm rỗng */
bool circular_buffer_is_empty(circular_buffer_t * const me) {
    if (me == NULL) {
        return true;
    }
    return me->count == 0;
}

/* Kiểm tra bộ đệm đầy */
bool circular_buffer_is_full(circular_buffer_t * const me) {
    if (me == NULL) {
        return true;
    }
    return me->count == me->max_items;
}
