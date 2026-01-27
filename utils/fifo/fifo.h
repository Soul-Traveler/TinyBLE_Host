
#ifndef __KFIFO_H
#define __KFIFO_H

#include "stdint.h"


typedef struct KFIFO
{
	uint8_t *buffer;	/* the buffer holding the data */
	uint32_t size;	/* the size of the allocated buffer */
	uint32_t in;	/* data is added at offset (in % size) */
	uint32_t out;	/* data is extracted from off. (out % size) */
	//spinlock_t *lock;	/* protects concurrent modifications */
} KFIFO,* pKFIFO;
// typedef KFIFO * pKFIFO;


#ifndef     FALSE
#define     FALSE   (0)
#endif

#ifndef     TRUE
#define     TRUE    (!FALSE)
#endif

#define     MIN(a, b)   ((a) < (b) ? (a) : (b))
#define     MAX(a, b)   ((a) < (b) ? (b) : (a))

uint8_t is_power_of_2(unsigned long n);

uint8_t kfifo_init(KFIFO *fifo, uint8_t *data, uint32_t size);
uint32_t kfifo_put(KFIFO *fifo, uint8_t *data, uint32_t len);
uint32_t kfifo_get(KFIFO *fifo, uint8_t *data, uint32_t len);
uint8_t kfifo_get_ch(KFIFO *fifo, uint8_t *data);
uint8_t kfifo_put_ch(KFIFO *fifo, uint8_t data);

uint8_t kfifo_is_empty(KFIFO *fifo);
uint8_t kfifo_is_full(KFIFO *fifo);
uint32_t kfifo_len(KFIFO *fifo);
uint32_t kfifo_avail(KFIFO *fifo);
void kfifo_reset(KFIFO *fifo);

#endif
