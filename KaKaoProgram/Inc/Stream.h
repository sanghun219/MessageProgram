#pragma once
#include "ServLibrary.h"
#define SHORT_SIZE sizeof(short)
#define INT_SIZE sizeof(int)
struct Packet;
class OutputStream
{
public:
	void Write(const void* pData, const uint32_t dataSize);
public:
	void Write(const std::string inString);
	void Write(const Packet& pck);
	inline void Write(const short& data, const uint32_t dataSize = SHORT_SIZE) { Write((void*)&data, dataSize); }
	inline void Write(const int& data, const uint32_t dataSize = INT_SIZE) { Write((void*)&data, dataSize); }
	inline void Write(const char* data, const uint32_t dataSize) { Write((void*)&data, dataSize); }
	inline char* GetBuffer()const { return m_buffer; }
	inline uint32_t GetBufferSize()const { return sizeof(m_buffer); }
private:
	uint32_t m_head;
	uint32_t m_capacity;
	char* m_buffer;
	void ReallocBuffer(uint32_t BufferCapacity);
public:
	/// Capcaity는 최대한 패킷크기보다 훨신 큰 크기로 잡아준다.
	OutputStream();
	~OutputStream();
};
class InputStream
{
private:
	void ReallocBuffer(uint32_t BufferCapacity);
public:
	void Read(void* outData, const uint32_t dataSize);

	void Read(std::string& inString);
	inline void Read(short data, const uint32_t dataSize = SHORT_SIZE) { Read((void*)data, dataSize); }
	inline void Read(int data, const uint32_t dataSize = INT_SIZE) { Read((void*)data, dataSize); }

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
	InputStream(const uint32_t Capacity, char* buffer);
	~InputStream();
private:
	uint32_t m_head;
	uint32_t m_capacity;
	char* m_buffer;
};
