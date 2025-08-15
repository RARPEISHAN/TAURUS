//--------------------------------------------------------------------
//	WakeupDaemon.h.
//	08/03/2020.				by bubo.
//	08/03/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _WAKEUP_DAEMON_H
#define _WAKEUP_DAEMON_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../../Common/B_Object.hpp"
#include "../../Common/Callbacks.h"
#include <map>
#include <optional>
#include <mutex>

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

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
namespace aldebaran::core::win
{
	//--------------------------------------------------------------------
	class WakeupDaemon : public B_Object_NonCopyable<WakeupDaemon>
	{
	public:
		WakeupDaemon(
			std::shared_ptr<Reactor> reactor,
			std::shared_ptr<NetAddress> listening_addr);
		~WakeupDaemon();

		void listen(void);

	private:
		void add_new_conn_callback(
			std::shared_ptr<NetAddress> conn_addr,
			NewConnectionCallback const& cb);
		void read_handler(std::chrono::system_clock::time_point time_stamp);
		void write_handler(void);
		void error_handler(void);
		std::optional<NewConnectionCallback> find_connection_callback(std::shared_ptr<NetAddress> addr);

		//--------------------------------------------------------------------
		void setNewConnectionCallback(NewConnectionCallback const& cb);
		//--------------------------------------------------------------------
	private:
		std::weak_ptr<Reactor>					_reactor;
		std::shared_ptr<EventSwitcher>			_listening_eventSwitcher;
		std::shared_ptr<Socket>					_listening_socket;
		std::map<std::shared_ptr<NetAddress>, NewConnectionCallback>
			_new_connection_callbacks;

		NewConnectionCallback                   _connection_callback;

		bool									_is_listenning = false;
		std::mutex								_new_cb_callbacks_mx;

	private:
		friend class Reactor;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_WAKEUP_DAEMON_H
