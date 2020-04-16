#include "KaKaoProgramPCH.h"
#include "Stream.h"

OutputStream::OutputStream() :m_head(0), m_buffer(nullptr)
{
	ReallocBuffer(1500 * 8);
}

OutputStream::~OutputStream()
{
	std::free(m_buffer);
}

void OutputStream::ReallocBuffer(int BufferCapacity)
{
	char* copyBuffer = new char[BufferCapacity];
	if (m_buffer == nullptr)
	{
		m_buffer = new char[BufferCapacity];
	}
	else
	{
		memcpy(copyBuffer, m_buffer, m_head);
		std::free(m_buffer);
		m_buffer = copyBuffer;
	}
}

void InputStream::ReallocBuffer(int BufferCapacity)
{
	char* copyBuffer = new char[BufferCapacity];
	if (m_buffer == nullptr)
	{
		m_buffer = new char[BufferCapacity];
	}
	else
	{
		memcpy(copyBuffer, m_buffer, m_head);
		std::free(m_buffer);
		m_buffer = copyBuffer;
	}
}

InputStream::InputStream(const int Capacity, char* buffer) :m_head(0), m_capacity(Capacity), m_buffer(buffer)
{
	ReallocBuffer(Capacity);
}

InputStream::~InputStream()
{
	std::free(m_buffer);
}