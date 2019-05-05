#ifndef  __HEX_H__
#define  __HEX_H__


#include <stdint.h>

#define HEX_DATA_LEN_MAX   50

typedef struct
{
	uint32_t addr;
	uint8_t* data;
	uint8_t  len;
}HEX_DATA_t;

typedef struct
{
	uint8_t buf[2*HEX_DATA_LEN_MAX];
	uint8_t count;
	uint8_t state;
	uint8_t addr_h[4];

	uint8_t header;
	uint8_t len;
	uint8_t addr_l[4];
	uint8_t type;
	uint8_t data[2*HEX_DATA_LEN_MAX];
	uint8_t checksum;
}HEX_OBJ_t;

#define HEX_E_OK       0
#define HEX_E_WAIT     1
#define HEX_E_FINISH   2
#define HEX_E_ERROR   -1

extern HEX_OBJ_t* hex_newobject(void);
extern void hex_resetobject(HEX_OBJ_t* obj);
extern int8_t hex_findobject(HEX_OBJ_t* obj, uint8_t ch);
extern uint8_t hex_getdata(HEX_OBJ_t* obj, HEX_DATA_t* data);

#endif //__HEX_H__

