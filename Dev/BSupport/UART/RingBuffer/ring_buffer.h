/*
 * ringbuffer.h
 *
 *  Created on: Feb 24, 2025
 *      Author: CAO HIEU
 */

#ifndef M4_UTILS_RINGBUFFER_RING_BUFFER_H_
#define M4_UTILS_RINGBUFFER_RING_BUFFER_H_

#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

#define RINGBUFFER_NAME_MAX_LEN 32
#define TOTAL_RINGBUFFERS 4

typedef unsigned short RingBufCtr;
typedef uint8_t RingBufElement;

typedef struct {
    uint32_t id;                                 // Unique ID for the ring buffer
    char name[RINGBUFFER_NAME_MAX_LEN];          // Descriptive name of the ring buffer
    RingBufElement *buffer;                      // Pointer to the data array
    RingBufCtr max_size;                         // Maximum number of elements in the buffer
    atomic_ushort head;                          // Write index (producer)
    atomic_ushort tail;                          // Read index (consumer)
} s_RingBufferType;

//s_RingBufferType Glb_RingBuffers[TOTAL_RINGBUFFERS];

typedef void (*RingBufHandler)(RingBufElement const el);

void RingBuffer_Create(s_RingBufferType *rb, uint32_t id, const char *name,
                         RingBufElement *buffer, RingBufCtr max_size);
_Bool RingBuffer_Put(s_RingBufferType * const rb, RingBufElement const el);
_Bool RingBuffer_Get(s_RingBufferType * const rb, RingBufElement *pel);
RingBufCtr RingBuffer_NumFreeSlots(s_RingBufferType * const rb);
void RingBuffer_ProcessAll(s_RingBufferType * const rb, RingBufHandler handler);

//Expand API:
RingBufCtr RingBuffer_GetHead(s_RingBufferType * const rb);
RingBufCtr RingBuffer_GetTail(s_RingBufferType * const rb);
RingBufElement RingBuffer_GetHeadData(s_RingBufferType * const rb);
RingBufElement RingBuffer_GetTailData(s_RingBufferType * const rb);
_Bool RingBuffer_IsDataAvailable(s_RingBufferType * const rb);

#endif /* M4_UTILS_RINGBUFFER_RING_BUFFER_H_ */
