#include "Stream.h"
#include "ServLibrary.h"
#include <string>
OutputStream::OutputStream() :m_head(0)
{
	m_buffer = nullptr;
	ReallocBuffer(1500 * 8);
}

OutputStream::~OutputStream()
{
	std::free(m_buffer);
}
void OutputStream::Write(const void* pData, const uint32_t dataSize)
{
	const char* copyData = static_cast<const char*>(pData);
	uint32_t next = m_head + dataSize;
	if (next > m_capacity)
	{
		ReallocBuffer(std::max<uint32_t>(m_capacity * 2, next));
	}
	memcpy((void*)&m_buffer[m_head], &copyData[0], dataSize);

	m_head = next;
}
void OutputStream::Write(const std::string inString)
{
	unsigned int elemCount = static_cast<unsigned int>(inString.size());
	Write((void*)&elemCount, sizeof(elemCount));
	for (const auto& elem : inString)
	{
		Write((void*)&elem, sizeof(elem));
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

void OutputStream::ReallocBuffer(uint32_t BufferCapacity)
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

void InputStream::ReallocBuffer(uint32_t BufferCapacity)
{
	if (m_buffer == nullptr)
	{
		m_buffer = new char[BufferCapacity];
		memset(m_buffer, 0, BufferCapacity);
	}
	else
	{
		char* copyBuffer = static_cast<char*>(new char[BufferCapacity]);
		memset(copyBuffer, 0, BufferCapacity);
		memcpy(copyBuffer, m_buffer, sizeof(m_buffer));
		std::free(m_buffer);
		m_buffer = copyBuffer;
	}
	m_capacity = BufferCapacity;
}

void InputStream::Read(void * outData, const uint32_t dataSize)
{
	uint8_t* copyData = reinterpret_cast<uint8_t*>(outData);
	uint32_t next = m_head + dataSize;

	if (next >= m_capacity)
	{
		m_head = 0;
	}
	*copyData = static_cast<uint8_t>(copyData[m_head]);
	m_head = next;
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

InputStream::InputStream(const uint32_t Capacity, char* buffer) :m_head(0), m_capacity(Capacity)
{
	ReallocBuffer(Capacity);
}

InputStream::~InputStream()
{
	delete m_buffer;
	m_buffer = nullptr;
}