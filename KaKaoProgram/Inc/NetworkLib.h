#pragma once
#include "ServLibrary.h"
class NetworkLib
{
public:
	static uint32_t ComputeGlobalCRC(Stream& stream);
};