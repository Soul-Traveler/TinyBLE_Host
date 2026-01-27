#include "fifo.h"
#include <string.h>


/**
* @brief  判断一个数是否为2的整数次幂
*
* @param  n 要判断的数
*
* @retval TRUE
* @retval FALSE
*/
 uint8_t is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

/**
* @brief  fifo初始化
*
* @param  fifo 要初始化的fifo
* @param  data fifo的缓冲区
* @param  size  fifo缓冲大小,必须为2的幂
*
* @retval TRUE 初始化成功
* @retval FALSE 初始化失败
*/
uint8_t kfifo_init(KFIFO *fifo, uint8_t *data, uint32_t size)
{
	if (!is_power_of_2(size))
	{
		return FALSE;
	}

	fifo->buffer = data;
	fifo->size = size;
	fifo->in = fifo->out = 0;
	return TRUE;
}



/**
* @brief  向FIFO写入数据
*
* @param  fifo 待写入的FIFO
* @param  data 待写入的数据指针
* @param  len 待写入的数据长度
*
* @return 实际写入的数据
*/
uint32_t kfifo_put(KFIFO *fifo, uint8_t *data, uint32_t len)
{
	uint32_t L;

	len = MIN(len, fifo->size - (fifo->in - fifo->out));
	L = MIN(len, fifo->size - (fifo->in & (fifo->size - 1)));

	/* first put the data starting from fifo->in to buffer end */
	memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), data, L);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, data + L, len - L);

	fifo->in += len;

	return len;
}


/**
* @brief  从FIFO取出数据
*
* @param  fifo 要取出数据的FIFO
* @param  data 存储取出数据的缓存
* @param  len 要取出数据的长度
*
* @return 实际取出的数据长度
*/
uint32_t kfifo_get(KFIFO *fifo, uint8_t *data, uint32_t len)
{
	uint32_t L;

	len = MIN(len, fifo->in - fifo->out);
	L = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));

	/* first get the data from fifo->out until the end of the buffer */
	memcpy(data, fifo->buffer + (fifo->out & (fifo->size - 1)), L);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(data + L, fifo->buffer, len - L);

	fifo->out += len;

	return len;
}



/**
* @brief  从FIFO取出数据
*
* @param  fifo 要取出数据的FIFO
* @param  buffer 存储取出数据的缓存
*
* @return 实际取出的数据长度
*/
uint8_t kfifo_get_ch(KFIFO *fifo, uint8_t *data)
{
	if(!(fifo->in == fifo->out)) //非空
	{
		*data = *(fifo->buffer + (fifo->out & (fifo->size - 1)));
		fifo->out += 1;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
* @brief  向FIFO写入1个字节数据
*
* @param  fifo 待写入的FIFO
* @param  buffer 待写入的数据
*
* @return 实际写入的数据个数
*/
uint8_t kfifo_put_ch(KFIFO *fifo, uint8_t data)
{
	if(!((fifo->in - fifo->out) > (fifo->size - 1))) //非满
	{
		*(fifo->buffer + (fifo->in & (fifo->size - 1))) = data;
		fifo->in += 1;
		return 1;
	}
	else
	{
		return 0;
	}
}


//kfifo_reset fifo->in = fifo->out = 0;
void kfifo_reset(KFIFO *fifo)
{
	fifo->in = fifo->out = 0;
}

// kfifo_len - returns the number of used elements in the fifo
uint32_t kfifo_len(KFIFO *fifo)
{
	return (fifo->in - fifo->out);
}

//kfifo_is_empty - returns true if the fifo is empty
uint8_t kfifo_is_empty(KFIFO *fifo)
{
	return (fifo->in == fifo->out);
}

//kfifo_is_full - returns true if the fifo is full
uint8_t kfifo_is_full(KFIFO *fifo)
{
	return ((fifo->in - fifo->out) > (fifo->size - 1));
}

//kfifo_avail - returns the number of unused elements in the fifo
uint32_t kfifo_avail(KFIFO *fifo)
{
	return (fifo->size - (fifo->in - fifo->out));
}
