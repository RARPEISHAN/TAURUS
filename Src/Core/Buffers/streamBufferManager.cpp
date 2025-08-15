#include "streamBufferManager.h"

std::weak_ptr<threadPool2::thread_set> aldebaran::core::bufferManager::_pthread;
aldebaran::core::bufferManager::bufferManager()
{
}

void aldebaran::core::bufferManager::setthreadContext(std::weak_ptr<threadPool2::thread_set> pthreads)
{
	_pthread = pthreads;
}

aldebaran::core::fd_t
aldebaran::core::bufferManager::createBuffer(EventHandle soc, size_t initialSize, size_t maxBufSize)
{
	auto pbuf = new StreamBuffer(soc, initialSize, maxBufSize);
	fd_t fd = reinterpret_cast<unsigned long long>(pbuf);
	_bufContain.emplace(std::pair<fd_t, std::shared_ptr<IBuffer>>{ fd, pbuf });
	return fd;
}

bool aldebaran::core::bufferManager::recv(fd_t fd, int* NumOfrecvied, int* errorN)
{
	auto pbuf = _bufContain.find(fd);

	if (pbuf == _bufContain.end())
	{
		*NumOfrecvied = -1;
		*errorN = -1;//无效fd
		return false;
	}

	if (pbuf->second._readflag.test())
	{
		*NumOfrecvied = -1;
		*errorN = 1;//缓冲区正在被使用
		return false;
	}

	if (!_pthread.expired())
	{
		async_recv(fd);
		*NumOfrecvied = -1;
		*errorN = 2;//正在接收.
		return false;
	}

	int recved = sync_recv(fd);
	*NumOfrecvied = recved;
	*errorN = 0;//成功接收

	return true;
}

bool aldebaran::core::bufferManager::send(fd_t fd, int* NumOfSended, int* errorN)
{
	auto pbuf = _bufContain.find(fd);

	if (pbuf == _bufContain.end())
	{
		*NumOfSended = -1;
		*errorN = -1;//无效fd
		return false;
	}

	//if (pbuf->second._writeflag.test())
	//{
	//	*NumOfSended = -1;
	//	*errorN = 1;//缓冲区正在被使用
	//	return false;
	//}

	if (!_pthread.expired())
	{
		async_send(fd);
		*NumOfSended = -1;
		*errorN = 2;//正在发送； .
		return false;
	}

	int sended = sync_send(fd);
	*NumOfSended = sended;
	*errorN = 0;//成功接收

	return true;
}

int aldebaran::core::bufferManager::read(fd_t fd, void* _DstBuf, unsigned int _MaxCharCount)
{
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return -1;

	if (!pbuf->second._readflag.test())
		return -1;

	int recived = pbuf->second._pBuf->read(_DstBuf, _MaxCharCount, 0);

	if (pbuf->second._pBuf->isEmpty(1))
		clear_status(pbuf->second._readflag);
	return recived;//recived ;
}

int aldebaran::core::bufferManager::write(fd_t fd, void* _SrcBuf, unsigned int _MaxCharCount)
{
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return -1;

	if (!pbuf->second._writeflag.test())
		return -1;

	int send = pbuf->second._pBuf->write(_SrcBuf, _MaxCharCount, 0);

	if (pbuf->second._pBuf->isFull(2))
		clear_status(pbuf->second._writeflag);

	return send;//send;
}

int aldebaran::core::bufferManager::wait(fd_t fd, int iotype)
{
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return -1;
	switch (iotype)
	{
	case 1:
		pbuf->second._readflag.wait(false);
		break;
	case 2:
		pbuf->second._writeflag.wait(false);
		break;
	}

	return 0;
}

void aldebaran::core::bufferManager::close(fd_t fd)
{
	_bufContain.erase(_bufContain.find(fd));
}

void aldebaran::core::bufferManager::set_bufSize(fd_t fd, size_t _newSize, int _type)
{
	assert(_newSize > 0);
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return;
	switch (_type)
	{
	case 1:
		pbuf->second._pBuf->setReadBufMaxSize(_newSize);
		break;
	case 2:
		pbuf->second._pBuf->setWriteBufMaxSize(_newSize);
		break;
	default:
		break;
	}
	return;
}

int aldebaran::core::bufferManager::async_recv(fd_t fd)
{
	if (!_pthread.expired())
	{
		auto fun = [&, this]() {
			auto pbuf = _bufContain.find(fd);
			if (pbuf == _bufContain.end())
				return;
			int recvied = pbuf->second._pBuf->read(nullptr, 0, 0);

			if (recvied <= 0)
				error_handle(0, pbuf->second._pBuf->getHandle());

			set_status(pbuf->second._readflag);
			//close_status(pbuf->second._lock);
		};
		_pthread.lock()->addTask(threadPool2::handle_event<threadPool2::FK_EVENT::FK_READ>(fun));
	}

	return 0;
}

int aldebaran::core::bufferManager::async_send(fd_t fd)
{
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return -1;

	clear_status(pbuf->second._writeflag);

	if (!_pthread.expired())
	{
		auto fun = [fd, this]() {
			auto pbuf = _bufContain.find(fd);
			if (pbuf == _bufContain.end())
				return;

			int sended = pbuf->second._pBuf->write(nullptr, 0, 0);

			if (sended <= 0)
				error_handle(0, pbuf->second._pBuf->getHandle());

			set_status(pbuf->second._writeflag);
		};
		_pthread.lock()->addTask(threadPool2::handle_event<threadPool2::FK_EVENT::FK_WRITE>(fun));
	}
	else
		return -1;

	return 0;
}

int aldebaran::core::bufferManager::sync_recv(fd_t fd)
{
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return 0;

	int recvied = pbuf->second._pBuf->read(nullptr, 0, 0);

	if (recvied <= 0)
		error_handle(0, pbuf->second._pBuf->getHandle());

	set_status(pbuf->second._readflag);

	return recvied;
}

int aldebaran::core::bufferManager::sync_send(fd_t fd)
{
	auto pbuf = _bufContain.find(fd);
	if (pbuf == _bufContain.end())
		return 0;

	clear_status(pbuf->second._writeflag);

	int sended = pbuf->second._pBuf->write(nullptr, 0, 0);

	if (sended <= 0)
		error_handle(0, pbuf->second._pBuf->getHandle());

	set_status(pbuf->second._writeflag);

	return sended;
}

void aldebaran::core::bufferManager::error_handle(int errorN, EventHandle handle)
{
	if (_error_handle)
		_error_handle(errorN, handle);
	else
		defErrorHandle(errorN, handle);
}

void aldebaran::core::bufferManager::defErrorHandle(int errorN, EventHandle handle)
{
	net::close(handle);
}
inline
void aldebaran::core::bufferManager::set_status(std::atomic_flag& flag)
{
	if (!flag.test())
		flag.test_and_set();
	flag.notify_one();
}
inline
void aldebaran::core::bufferManager::clear_status(std::atomic_flag& flag)
{
	flag.clear();
}

//
//inline
//void
//aldebaran::core::bufferManager::close_status(std::atomic_flag& flag)
//{
//	if (flag.test())
//		flag.clear();
//}

//inline
//void
//aldebaran::core::bufferManager::set_status(std::atomic_flag& flag)
//{
//	if (!flag.test())
//		flag.test_and_set();
//	flag.notify_one();
//}