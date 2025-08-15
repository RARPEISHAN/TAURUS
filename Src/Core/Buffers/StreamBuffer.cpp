//--------------------------------------------------------------------
//	StreamBuffer.cpp.
//	07/12/2020.				by bubo.
//	07/14/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "StreamBuffer.h"

//--------------------------------------------------------------------
aldebaran::core::StreamBuffer::StreamBuffer(EventHandle handler,
	size_t buffSize,
	size_t maxBuffSize) :_handle(handler),
	_writeBuffer(handler, buffSize, maxBuffSize),
	_readBuffer(handler, buffSize, maxBuffSize)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::StreamBuffer::~StreamBuffer()
{
	net::close(_handle);
	_handle = INVALID_SOCKET;
	//
}

//--------------------------------------------------------------------
unsigned int
aldebaran::core::StreamBuffer::read(
	void* _DstBuf,
	unsigned int _MaxCharCount,
	int flags)
{
	int loopCount = 0;
	while (_readLock.test_and_set())
	{
		++loopCount;
		if (loopCount > 10000000)
			throw std::exception("deadlock");
		std::this_thread::yield();
	}
	if (_DstBuf)
	{
		int i = _readBuffer.read(reinterpret_cast<char*>(_DstBuf), _MaxCharCount);

		_readLock.clear();
		return i;
	}

	_recvFlags = flags;

	int recvied = recv();

	_readLock.clear();
	return recvied;
}

//--------------------------------------------------------------------

unsigned int
aldebaran::core::StreamBuffer::write(
	void const* _Buf,
	unsigned int _MaxCharCount,
	int flags)
{
	int loopCount = 0;
	while (_writeLock.test_and_set())
	{
		++loopCount;
		if (loopCount > 10000000)
			throw std::exception("deadlock");
		std::this_thread::yield();
	}
	if (_Buf)
	{
		int writed = _writeBuffer.write(_Buf, _MaxCharCount);
		_writeLock.clear();
		return writed;
	}

	_sendFlags = flags;
	int _sended = 0;
	if (_writeBuffer.empty());
	else
		_sended = send();
	_writeLock.clear();
	return _sended;
}
//--------------------------------------------------------------------
void
aldebaran::core::StreamBuffer::setWriteBufMaxSize(
	size_t maxSize)
{
	_writeBuffer.setMaxSize(maxSize);
}
//--------------------------------------------------------------------
void
aldebaran::core::StreamBuffer::setReadBufMaxSize(
	size_t maxSize)
{
	_readBuffer.setMaxSize(maxSize);
}
inline
aldebaran::EventHandle
aldebaran::core::StreamBuffer::getHandle()
{
	return _handle;
}
bool aldebaran::core::StreamBuffer::isEmpty(int iotype)
{
	switch (iotype)
	{
	case 1:
		return _readBuffer.empty();
	case 2:
		return _writeBuffer.empty();
	default:
		break;
	}
	return true;
}
bool aldebaran::core::StreamBuffer::isFull(int iotype)
{
	switch (iotype)
	{
	case 1:
		return _readBuffer.full();
	case 2:
		return _writeBuffer.full();
	default:
		break;
	}
	return false;
}
//--------------------------------------------------------------------
//void
//aldebaran::core::StreamBuffer::setReadFinishedCallBack(
//	const ReadFinishCallBack& cb)
//{
//	_readFinishCallBack = cb;
//}

//--------------------------------------------------------------------
//void
//aldebaran::core::StreamBuffer::setWriteFinishedCallBack(
//	const WriteFinishCallBack& cb)
//{
//	_writeFinishCallBack = cb;
//}
//--------------------------------------------------------------------

//bool aldebaran::core::StreamBuffer::trySend()
//{
//	if (_writeMx.try_lock())
//	{
//		_writeMx.unlock();
//		return true;
//	}
//	return false;
//}
//
////--------------------------------------------------------------------
//
//bool aldebaran::core::StreamBuffer::tryRecv()
//{
//	if (_readMx.try_lock())
//	{
//		_readMx.unlock();
//		return true;
//	}
//	return false;
//}

//--------------------------------------------------------------------
int
aldebaran::core::StreamBuffer::recv()
{
	int availableCapacity = _readBuffer.maxCapacity() - _readBuffer.size();

	if (availableCapacity <= 0)
	{
		return -1;
	}

	std::vector<char> tBuf(availableCapacity);
	if (_handle != INVALID_SOCKET)
	{
		auto [recvied, ioType] = net::read(_handle, tBuf.data(), availableCapacity);
		if (recvied > 0)
			return _readBuffer.write(tBuf.data(), recvied);
		else if (recvied == 0)
			_handle = INVALID_SOCKET;
	}
	return -2;//socket 无效
}

//--------------------------------------------------------------------

int
aldebaran::core::StreamBuffer::send()
{
	std::tuple<long long, aldebaran::IOTYPE> _send{ -1,aldebaran::IOTYPE::InProgress };

	if (_writeBuffer.empty())
	{
		return -1;//缓冲区为空
	}

	_send = _writeBuffer.send();

	return std::get<0>(_send);
}

//--------------------------------------------------------------------
inline
void
aldebaran::core::buffer::setMaxSize(size_t maxSize)
{
	_maxBufSize = maxSize;
}

//--------------------------------------------------------------------
aldebaran::core::buffer::buffer(
	EventHandle sock_handle,
	size_t bufferSize,
	size_t maxBufferSize) :
	_sock_handle(sock_handle),
	_buf(bufferSize + 1),
	_maxBufSize(maxBufferSize + 1),
	_bufSize(bufferSize + 1)
{
}

//--------------------------------------------------------------------
int
aldebaran::core::buffer::write(
	const void* pBuffer,
	size_t charCount)
{
	if (full())
		if (!reserve(charCount + 1))
			return 0;

	int availableCapacity = _bufSize - _size - 1;

	if (availableCapacity < charCount)
		if (!reserve(charCount + _size + 1 > _maxBufSize ? _maxBufSize : charCount + _size + 1))
			return 0;

	availableCapacity = _bufSize - _size - 1;

	int endSize = availableCapacity > charCount ? charCount : availableCapacity;

	char* pCharBuffer = static_cast<char*>(const_cast<void*>(pBuffer));

	for (int i = 0; i < endSize; ++i)
	{
		_buf[_rear] = pCharBuffer[i];
		_rear = (_rear + 1) % _bufSize;
	}

	_size += endSize;

	return endSize;
}

//--------------------------------------------------------------------
int
aldebaran::core::buffer::read(
	char* pDstBuf,
	size_t maxCharCount)
{
	if (empty())
		return 0;

	int endSize = _size > maxCharCount ? maxCharCount : _size;

	if (_rear > _front)
		memcpy(pDstBuf, _buf.data() + _front, endSize);
	else
	{
		if (_bufSize - _front > endSize)
			memcpy(pDstBuf, _buf.data() + _front, endSize);
		else
		{
			if (!reserve(_bufSize))
				return -1;
			memcpy(pDstBuf, _buf.data() + _front, endSize);
		}
	}

	_size -= endSize;

	_front = (_front + endSize) % _bufSize;

	return endSize;
}

//--------------------------------------------------------------------
inline
int
aldebaran::core::buffer::size()
{
	return _size;
}

//--------------------------------------------------------------------
inline bool aldebaran::core::buffer::empty()
{
	if (_front == _rear)
		return true;
	return false;
}

//--------------------------------------------------------------------
int aldebaran::core::buffer::capacity()
{
	return _buf.size() - 1;
}

//--------------------------------------------------------------------

int aldebaran::core::buffer::maxCapacity()
{
	return _maxBufSize - 1;
}

//--------------------------------------------------------------------
bool aldebaran::core::buffer::reserve(size_t newCapacity)
{
	if (newCapacity > _maxBufSize)
		return false;

	if (newCapacity <= _size)
		return false;

	std::vector<char> temp_buffer(newCapacity);

	if (_rear > _front)
		memcpy(temp_buffer.data(), _buf.data() + _front, _size);
	else
	{
		memcpy(temp_buffer.data(), _buf.data() + _front, _bufSize - _front);

		memcpy(temp_buffer.data() + _bufSize - _front, _buf.data(), _rear);
	}

	_buf.swap(temp_buffer);

	_front = 0;

	_rear = _size;

	_bufSize = newCapacity;

	return true;
}

//--------------------------------------------------------------------
inline bool aldebaran::core::buffer::full()
{
	if (_maxBufSize <= 0)
		return true;

	if ((_rear + 1) % _bufSize == _front)
		return true;

	return false;
}

std::tuple<long long, aldebaran::IOTYPE>
aldebaran::core::buffer::send()
{
	if (empty())
		return { 0,IOTYPE::OK };

	std::tuple<long long, aldebaran::IOTYPE> _sended = { -1 , IOTYPE::InProgress };

	if (_rear > _front)
	{
		_sended = net::send(_sock_handle, _buf.data() + _front, _size);
	}
	else
	{
		reserve(_bufSize);
		_sended = net::send(_sock_handle, _buf.data() + _front, _size);
	}
	if (std::get<0>(_sended) > 0)
	{
		_front = (_front + std::get<0>(_sended)) % _bufSize;
		_size -= std::get<0>(_sended);
	}
	return _sended;
}