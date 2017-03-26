#define LOG_TAG "fifo"
#include "log.h"
#include "fifo.h"
#include "string.h"
#include "stdlib.h"
#include "common.h"

#define FIFO_DEBUG 0
/*

	   table|
		----|-----data_len=5-----|
		 |	|---|---|---|---|----|
tail=1	 |	|---|---|---|---| 
		 |  |---|
  		len |---|
head=4	 7  |---|
		 |  |---|
		 |  |---|
		----|
size=3
*/

static int fifo_in(fifo_t *fifo, FIFO_DATA_TYPE *data, FIFO_DATA_TYPE len);
static int fifo_out(fifo_t *fifo, FIFO_DATA_TYPE *data, FIFO_DATA_TYPE *len);
static int fifo_get_out(fifo_t *fifo, void *add, FIFO_DATA_TYPE *len);
u8 is_empty(fifo_t *fifo);
static u8 _is_full(fifo_t *fifo);

int fifo_init(fifo_t *fifo, FIFO_LEN_TYPE fifo_len, FIFO_DATA_TYPE data_len)
{
    int err = 0;
    if (!fifo) {
        LOGE("fifo is null");
        return -EINVAL;
    }
    
    if (fifo_len < 1 || fifo_len > 255) {
        LOGE("fifo_len should be 1 - 255");
        return -EINVAL;
    }

    if (data_len < 1 || data_len > 255) {
        LOGE("data_len should be 1 - 255");
        return -EINVAL;
    }
    
    fifo->head = 0;
    fifo->tail = 0;
    fifo->size  = 0;
    fifo->len  = fifo_len;
	
    fifo->in   = fifo_in;
    fifo->out  = fifo_out;
    fifo->get_out = fifo_get_out;
	fifo->is_empty = is_empty;
	
	fifo->data.len = data_len;
	//TODO, whether clear the allocated buffer
    fifo->data.buffer = (FIFO_DATA_TYPE *)malloc(fifo_len * data_len);
    if (!fifo->data.buffer) {
        LOGE("Out of memory.");
        return -ENOMEM;
    }
	
    fifo->table  = (FIFO_LEN_TYPE *)malloc(fifo_len);
    if (!fifo->table) {
        free(fifo->data.buffer);
        LOGD("Out of memory.");
        return -ENOMEM;
    }

    LOGD("Malloc OK, fifo len: %d, data_len: %d", fifo_len, data_len);
    return err;
}

int fifo_in(fifo_t *fifo, FIFO_DATA_TYPE *data, FIFO_DATA_TYPE len)
{
    int err = 0;
#if FIFO_DEBUG
	static u32 i = 0;
	i++;
#endif
	
	if (!fifo) {
        LOGE("fifo is null");
        return -EINVAL;
	}

    if (!data) {
        LOGE("data is null");
        return -EINVAL;
	}

    if (len < 1 || len > 255) {
        LOGE("data_len should be 1 - 255, but is: %d", len);
        return -EINVAL;
    }

    if (_is_full(fifo)) {
        LOGE("fifo is full");
        return -EAGAIN;
    }
	
    memcpy((fifo->data.buffer + (fifo->head * fifo->data.len)), data, len);
    fifo->table[fifo->head++] = len;

    if (fifo->head == fifo->len) {
        fifo->head = 0;
    }
    fifo->size++;
	
#if FIFO_DEBUG
    LOGD("size:%d data_len:%d in_cnt:%d",fifo->size,len,i);
    prta(data, len);
#endif
	return err;
}

int fifo_out(fifo_t *fifo, FIFO_DATA_TYPE *data, FIFO_DATA_TYPE *len)
{
    int err = 0;
#if FIFO_DEBUG
    static u32 i = 0;
    i++;
#endif
    
    if (!fifo) {
        LOGE("fifo is null");
        return -EINVAL;
    }

    if (!data) {
        LOGE("data is null");
        return -EINVAL;
    }

    if (!len) {
        LOGE("len is null");
        return -EINVAL;
    }

    if (is_empty(fifo)) {
        LOGE("fifo is empty");
        return -EAGAIN;
    }

    *len = fifo->table[fifo->tail];
    memcpy(data, (fifo->data.buffer + fifo->tail * fifo->data.len), *len);
    fifo->tail++;
    if (fifo->tail == fifo->len) {
        fifo->tail = 0;
    }
    fifo->size--;
#if FIFO_DEBUG
        LOGD("out: size:%d data_len:%d out_cnt:%d",fifo->size,*len,i);
        prta(data, *len);
#endif
    return err;
}


int fifo_get_out(fifo_t *fifo, void *add, FIFO_DATA_TYPE *len)
{
    int err = 0;
#if FIFO_DEBUG
    static u32 i = 0;
    i++;
#endif
    
    if (!fifo) {
        LOGE("fifo is null");
        return -EINVAL;
    }

    if (!add) {
        LOGE("add is null");
        return -EINVAL;
    }

    if (!len) {
        LOGE("len is null");
        return -EINVAL;
    }

    if (is_empty(fifo)) {
        LOGE("fifo is empty");
        return -EAGAIN;
    }

	*(u32 *)add = (u32)(fifo->data.buffer + fifo->tail * fifo->data.len);
	*len = fifo->table[fifo->tail++];

    if (fifo->tail == fifo->len) {
        fifo->tail = 0;
    }
    fifo->size--;
#if FIFO_DEBUG
    LOGD("out: size:%d data_len:%d out_cnt:%d",fifo->size,*len,i);
    prta((u8 *)(*(u32 *)add), *len);
#endif
    return err;
}

u8 is_empty(fifo_t *fifo)
{
    if (!fifo) {
        LOGE("fifo is null");
    }
    return ((fifo->size == 0) ? 1 : 0);
}

static u8 _is_full(fifo_t *fifo)
{
    if (!fifo) {
        LOGE("fifo is null");
    }
    return ((fifo->size == fifo->len) ? 1 : 0);
}


