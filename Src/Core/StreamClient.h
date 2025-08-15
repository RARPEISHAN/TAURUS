//--------------------------------------------------------------------
//	StreamClient.h.
//	07/31/2020.				by bubo.
//	07/31/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _STREAM_CLIENT_H
#define _STREAM_CLIENT_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "./Connector.h"
#include "../Common/Callbacks.h"
#include "../threadsPool/thread_set.h"
#include "./NetAddress.h"
#include "./Buffers/streamBufferManager.h"
#include <future>
#include <optional>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class StreamClient : public B_Object_NonCopyable<StreamClient>
	{
	public:

		using Task = aldebaran::core::Connector::Task;

		StreamClient(std::shared_ptr<Reactor> reactor,
			std::shared_ptr<NetAddress> peer_addr,
			std::shared_ptr<NetAddress> local_addr);
		~StreamClient();

		std::optional<std::future<int>> connect();
		Task read(void* _DstBuf, unsigned int _MaxCharCount);
		Task write(void const* _Buf, unsigned int _MaxCharCount);
		void closeHandle();
		void setThreadsPool(std::shared_ptr<threadPool2::thread_set> thread_set);
	private:

		void read_handler(std::chrono::system_clock::time_point time_stamp);
		void write_handler();
		void error_handler();

	private:

		std::weak_ptr<Reactor>                  _reactor;
		std::shared_ptr<EventSwitcher>          _eventSwitcher;
		std::shared_ptr<NetAddress>             _local_addr;
		std::shared_ptr<NetAddress>             _peer_addr;
		std::shared_ptr<Socket>                 _socket;
		std::shared_ptr<Connector>              _connector;

		std::promise<int>                       _conP;

		std::weak_ptr<threadPool2::thread_set>  _threa_set;
		std::shared_ptr<bufferManager>          _pbuf;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_STREAM_CLIENT_H
