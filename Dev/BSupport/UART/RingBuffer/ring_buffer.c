/*
 * ringbuffer.c
 *
 *  Created on: Feb 24, 2025
 *      Author: CAO HIEU
 */

#include <stdint.h>
#include <stdbool.h>

#include "ring_buffer.h"

void RingBuffer_Create(s_RingBufferType *rb, uint32_t id, const char *name,
                         RingBufElement *buffer, RingBufCtr max_size)
{
    rb->id = id;

    strncpy(rb->name, name, RINGBUFFER_NAME_MAX_LEN - 1);
    rb->name[RINGBUFFER_NAME_MAX_LEN - 1] = '\0';

    rb->buffer = buffer;
    rb->max_size = max_size;

    atomic_store_explicit(&rb->head, 0U, memory_order_release);
    atomic_store_explicit(&rb->tail, 0U, memory_order_release);
}

_Bool RingBuffer_Put(s_RingBufferType * const rb, RingBufElement const el)
{
//	__disable_irq();
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_relaxed) + 1U;
    if (head == rb->max_size)
    {
        head = 0U;
    }

    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    if (head != tail) // buffer NOT full?
    {
        rb->buffer[atomic_load_explicit(&rb->head, memory_order_relaxed)] = el;
        atomic_store_explicit(&rb->head, head, memory_order_release);
//        __enable_irq();
        return true;
    }
    else
    {
//    	__enable_irq();
        return false;
    }
}

_Bool RingBuffer_Get(s_RingBufferType * const rb, RingBufElement *pel)
{
//	__disable_irq();
    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_acquire);
    if (head != tail)  // buffer NOT empty?
    {
        *pel = rb->buffer[tail];
        ++tail;
        if (tail == rb->max_size)
        {
            tail = 0U;
        }
        atomic_store_explicit(&rb->tail, tail, memory_order_release);
//    	__enable_irq();
        return true;
    }
    else
    {
//    	__enable_irq();
        return false;
    }
}

// Function to calculate the number of free slots in the ring buffer.
// Note: We always leave one empty slot to distinguish between an empty and a full buffer.
RingBufCtr RingBuffer_NumFreeSlots(s_RingBufferType * const rb)
{
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_acquire);
    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);

    if (head == tail)
    {
        return (RingBufCtr)(rb->max_size - 1U);
    }
    else if (head < tail)
    {
        return (RingBufCtr)(tail - head - 1U);
    }
    else
    {
        return (RingBufCtr)(rb->max_size + tail - head - 1U);
    }
}

// Function to process all elements in the ring buffer using a callback handler.
void RingBuffer_ProcessAll(s_RingBufferType * const rb, RingBufHandler handler)
{
    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_acquire);

    while (head != tail)  // There are still elements in the buffer
    {
        (*handler)(rb->buffer[tail]);  // Process the element at the tail position
        ++tail;
        if (tail == rb->max_size)     // If the end of the array is reached, wrap around
        {
            tail = 0U;
        }
        atomic_store_explicit(&rb->tail, tail, memory_order_release);
    }
}

RingBufCtr RingBuffer_GetHead(s_RingBufferType * const rb)
{
    return atomic_load_explicit(&rb->head, memory_order_acquire);
}

RingBufCtr RingBuffer_GetTail(s_RingBufferType * const rb)
{
    return atomic_load_explicit(&rb->tail, memory_order_acquire);
}

RingBufElement RingBuffer_GetHeadData(s_RingBufferType * const rb)
{
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_acquire);
    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    if (head == tail)
    {
        return 0;
    }
    RingBufCtr index = (head == 0U) ? (rb->max_size - 1U) : (head - 1U);
    return rb->buffer[index];
}

RingBufElement RingBuffer_GetTailData(s_RingBufferType * const rb)
{
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_acquire);
    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    if (head == tail)
    {
        return 0;
    }
    return rb->buffer[tail];
}

_Bool RingBuffer_IsDataAvailable(s_RingBufferType * const rb)
{
    RingBufCtr head = atomic_load_explicit(&rb->head, memory_order_acquire);
    RingBufCtr tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    return (head != tail);
}

