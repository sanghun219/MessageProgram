#pragma once
#include "ServLibrary.h"

#define SHORT_SIZE sizeof(short)
#define INT_SIZE sizeof(int32_t)

class OutputStream
{
public:

	void Write(const char* pData, const int dataSize)
	{
		int next = m_head + dataSize;
		const char* copyData = static_cast<const char*>(pData);
		if (next > m_capacity)
		{
			ReallocBuffer(std::max<int>(m_capacity * 2, next));
		}

		memcpy(&m_buffer[m_head], &copyData[0], dataSize);
		m_head += dataSize;
	}
	void Write(const std::string inString);
	void Write(const short data, const int32_t dataSize = SHORT_SIZE) { Write((const char*)data, dataSize); }
	void Write(const int32_t data, const int32_t dataSize = INT_SIZE) { Write((const char*)data, dataSize); }
	void ReallocBuffer(int32_t BufferCapacity);
private:
	int m_head;
	int m_capacity;
	char* m_buffer;
public:
	/// Capcaity는 최대한 패킷크기보다 훨신 큰 크기로 잡아준다.
	OutputStream();
	~OutputStream();
};
class InputStream
{
private:
	void ReallocBuffer(int32_t BufferCapacity);
public:

	void Read(char* pData, const int32_t dataSize)
	{
		int next = m_head + dataSize;
		const char* copyData = reinterpret_cast<const char*>(pData);

		memcpy((void*)&copyData[0], &m_buffer[m_head], dataSize);
		m_head = next;
	}
	void Read(std::string& inString);
	void Read(short data, const int32_t dataSize = SHORT_SIZE) { Read((char*)data, dataSize); }
	void Read(int32_t data, const int32_t dataSize = INT_SIZE) { Read((char*)data, dataSize); }

	char* GetBuffer()const { return m_buffer; }
public:
	InputStream& operator = (const InputStream& stream)
	{
		if (this == &stream)
			return *this;

		m_head = stream.m_head;
		m_capacity = stream.m_capacity;
		memcpy(m_buffer, stream.m_buffer, sizeof(stream.m_buffer));
		return *this;
	}
public:
	InputStream(const int32_t Capacity, char* buffer);
	~InputStream();
private:
	int32_t m_head;
	int32_t m_capacity;
	char* m_buffer;
};
