#pragma once

#include <inttypes.h>

namespace archiver {

using byte = uint8_t;

struct IInputStream {
	// Возвращает false, если поток закончился
	virtual bool Read(byte& value) = 0;
};

struct IOutputStream {
	virtual void Write(byte value) = 0;
};

}   // namespace archiver