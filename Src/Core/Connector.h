//--------------------------------------------------------------------
//	Connection.h.
//	07/08/2020.				by bubo.
//	07/31/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _CONNECTOR_H
#define _CONNECTOR_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <atomic>
#include "../Common/B_Object.hpp"
#include "../Common/Callbacks.h"
#include "../Common/Basics.h"
#include "./Buffers/streamBufferManager.h"
#include "./Socket.h"
#include "./EventSwitcher.h"
#include <future>
#include <optional>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete types.
	//--------------------------------------------------------------------
	class Reactor;
	class NetAddress;
	class EventSwitcher;
	class Socket;
	class threadPool2::thread_set;

	//--------------------------------------------------------------------
	class Connector : public B_Object_NonCopyable<Connector>
	{
		class task
		{
			fd_t                         _fd;
			std::weak_ptr<bufferManager> _pbuf;
			void* _Dest;
			size_t                       _maxCharCount;
			int                          _iotype;
			std::function<void(void)>    _cb;
		public:
			int get();
			task(fd_t fd, std::weak_ptr<bufferManager> pbuf, void* _Dest, size_t maxCharCount, int iotype);
			void set_data_calllback(std::function<void(void)> const& cb);
		};
	public:

		using Task = std::optional<task>;
		using ConnectorCloseCallback = std::function<void(std::shared_ptr<Connector>)>;
		/*using ReadCallback = std::function<void(long long s, IOTYPE t)>;
		using WriteCallback = std::function<void(long long s, IOTYPE t)>;*/

		Connector(
			std::weak_ptr<Reactor> reactor,
			std::shared_ptr<Socket> socket,
			std::shared_ptr<EventSwitcher> eventSwitcher);

		~Connector();

		void set_close_callback(ConnectorCloseCallback const& cb);

		//set the thread pool
		//void set_threadsPool(std::weak_ptr<threadPool2::thread_set> thread_set);

		// create the buffer and set the buffer property
		void createBuffer(size_t bufferSize, size_t maxBufSize);
		void setBuffer(std::weak_ptr<bufferManager> buf);
		void setReadBufMaxSize(size_t maxBufSize);
		void setWriteBufMaxSize(size_t maxBufSize);
		/*void setReadFinished(ReadCallback const& cb);
		void setWriteFinished(WriteCallback const& cb);*/

		//send and recv;
		Task send(void const* Source, unsigned int _maxCharCount);
		Task recv(char* Dest, unsigned int _maxCharCount);

		void enable_writing(bool starUp = true);
		void enable_reading(bool starUp = true);

		void connect_dropped(void);

		bool is_connected(void) const;
		bool is_disconnected(void) const;
		void shutdown(void);
		void force_close(void);
		void force_close(std::chrono::milliseconds delay);
		void close();

	private:
		enum class State
		{
			Connecting = 0,
			Connected,
			Disconnected,
			Disconnecting,
		};

		void set_state(State state);
		void read_handler(void);
		void close_handler(void);
		void write_handler(void);
		void error_handler(void);
		void shutdown_handler(void);
		void force_close_handler(void);

	public:
		unsigned long long int					_id = 0ull;

	private:

		std::weak_ptr<Reactor>					_reactor;
		EventHandle                             _event_handle;
		std::shared_ptr<EventSwitcher>			_eventSwitcher;
		std::shared_ptr<Socket>					_socket;
		ConnectorCloseCallback					_connection_close_callback;
		std::atomic<State>						_state{ State::Connecting };

		std::promise<int>                       _readTask;
		std::promise<int>                       _writeTask;

	private:

		std::weak_ptr<bufferManager>            _buf;
		fd_t                                    _fd;

		std::weak_ptr<threadPool2::thread_set>  _thread_set;
	};

	//--------------------------------------------------------------------

	//enum class BUFFERSTATUS
	//{
	//	Available,
	//	NotAvailable
	//};

//	//--------------------------------------------------------------------
//	class Task
//	{
//	public:
//		int get();
//		void set();
//	};
//
//
//	//--------------------------------------------------------------------
//
//	class completion
//	{
//
//	public:
//
//
//
//
//
//
//
//	};
//
//}

//--------------------------------------------------------------------
}
#endif // !_CONNECTION_H
