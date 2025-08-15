//--------------------------------------------------------------------
//	Socket.h.
//	07/08/2020.				created.
//	07/15/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _SOCKET_H
#define _SOCKET_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "../Common/Basics.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete classes.
	//--------------------------------------------------------------------
	class NetAddress;

	//--------------------------------------------------------------------
	class Socket : public B_Object<Socket>
	{
	public:
		Socket(EventHandle handle);
		Socket(Socket&& other) noexcept;
		~Socket();

		EventHandle event_handle(void) const;

		// info(void); // tcp / udp info.
		bool bind(std::shared_ptr<NetAddress> addr);
		bool listen(void);

		//	If success, returns a non-negative integer that is
		//	a descriptor for the accepted socket, which has been
		//	set to non-blocking and close-on-exec. peer_addr is assigned.
		//	else error occurs, INVALID_HANDLE is returned, and peer_addr is untouched.
		//	[event handle, peer address].
		std::tuple<EventHandle, std::shared_ptr<NetAddress>>
			accept(void);

		void shutdown_write(void);

		// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
		void set_no_delay(bool enable);

		// Enable/disable SO_REUSEADDR
		void set_reuse_addr(bool enable);

		// Enable/disable SO_REUSEPORT
		void set_reuse_port(bool enable);

		// Enable/disable SO_KEEPALIVE
		void set_keep_alive(bool enable);

	private:
		EventHandle					_socket_handle;
		Family						_family;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_SOCKET_H
