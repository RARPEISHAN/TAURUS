#pragma once

#include "./StreamBuffer.h"
#include "../../Common/B_Object.hpp"
#include "../IBuffer.h"
#include <map>
#include <atomic>
#include "../TimerQueue.h"
#include "../../threadsPool/thread_set.h"
#include <semaphore>

namespace aldebaran::core
{
	typedef unsigned long long fd_t;
	class bufferManager final
		:public B_Object_NonCopyable<bufferManager>
	{
		using ERROR_HANDLE = std::function<void(int errorN, EventHandle handle)>;

		struct buf
		{
			std::shared_ptr<IBuffer>          _pBuf;
			std::atomic_flag                  _readflag = ATOMIC_FLAG_INIT;
			std::atomic_flag                  _writeflag = ATOMIC_FLAG_INIT;
			buf(std::shared_ptr<IBuffer> pBuf) :_pBuf(pBuf) {
				_writeflag.test_and_set();
			}
		};

		size_t                                                        _maxBufSzie,
			_initialSize;
		std::map<unsigned long long,
			buf>                                                      _bufContain;

		static std::weak_ptr<threadPool2::thread_set>                 _pthread;

	public:
		bufferManager();

		void setthreadContext(std::weak_ptr<threadPool2::thread_set>);
		fd_t createBuffer(EventHandle soc, size_t initialSize, size_t maxBufSize);

		bool recv(fd_t fd, int* NumOfrecvied, int* errorN);
		bool send(fd_t fd, int* NumOfSended, int* errorN);

		int read(fd_t fd, void* _DstBuf, unsigned int _MaxCharCount);
		int write(fd_t fd, void* _SrcBuf, unsigned int _MaxCharCount);

		int wait(fd_t fd, int iotype);

		void close(fd_t fd);

		void set_bufSize(fd_t fd, size_t _newSize, int /*1是读缓冲区，2是写缓冲区*/ _type);

	private:
		int async_recv(fd_t fd);
		int async_send(fd_t fd);

		int sync_recv(fd_t fd);
		int sync_send(fd_t fd);

		void error_handle(int errorN, EventHandle handle);

	private:

		void defErrorHandle(int errorN, EventHandle handle);

		void set_status(std::atomic_flag& flag);

		void clear_status(std::atomic_flag& flag);

	private:
		ERROR_HANDLE                          _error_handle;
	};
}