#include "Stream.h"
#define STREAM_READ(type,retval)\
INT32 size = sizeof(type);\
if(this->checkReadBound(size)==false){\
return;\
}\
this->read((void*)(retval),size);

#define STREAM_WRITE(value)	\
INT32 size = sizeof(value);	\
if (this->checkWriteBound(size) == false){	\
	return;	\
}	\
memcpy_s((void*)(m_stream.data()+m_offset),m_stream.size()-m_offset,(const void*)&value,	\
size);m_offset += size;
Stream::Stream()
{
	this->Init();
}

Stream::Stream(UCHAR * stream, UINT32 size)
{
	this->Init();
	this->set(stream, size);
}

void Stream::Init()
{
	m_readPt = 0;
	m_offset = 0;
	ZeroMemory(&m_stream, sizeof(m_stream));
}

UCHAR* Stream::data()
{
	return m_stream.data();
}

UINT32 Stream::size()
{
	return m_offset;
}

void Stream::operator= (Stream& stream)
{
	this->set(stream.data(), stream.size());
}

void Stream::set(UCHAR* data, UINT32 size)
{
	this->m_offset = size;
	memcpy_s(this->m_stream.data(), m_stream.size(), (void*)data, size);
}

bool Stream::checkWriteBound(INT32 len)
{
	if (m_offset + len > sizeof(m_stream))
	{
		LOG("Write Socket over bufsize!");
		assert(FALSE);
		return false;
	}
	return true;
}

void Stream::operator<<(const bool & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const INT8 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const UINT8 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const INT16 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const UINT16 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const INT32 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const UINT32 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const INT64 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const UINT64 & value)
{
	STREAM_WRITE(value);
}

void Stream::operator<<(const std::string & value)
{
	*this << (INT32)value.length();
	for (auto i : value) {
		*this << (UINT8)i;
	}
}

bool Stream::checkReadBound(UINT32 len)
{
	if (m_readPt + len > m_offset) {
		LOG(" readOffset : %d, size : %d, totalOffset = %d", m_readPt, len, m_offset);
		LOG("socket stream has not more memory!");
		assert(false);
		return false;
	}
	return true;
}

void Stream::read(void * retval, UINT32 len)
{
	memcpy_s(retval, len, (void*)(m_stream.data() + m_readPt), len);
	m_readPt += len;
}

void Stream::operator>>(bool * retval)
{
	STREAM_READ(bool, retval);
}

void Stream::operator>>(INT8 * retval)
{
	STREAM_READ(INT8, retval);
}

void Stream::operator>>(UINT8 * retval)
{
	STREAM_READ(UINT8, retval);
}

void Stream::operator>>(INT16 * retval)
{
	STREAM_READ(INT16, retval);
}

void Stream::operator>>(UINT16 * retval)
{
	STREAM_READ(UINT16, retval);
}

void Stream::operator>>(INT32 * retval)
{
	STREAM_READ(INT32, retval);
}

void Stream::operator>>(UINT32 * retval)
{
	STREAM_READ(UINT32, retval);
}

void Stream::operator>>(INT64 * retval)
{
	STREAM_READ(INT64, retval);
}

void Stream::operator>>(UINT64 * retval)
{
	STREAM_READ(UINT64, retval);
}

void Stream::operator>>(std::string * retval)
{
	INT32 size;
	*this >> &size;
	if (this->checkReadBound(size) == false)
		return;

	char* buf = new char[size + 1];
	this->read((void*)(buf), size * sizeof(CHAR));
	buf[size] = '\0';
	retval->clear();
	*retval = buf;

	delete buf;
}

template<typename T>
void Stream::operator>>(T * retVal)
{
	STREAM_READ(T, retVal);
}
template<typename T>
void Stream::operator>>(std::vector<T>* retval)
{
	size_t size;
	*this >> &size;
	for (size_t i = 0; i < size; ++i)
	{
		T tmp;
		*this >> &tmp;
		retval->push_back(tmp);
	}
}

template <typename T>
void Stream::operator << (const T& value)
{
	STREAM_WRITE(value);
}
template<typename T>
void Stream::operator<<(const std::vector<T>& value)
{
	*this << value.size();
	for (auto i : value) {
		*this << i;
	}
}