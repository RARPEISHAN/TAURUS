//--------------------------------------------------------------------
//	StreamServer.h.
//	07/08/2020.				created.
//	08/01/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _STREAM_SERVER_H
#define _STREAM_SERVER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <map>
#include "../Common/B_Object.hpp"
#include "../Common/Callbacks.h"
#include "../threadsPool/thread_set.h"
#include "../Core/Connection.h"
#include "./Buffers/streamBufferManager.h"
#include <condition_variable>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete classes.
	//--------------------------------------------------------------------
	class Reactor;
	class Acceptor;
	class NetAddress;

	//--------------------------------------------------------------------
	class StreamServer : public B_Object_NonCopyable<StreamServer>
	{
	public:
		StreamServer(
			std::shared_ptr<Reactor> reactor,
			std::shared_ptr<NetAddress> listen_addr);

		~StreamServer();

		void start(void);
		void set_connection_callback(ConnectionCallback const& cb);
		void set_data_callback(DataCallback const& cb);

		/*	void set_read_callback(ReadCallback const& cb);
			void set_write_callback(WriteCallback const& cb);*/
	public:

		void set_threadsPool(std::weak_ptr<threadPool2::thread_set> pThreadsPool);
		auto accept() {
			return _connectTasks.getTask();
		}

	private:
		// callback method.
		void new_connection(
			EventHandle event_handle,
			std::shared_ptr<core::NetAddress> peer_addr);
		void remove_connection(std::shared_ptr<Connection> connection);

	private:
		bool									_is_started = false;
		std::map<unsigned long long, std::shared_ptr<Connection>>
			_connections;
		struct conContainer
			:std::queue<std::weak_ptr<Connection>>
		{
			using Base = std::queue <std::weak_ptr<Connection>>;
		public:
			class task {
				conContainer* _pBase;
			public:
				task(task const& other) {
					_pBase = other._pBase;
				}
				task(conContainer* pBase) :_pBase(pBase) {}
				task(task&& other) {
					_pBase = other._pBase;
				}
				std::optional<std::weak_ptr<Connection>> get() {
					if (_pBase)
					{
						if (!_pBase->empty())
							return _pBase->pop();
						else
						{
							_pBase->wait();
							if (!_pBase->empty())
								return _pBase->pop();
							else
								return std::nullopt;
						}
					}
					return std::nullopt;
				}
			};
			bool empty(void)
			{
				std::lock_guard lck(_mx);
				return Base::empty();
			}
			auto getTask() ->task {
				return task{ this };
			}
			void push_back(std::weak_ptr<Connection> con)
			{
				std::lock_guard lck(_mx);
				Base::push(con);
				_cv.notify_one();
			}
			auto pop()->std::weak_ptr<Connection>
			{
				std::lock_guard lck(_mx);
				auto con = Base::front();
				Base::pop();
				return con;
			}
			void wait() {
				std::unique_lock lck(_cvMx);
				_cv.wait(lck);
			}

		private:
			std::mutex                          _mx, _cvMx;
			std::condition_variable             _cv;
		}
		_connectTasks;

		ConnectionCallback						_connection_callback;
		DataCallback							_data_callback;

		/*ReadCallback                            _read_callback;
		WriteCallback                           _write_callback;*/

		std::weak_ptr<Reactor>					_reactor;
		std::shared_ptr<Acceptor>				_acceptor;

	private:
		std::weak_ptr<threadPool2::thread_set>  _pThreadsPool;
		std::shared_ptr<bufferManager>          _pbuf;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_STREAM_SERVER_H
