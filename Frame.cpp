#include "Frame.h"
#include <stdlib.h>
#include <string.h>

Frame::Frame(uint8_t *buffer, uint32_t len)
{
	size = len;
	memcpy(data, buffer, len);
}