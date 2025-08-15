//--------------------------------------------------------------------
//	Acceptor.h.
//	07/08/2020.				by bubo.
//	07/14/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "../Common/Basics.h"
#include "../Common/Callbacks.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete types.
	//--------------------------------------------------------------------
	class Reactor;
	class NetAddress;
	class Socket;
	class EventSwitcher;

	//--------------------------------------------------------------------
	class Acceptor : public B_Object_NonCopyable<Acceptor>
	{
	public:
		Acceptor(
			std::shared_ptr<Reactor> reactor,
			std::shared_ptr<NetAddress> listen_addr);

		~Acceptor();
		void set_new_connection_callback(NewConnectionCallback const& cb);
		void listen(void);
		bool is_listenning(void) const;

	private:
		void read_handler(std::chrono::system_clock::time_point time_stamp);

	private:
		std::weak_ptr<Reactor>			_reactor;
		std::shared_ptr<Socket>			_accept_socket;
		std::shared_ptr<EventSwitcher>	_accept_event_switcher;
		NewConnectionCallback			_new_connection_callback;
		bool							_is_listenning = false;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_ACCEPTOR_H
