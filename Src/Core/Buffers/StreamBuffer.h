//--------------------------------------------------------------------
//	StreamBuffer.h.
//	07/12/2020.				by bubo.
//	06/29/2021.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _STREAM_BUFFER_H
#define _STREAM_BUFFER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------

#include "../IBuffer.h"
#include "../../Common/Basics.h"
#include "../../Core/NetFoundation.h"
#include "../../Common/Callbacks.h"
#include "../../threadsPool/thread_set.h"
#include "../../threadsPool/handle_event.h"
#include "../../Core/NetFoundation.h"

#include <vector>
#include <atomic>
#include <algorithm>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	constexpr int BUFSIZE = 128 * 1024;
	class Reactor;
	class EventSwitcher;

	class buffer
	{
	public:

		void setMaxSize(size_t maxSize);
		buffer(EventHandle sock_handle, size_t bufferSize = BUFSIZE, size_t maxBufferSize = BUFSIZE);

		int write(const void* pBuffer, size_t charCount);

		int read(char* pDstBuf, size_t maxCharCount);

		int size();

		bool empty();

		int capacity();

		int maxCapacity();

		bool full();

		std::tuple<long long, aldebaran::IOTYPE> send();

	private:
		bool reserve(size_t newCapacity);

	private:

		std::vector<char>        _buf;

		int                      _maxBufSize = 0;
		int                      _front = 0;
		int                      _rear = 0;
		int                      _size = 0;
		int                      _bufSize = 0;

		aldebaran::EventHandle   _sock_handle;
	};

	//--------------------------------------------------------------------
	class StreamBuffer : public IBuffer
	{
	public:

		StreamBuffer(EventHandle handler,
			size_t buffSize,
			size_t maxBuffSize);
		virtual ~StreamBuffer();

		virtual unsigned int read(void* _DstBuf, unsigned int _MaxCharCount, int flags) override;
		virtual unsigned int write(void const* _Buf, unsigned int _MaxCharCount, int flags)override;
		virtual void setWriteBufMaxSize(size_t) override;
		virtual void setReadBufMaxSize(size_t)override;
		virtual EventHandle getHandle()override;
		virtual bool isEmpty(int) override;
		virtual bool isFull(int) override;

		/*void setReadFinishedCallBack(const ReadFinishCallBack&) ;
		void setWriteFinishedCallBack(const WriteFinishCallBack&) ;
		bool trySend();
		bool tryRecv();*/
	private:

		int recv();
		int send();

	private:

		/*	std::recursive_mutex          _readMx;
			std::recursive_mutex          _writeMx;

			ReadFinishCallBack            _readFinishCallBack;
			WriteFinishCallBack           _writeFinishCallBack;*/

	private:

		std::atomic_flag              _readLock = ATOMIC_FLAG_INIT;
		std::atomic_flag              _writeLock = ATOMIC_FLAG_INIT;

		buffer                        _writeBuffer;
		buffer                        _readBuffer;
		EventHandle                   _handle;
		int                           _recvFlags;
		int                           _sendFlags;

		//public:
		//	static std::weak_ptr<threadPool2::thread_set>         _pThreadsPool;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_STREAM_BUFFER_H
