#ifndef ROOM_H
#define ROOM_H

#include <stdint.h>

	struct Room {
		Room(uint32_t id, uint32_t rid);
		uint32_t teacherId;
		uint32_t roomId;
	};

#endif