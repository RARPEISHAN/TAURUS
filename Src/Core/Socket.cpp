//--------------------------------------------------------------------
//	Socket.cpp.
//	07/12/2020.				created.
//	07/31/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "Socket.h"
#include "NetFoundation.h"
#include "NetAddress.h"

//--------------------------------------------------------------------
aldebaran::core::Socket::Socket(EventHandle handle) : _socket_handle(handle),
_family(AF_INET)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::Socket::Socket(Socket&& other) noexcept
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::Socket::~Socket()
{
	//
}

//--------------------------------------------------------------------
aldebaran::EventHandle
aldebaran::core::Socket::event_handle(void)
const
{
	return _socket_handle;
}

//--------------------------------------------------------------------
bool
aldebaran::core::Socket::bind(std::shared_ptr<NetAddress> addr)
{
	_family = addr->family();

	return net::bind(
		_socket_handle,
		addr->raw_addr());
}

//--------------------------------------------------------------------
bool
aldebaran::core::Socket::listen(void)
{
	return net::listen(_socket_handle);
}

//--------------------------------------------------------------------
std::tuple<aldebaran::EventHandle, std::shared_ptr<aldebaran::core::NetAddress>>
aldebaran::core::Socket::accept(void)
{
	if (_family == AF_INET)
	{
		auto [handle, peer_addr] = net::accept<AF_INET>(_socket_handle);
		std::shared_ptr<aldebaran::core::NetAddress> addr(make_obj<NetAddress>(peer_addr));
		return { handle, addr };
	}
	else if (_family == AF_INET6)
	{
		auto [handle, peer_addr] = net::accept<AF_INET6>(_socket_handle);
		std::shared_ptr<aldebaran::core::NetAddress> addr(make_obj<NetAddress>(peer_addr));
		return { handle, addr };
	}

	return { INVALID_HANDLE,  nullptr };
}

//--------------------------------------------------------------------
void
aldebaran::core::Socket::shutdown_write(void)
{
	net::shutdown_write(_socket_handle);
}

//--------------------------------------------------------------------
void
aldebaran::core::Socket::set_no_delay(bool enable)
{
	net::set_no_delay(
		_socket_handle,
		enable);
}

//--------------------------------------------------------------------
void
aldebaran::core::Socket::set_reuse_addr(bool enable)
{
	net::set_reuse_addr(
		_socket_handle,
		enable);
}

//--------------------------------------------------------------------
void
aldebaran::core::Socket::set_reuse_port(bool enable)
{
	net::set_reuse_port(
		_socket_handle,
		enable);
}

//--------------------------------------------------------------------
void
aldebaran::core::Socket::set_keep_alive(bool enable)
{
	net::set_keep_alive(
		_socket_handle,
		enable);
}

//--------------------------------------------------------------------