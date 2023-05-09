#ifndef __STM32_RING_BUFFER_H
#define __STM32_RING_BUFFER_H

#include "port.h"

#include <stdint.h>

struct stm32_ring_buffer
{
    uint8_t *data;
    int head, tail, count, size;
};

static inline void stm32_rbuf_put( struct stm32_ring_buffer* buf, int ch, int is_isr )
{
    if (buf->count >= buf->size)
		return;

    if(!is_isr)
      portENTER_CRITICAL();

    buf->data[buf->head] = ch;
	buf->head++;
    buf->count++;

	if (buf->head >= buf->size)
		buf->head = 0;

    if(!is_isr)
      portEXIT_CRITICAL();
}

static inline int stm32_rbuf_get( struct stm32_ring_buffer* buf, int is_isr )
{
    if( !buf->count )
        return -1;

    if(!is_isr) 
      portENTER_CRITICAL();

	int rv = buf->data[buf->tail];

    buf->tail++;
    if (buf->tail >= buf->size)
		buf->tail = 0;
    buf->count--;

    if(!is_isr) 
      portEXIT_CRITICAL();

    return rv;
}


static inline int stm32_rbuf_empty( struct stm32_ring_buffer* buf )
{
    return ( buf->count == 0 );
}

static inline int stm32_rbuf_init( struct stm32_ring_buffer *buf, int size )
{
    buf->head = buf->tail = buf->count = 0;
    buf->size = size;
    buf->data = pvPortMalloc( size );
    return 0;
}

static inline int stm32_rbuf_avail( struct stm32_ring_buffer* buf )
{
    return buf->size - buf->count;
}

static inline int stm32_rbuf_count( struct stm32_ring_buffer* buf )
{
    return buf->count;
}

static inline int stm32_rbuf_tail( struct stm32_ring_buffer* buf, int is_isr )
{
    int rv ;
    if( !is_isr )
        portENTER_CRITICAL();

    rv = buf->data[buf->tail];

    if( !is_isr )
        portEXIT_CRITICAL();

    return rv;
}


#endif
