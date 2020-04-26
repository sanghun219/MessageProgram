#include "Stream.h"

OutputStream::OutputStream() :m_head(0), m_buffer(nullptr)
{
	ReallocBuffer(1500 * 8);
}

OutputStream::~OutputStream()
{
	std::free(m_buffer);
}

void OutputStream::Write(const std::string inString)
{
	unsigned int elemCount = static_cast<unsigned int>(inString.size());
	Write((char*)&elemCount, sizeof(elemCount));
	for (const auto& elem : inString)
	{
		Write(&elem, sizeof(elem));
	}
}

void OutputStream::ReallocBuffer(int64_t BufferCapacity)
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

void InputStream::ReallocBuffer(int64_t BufferCapacity)
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

void InputStream::Read(std::string& inString)
{
	int64_t dataSize = 0;
	Read((char*)&dataSize, inString.size());
	inString.resize(dataSize);
	for (auto& elem : inString)
	{
		Read(&elem, sizeof(elem));
	}
}

InputStream::InputStream(const int64_t Capacity, char* buffer) :m_head(0), m_capacity(Capacity), m_buffer(buffer)
{
	ReallocBuffer(Capacity);
}

InputStream::~InputStream()
{
	std::free(m_buffer);
}