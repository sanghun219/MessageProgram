#pragma once
#include "ServLibrary.h"
#define SHORT_SIZE sizeof(short)
#define INT_SIZE sizeof(int)
struct Packet;
class OutputStream
{
public:

	inline void StreamWrite(const void* pData, const int dataSize)
	{
		const char* copyData = static_cast<const char*>(pData);
		int next = m_head + dataSize;
		if (next > m_capacity)
		{
			ReallocBuffer(std::max<int>(m_capacity * 2, next));
		}

		memmove(&m_buffer[m_head], copyData, dataSize);
		m_head = next;
	}
	inline void Write(const std::string inString);
	void Write(const Packet& pck);
	inline void Write(const short& data, const int dataSize = SHORT_SIZE) { StreamWrite(&data, dataSize); }
	inline void Write(const int& data, const int dataSize = INT_SIZE) { StreamWrite(&data, dataSize); }
	inline void Write(const char* data, const int dataSize) { StreamWrite(data, dataSize); }
	inline char* GetBuffer()const { return m_buffer; }
	inline int GetBufferSize()const { return sizeof(m_buffer); }
private:
	int m_head;
	int m_capacity;
	char* m_buffer;
	void ReallocBuffer(int BufferCapacity);
public:
	/// Capcaity는 최대한 패킷크기보다 훨신 큰 크기로 잡아준다.
	OutputStream();
	~OutputStream();
};
class InputStream
{
private:
	void ReallocBuffer(int BufferCapacity);
public:

	inline void Read(char* pData, const int dataSize)
	{
		int next = m_head + dataSize;
		const char* copyData = reinterpret_cast<const char*>(pData);

		memcpy((void*)&copyData[0], &m_buffer[m_head], dataSize);
		m_head = next;
	}
	void Read(std::string& inString);
	inline void Read(short data, const int dataSize = SHORT_SIZE) { Read((char*)data, dataSize); }
	inline void Read(int data, const int dataSize = INT_SIZE) { Read((char*)data, dataSize); }

	inline char* GetBuffer()const { return m_buffer; }
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
	InputStream(const int Capacity, char* buffer);
	~InputStream();
private:
	int m_head;
	int m_capacity;
	char* m_buffer;
};
