#pragma once
#include "ServLibrary.h"

class Stream
{
private:
	bool checkReadBound(UINT32 len);
	void Init();
	void set(UCHAR* data, UINT32 size);
public:
	Stream();
	Stream(UCHAR* stream, UINT32 size);

	UCHAR* data();
	UINT32 size();

	void operator = (Stream& stream);

	// Write
public:
	bool checkWriteBound(INT32 len);
	template<typename T>
	void operator <<(const T& value);
	void operator <<(const bool& value);
	void operator <<(const INT8& value);
	void operator <<(const UINT8& value);
	void operator <<(const INT16& value);
	void operator <<(const UINT16& value);
	void operator <<(const INT32& value);
	void operator <<(const UINT32& value);
	void operator <<(const INT64& value);
	void operator <<(const UINT64& value);

	template<typename T>
	void operator <<(const std::vector<T>& value);
	Stream& operator <<(const std::string& value);

	//READ
private:
	void read(void* retval, UINT32 len);
public:
	template<typename T>
	void operator >> (T* retVal);

	void operator >> (bool* retval);
	void operator >> (INT8* retval);
	void operator >> (UINT8* retval);
	void operator >> (INT16* retval);
	void operator >> (UINT16* retval);
	void operator >> (INT32* retval);
	void operator >> (UINT32* retval);
	void operator >> (INT64* retval);
	void operator >> (UINT64* retval);

	template<typename T>
	void operator >> (std::vector<T>* retval);
	Stream& operator >> (std::string* retval);

private:
	size_t m_offset;
	size_t m_readPt;
	std::array < UCHAR, PACKET_SIZE> m_stream;
};
