#pragma once
#include "ServLibrary.h"
class OutputStream
{
public:
	template <typename T>
	void Read(T* pData, const int dataSize = sizeof(T));
	void ReallocBuffer(int BufferCapacity);
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
public:
	template <typename T>
	void Write(const T* pData, const int dataSize);
	void ReallocBuffer(int BufferCapacity);
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
	InputStream(const int Capacity, char* buffer);
	~InputStream();
private:
	int m_head;
	int m_capacity;
	char* m_buffer;
};

template<typename T>
inline void OutputStream::Read(T * pData, const int dataSize)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "해당 데이터는 원시 자료형이 아닙니다!!");

	int next = m_head + dataSize;
	unsigned char* copyData = reinterpret_cast<unsigned char*>(pData);

	memcpy(&copyData[0], &m_buffer[m_head], dataSize);
	m_head = next;
}

template<typename T>
inline void InputStream::Write(const T * pData, const int dataSize)
{
	static_assert(std::is_arithmetic<T>::value() || std::is_enum<T>::value(), "해당 데이터는 원시 자료형이 아닙니다!!");

	int next = m_head + dataSize;
	unsigned char* copyData = reinterpret_cast<unsigned char*>(pData);
	if (next > m_capacity)
	{
		ReallocBuffer(std::max<int>(m_capacity * 2, next));
	}

	memcpy(&m_buffer[m_head], &copyData[0], dataSize);
	m_head += dataSize;
}
