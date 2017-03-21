#ifndef _FIFO_H
#define _FIFO_H
#include "type.h"
#include "error.h"

/*Default fifo support 256 len
  TODO: debug whether u16 is support
*/
//fifo len parameter may be modified in interrupt
#define FIFO_LEN_TYPE  vu8
#define FIFO_DATA_TYPE u8

typedef enum {
    FIFO_ERR_NO = 0,
    FIFO_ERR_NOMEM,
    FIFO_ERR_INVAL,
	FIFO_ERR_FAULT,
    FIFO_ERR_FULL,
    FIFO_ERR_EMPTY,
	FIFO_ERR_DATA_NULL,
}fifo_error_code_t;

typedef struct _fifo_data{
    FIFO_DATA_TYPE *buffer;
	FIFO_DATA_TYPE len;
}fifo_data_t;

typedef struct _fifo{
    FIFO_LEN_TYPE head;
    FIFO_LEN_TYPE tail;
    FIFO_LEN_TYPE size;
    FIFO_LEN_TYPE len;

	FIFO_LEN_TYPE *table;
	fifo_data_t data;

    int (*in)(struct _fifo *fifo, \
        FIFO_DATA_TYPE *data, FIFO_DATA_TYPE len);
    int (*out)(struct _fifo *fifo, \
        FIFO_DATA_TYPE *data, FIFO_DATA_TYPE *len);
	int (*get_out)(struct _fifo *fifo, \
        void *add, FIFO_DATA_TYPE *len);
	u8 (*is_empty)(struct _fifo *fifo);
}fifo_t;

int fifo_init(fifo_t *fifo, FIFO_LEN_TYPE fifo_len, FIFO_DATA_TYPE data_len);

#endif
