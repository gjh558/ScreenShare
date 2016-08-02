#ifndef FRAME_H
#define FRAME_H
#include <stdint.h>

struct Frame
{
	Frame(uint8_t *, uint32_t);
	
	uint8_t data[1024 * 1024];
	uint32_t size;
};
#endif