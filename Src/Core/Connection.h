//--------------------------------------------------------------------
//	Connection.h.
//	07/08/2020.				by bubo.
//	07/31/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _CONNECTION_H
#define _CONNECTION_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <atomic>
#include "../Common/B_Object.hpp"
#include "../Common/Callbacks.h"
#include "../Common/Basics.h"
#include "./Buffers/streamBufferManager.h"

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
	class Connection : public B_Object_NonCopyable<Connection>
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

		/*	using ReadCallback = std::function<void(std::weak_ptr<Connection> connection, long long s, IOTYPE t)>;
			using WriteCallback = std::function<void(std::weak_ptr<Connection> connection, long long s, IOTYPE t)>;*/

		using Task = std::optional<task>;
		Connection(
			std::shared_ptr<Reactor> reactor,
			EventHandle event_handle,
			std::shared_ptr<NetAddress> local_addr,
			std::shared_ptr<NetAddress> peer_addr);

		~Connection();

		void set_connection_callback(ConnectionCallback const& cb);
		void set_data_callback(DataCallback const& cb);
		void set_close_callback(ConnectionCloseCallback const& cb);

		//set the thread pool
		//void set_threadsPool(std::weak_ptr<threadPool2::thread_set>);

		// create the buffer and set the buffer property
		void createBuffer(size_t initialSize, size_t maxBufSize);
		void setBuffer(std::weak_ptr<bufferManager> buf);
		void setReadBufMaxSize(size_t maxBufSize);
		void setWriteBufMaxSize(size_t maxBufSize);
		/*void setReadFinished(ReadCallback cb);
		void setWriteFinished(WriteCallback cb);*/

		//send and recv;
		Task send(void const* Source, unsigned int _maxCharCount);
		Task recv(char* Dest, unsigned int _maxCharCount);

		// called when Stream Server accepts a new connection
		// should be called only once
		void connect_established(void);

		// called when Stream Server has removed me from its map
		// should be called only once
		void connect_dropped(void);

		bool is_connected(void) const;
		bool is_disconnected(void) const;
		void shutdown(void);
		void force_close(void);
		void force_close(std::chrono::milliseconds delay);
		void close();

		void enable_writing(bool starUp = true);
		void enable_reading(bool starUp = true);

		TimerToken setTimer(std::shared_ptr<Timer> timer);

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
		EventHandle								_event_handle;
		std::shared_ptr<NetAddress>				_local_addr;
		std::shared_ptr<NetAddress>				_peer_addr;
		std::shared_ptr<EventSwitcher>			_eventSwitcher;
		std::shared_ptr<Socket>					_socket;
		ConnectionCallback						_connection_callback;
		DataCallback							_data_callback;
		ConnectionCloseCallback					_connection_close_callback;
		std::atomic<State>						_state{ State::Connecting };

	private:
		std::vector<TimerToken>                 _timer;
	private:

		/*	std::promise<int>                       _readTask;
			std::promise<int>                       _writeTask;*/

		std::weak_ptr<bufferManager>            _buf;
		fd_t                                    _fd;

		std::weak_ptr<threadPool2::thread_set>  _thread_set;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_CONNECTION_H
