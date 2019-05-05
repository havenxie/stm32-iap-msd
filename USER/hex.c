#include "hex.h"
#include <stdlib.h>
#include <string.h>

#define _ASCII_2_BYTE(ch1, ch2) ((lookuptable[(ch1)-'0']<<4) | (lookuptable[(ch2)-'0']))

static const uint8_t lookuptable[] = 
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ':', ';', '<', '=', '>', '?', '@', 10, 11, 12, 13, 14, 15,
};

static HEX_OBJ_t HexObj;

static uint8_t hex_ascii2byte(uint8_t* ascii, uint8_t* bytes, uint8_t len)
{
	uint8_t i = 0;

	if(len%2)
	{
		return 0;
	}

	for(i = 0; i < len/2; i++)
	{
		bytes[i] = _ASCII_2_BYTE(ascii[2*i], ascii[2*i+1]);
	}

	return len/2;
}

HEX_OBJ_t* hex_newobject(void)
{
    return &HexObj;
}

void hex_resetobject(HEX_OBJ_t* obj)
{
	memset(obj, 0, sizeof(HEX_OBJ_t));
}

int8_t hex_findobject(HEX_OBJ_t* obj, uint8_t ch)
{
	int8_t result = HEX_E_WAIT;

	if(NULL == obj)
	{
		return HEX_E_ERROR;
	}

	switch(obj->state)
	{
		case 0: // search header
			{
				if(':' == ch)
				{
					//hex_resetobject(obj);
					obj->state  = 1;
					obj->header = ch;
					obj->count  = 2;
				}
			}
			break;
		case 1: // len
			{
				obj->buf[2 - obj->count] = ch;
				if(--obj->count == 0)
				{
					obj->len = _ASCII_2_BYTE(obj->buf[0], obj->buf[1]);

					if(0 == obj->len) // File Ends
					{
						obj->buf[0] = '0';
						obj->buf[1] = '0';
						obj->buf[2] = '0';
						obj->buf[3] = '0';
						obj->buf[4] = '0';
						obj->buf[5] = '1';
						obj->buf[6] = 'F';
						obj->buf[7] = 'F';
						obj->state  = 8;
						obj->count  = 8;
					}
					else if(obj->len <= HEX_DATA_LEN_MAX)
					{
						obj->count = 4;
						obj->state = 2;
					}
					else  // Error
					{
						hex_resetobject(obj);
						result = HEX_E_ERROR;
					}
				}
			}
			break;
		case 2: // address
			{
				obj->buf[4 - obj->count] = ch;
				if(--obj->count == 0)
				{
					obj->addr_l[0] = _ASCII_2_BYTE(obj->buf[0], obj->buf[1]);
					obj->addr_l[1] = _ASCII_2_BYTE(obj->buf[2], obj->buf[3]);
					obj->state     = 3;
					obj->count     = 2;
				}
			}
			break;
		case 3: // type
			{
				obj->buf[2 - obj->count] = ch;
				if(--obj->count == 0)
				{
					// Hex Line Type
					obj->type = _ASCII_2_BYTE(obj->buf[0], obj->buf[1]);

					switch(obj->type)
					{
						case 0x00: // data
						case 0x04: // Section
                        case 0x05: // Donothing                            
							{
								obj->state = 4;
								obj->count = obj->len*2;
							}
							break;
                            
						default:
							{
								hex_resetobject(obj);
								result = HEX_E_ERROR;
							}
							break;
					}
				}
			}
			break;
		case 4: // data
			{
				obj->buf[obj->len*2 - obj->count] = ch;
				if(--obj->count == 0)
				{
					hex_ascii2byte(obj->buf, obj->data, obj->len*2);

					if(0x04 == obj->type)
					{
						memcpy(obj->addr_h, obj->data, obj->len);
					}
                    else if(0x05 == obj->type)
                    {
                        // Donothing
                    }

					obj->state = 5;
					obj->count = 2;
				}
			}
			break;
		case 5: // checksum
			{
				obj->buf[2 - obj->count] = ch;
				if(--obj->count == 0)
				{
					uint8_t sum = 0;
					obj->checksum = _ASCII_2_BYTE(obj->buf[0], obj->buf[1]);

					sum += obj->len;
					sum += obj->addr_l[0];
					sum += obj->addr_l[1];
					sum += obj->type;

					{
						uint8_t i = 0;
						for(i = 0; i < obj->len; i++)
						{
							sum += obj->data[i];
						}
					}

					sum += obj->checksum;

					if(0 == sum) // Passed
					{
                        obj->state = 0;
						result = HEX_E_OK;
					}
					else         // Failured
					{
						hex_resetobject(obj);
						result = HEX_E_ERROR;
					}
				}
			}
			break;

		case 8: // File End Sqeue
			{
				uint8_t i = 8 - obj->count;
				if(obj->buf[i] == ch)
				{
					if(--obj->count == 0)
					{
						hex_resetobject(obj);
						result = HEX_E_FINISH;
					}
				}
				else
				{
					hex_resetobject(obj);
					result = HEX_E_ERROR;
				}
			}
			break;
	}

	return result;
}

uint8_t hex_getdata(HEX_OBJ_t* obj, HEX_DATA_t* data)
{
    uint8_t* p = (uint8_t*) &(data->addr);
    
    if(obj->type)
    {
        return 0;
    }
    
    p[0] = obj->addr_l[1];
    p[1] = obj->addr_l[0];
    p[2] = obj->addr_h[1];
    p[3] = obj->addr_h[0];
    
    data->data = obj->data;
    data->len  = obj->len;
    
    return data->len;
}

