#include "Stream.h"
#include "ServLibrary.h"
OutputStream::OutputStream() :m_head(0)
{
	m_buffer = nullptr;
	ReallocBuffer(1500 * 8);
}

OutputStream::~OutputStream()
{
	std::free(m_buffer);
}

void OutputStream::Write(const std::string inString)
{
	unsigned int elemCount = static_cast<unsigned int>(inString.size());
	StreamWrite((char*)&elemCount, sizeof(elemCount));
	for (const auto& elem : inString)
	{
		StreamWrite(&elem, sizeof(elem));
	}
}

void OutputStream::Write(const Packet & pck)
{
	short pk_id = static_cast<short>(pck.pckData.pkHeader.id);
	short pk_dir = static_cast<short>(pck.pckData.pkHeader.dir);
	int pk_size = pck.pckData.dataSize;
	char* pk_data = nullptr;
	memcpy(&pk_data[0], &pck.pckData.data[0], sizeof(pck.pckData.data));

	Write(pk_dir);
	Write(pk_id);
	Write(pk_size);
	Write(pk_data, pk_size);
}

void OutputStream::ReallocBuffer(int BufferCapacity)
{
	if (m_buffer == nullptr)
	{
		m_buffer = new char[BufferCapacity];
		memset(m_buffer, 0, BufferCapacity);
	}
	else
	{
		char* copyBuffer = static_cast<char*>(new char[BufferCapacity]);
		memcpy(copyBuffer, m_buffer, BufferCapacity);
		std::free(m_buffer);
		m_buffer = copyBuffer;
	}
	m_capacity = BufferCapacity;
}

void InputStream::ReallocBuffer(int BufferCapacity)
{
	if (m_buffer == nullptr)
	{
		m_buffer = new char[BufferCapacity];
	}
	else
	{
		char* copyBuffer = static_cast<char*>(new char[BufferCapacity]);
		memcpy(copyBuffer, m_buffer, BufferCapacity);
		std::free(m_buffer);
		m_buffer = copyBuffer;
	}
	m_capacity = BufferCapacity;
}

void InputStream::Read(std::string& inString)
{
	int dataSize = 0;
	Read((char*)&dataSize, inString.size());
	inString.resize(dataSize);
	for (auto& elem : inString)
	{
		Read(&elem, sizeof(elem));
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