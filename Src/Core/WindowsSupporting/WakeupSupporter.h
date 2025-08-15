//--------------------------------------------------------------------
//	WakeupSupporter.h.
//	08/03/2020.				by bubo.
//	08/03/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _WAKEUP_SUPPORTER_H
#define _WAKEUP_SUPPORTER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../../Common/B_Object.hpp"
#include "../../Common/Callbacks.h"
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
	class WakeupSupporter : public B_Object_NonCopyable<WakeupSupporter>
	{
	public:
		WakeupSupporter(
			std::shared_ptr<Reactor> reactor,
			std::shared_ptr<NetAddress> connecting_addr,
			std::shared_ptr<NetAddress> local_binding_addr);
		~WakeupSupporter();
		bool connect(void);
		void wakeup(char val);

	private:
		void read_handler(std::chrono::system_clock::time_point time_stamp);
		void write_handler(void);
		void error_handler(void);

	private:
		std::weak_ptr<Reactor>					_reactor;

		//--------------------------------------------------------------------
		//	*** EventSwitcher: _conn_eventSwitcher ***
		//--------------------------------------------------------------------
		// used for connecting to the wakeup-daemon &
		// waking up the wakeup-eventSwitcher of a reactor.
		//--------------------------------------------------------------------
		std::shared_ptr<EventSwitcher>			_conn_eventSwitcher;
		std::shared_ptr<Socket>					_conn_socket;
		std::shared_ptr<NetAddress>				_connecting_addr;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_WAKEUP_SUPPORTER_H
