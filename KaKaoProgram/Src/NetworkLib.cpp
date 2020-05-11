#include "..\Inc\NetworkLib.h"
#include <zlib.h>

uint32_t NetworkLib::ComputeGlobalCRC(Stream & stream)
{
	uint32_t crc = crc32(0, Z_NULL, 0);
	crc = crc32(crc, reinterpret_cast<const Bytef*>(stream.data()), stream.size());
	return crc;
}