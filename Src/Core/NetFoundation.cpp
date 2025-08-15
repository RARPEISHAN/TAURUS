//--------------------------------------------------------------------
//	*** Abstract Layer for Underlaying Network ***
//--------------------------------------------------------------------
//	NetFoundation.cpp.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "NetFoundation.h"
#include "../Common/Basics.h"
#include "../Taurus.h"

//--------------------------------------------------------------------
using namespace std::string_literals;

//--------------------------------------------------------------------
aldebaran::EventHandle
aldebaran::core::net::create_nonblocking_socket(Family family)
{
#ifdef __TAURUS_WIN_ENV
	// creates a streaming socket.
	EventHandle sock_handle = ::socket(family, SOCK_STREAM, 0);

	/*auto i = ::WSAGetLastError();*/

	if (sock_handle == INVALID_HANDLE)
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Create the listening socket failed!"s);

	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-ioctlsocket
	//--------------------------------------------------------------------
	//	The ioctlsocket function controls the I/O mode of a socket.
	//	set the non-blocking state.
	//--------------------------------------------------------------------
	auto val = 1ul;
	int ret = ::ioctlsocket(
		sock_handle,
		FIONBIO,
		static_cast<unsigned long*>(&val));

	if (ret == SOCKET_ERROR)
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Set non-blocking socket of accept failed!"s);

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return sock_handle;
}

//--------------------------------------------------------------------
void
aldebaran::core::net::close(EventHandle event_handle)
{
	if (event_handle == INVALID_HANDLE)
		return;

#ifdef __TAURUS_WIN_ENV
	if (::closesocket(event_handle) == SOCKET_ERROR)
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Close socket failed!"s);
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV
}

//--------------------------------------------------------------------
//	this will be changed to [Concept technology] in C++ 20.
//--------------------------------------------------------------------
//	IPv4 version.
//--------------------------------------------------------------------
//aldebaran::EventHandle
//aldebaran::core::net::accept(
//	EventHandle socket_handle,
//	ADDR_IPv4& handle_addr)
//{
//	if (socket_handle == INVALID_HANDLE)
//		// call the logger.
//		return INVALID_HANDLE;
//
//#ifdef __TAURUS_WIN_ENV
//	// Windows.
//	int len = sizeof(handle_addr);
//	auto connected_handle = ::accept(
//		socket_handle,
//		reinterpret_cast<ADDR_IP_COMMON*>(&handle_addr),
//		&len);
//
//	// set the non-blocking state.
//	auto val = 1ul;
//	int ret = ::ioctlsocket(
//		connected_handle,
//		FIONBIO,
//		static_cast<unsigned long*>(&val));
//
//	if (ret == SOCKET_ERROR)
//		nfx_print<info::SeverityType::Error>(L"Set non-blocking socket of accept failed!"s);
//#else
//	// Linux / BSD-Unix.
//
//#endif // __TAURUS_WIN_ENV
//
//	return connected_handle;
//}

//--------------------------------------------------------------------
//	IPv6 version.
//--------------------------------------------------------------------
//aldebaran::EventHandle
//aldebaran::core::net::accept(
//	EventHandle socket_handle,
//	ADDR_IPv6& handle_addr)
//{
//	if (socket_handle == INVALID_HANDLE)
//		// call the logger.
//		return INVALID_HANDLE;
//
//#ifdef __TAURUS_WIN_ENV
//	// Windows.
//	int len = sizeof(handle_addr);
//	auto connected_handle = ::accept(
//		socket_handle,
//		reinterpret_cast<ADDR_IP_COMMON*>(&handle_addr),
//		&len);
//
//	// set the non-blocking state.
//	auto val = 1ul;
//	int ret = ::ioctlsocket(
//		connected_handle,
//		FIONBIO,
//		static_cast<unsigned long*>(&val));
//
//	if (ret == SOCKET_ERROR)
//		nfx_print<info::SeverityType::Error>(L"Set non-blocking socket of accept failed!"s);
//#else
//	// Linux / BSD-Unix.
//
//#endif // __TAURUS_WIN_ENV
//
//	return connected_handle;
//}

//--------------------------------------------------------------------
bool
aldebaran::core::net::listen(EventHandle socket_handle)
{
	if (socket_handle == INVALID_HANDLE)
		return false;

#ifdef __TAURUS_WIN_ENV
	// Windows.
	if (::listen(
		socket_handle,
		SOMAXCONN) == SOCKET_ERROR)
		return false;
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
//	bind function for IPv4 / IPv6.
//--------------------------------------------------------------------
bool
aldebaran::core::net::bind(
	EventHandle socket_handle,
	ADDR_IP_COMMON const* addr)
{
	if (socket_handle == INVALID_HANDLE || !addr)
		return false;

	if (addr->sa_family != AF_INET &&
		addr->sa_family != AF_INET6)
		return false;

#ifdef __TAURUS_WIN_ENV
	// Windows.
	if (::bind(
		socket_handle,
		addr,
		addr->sa_family == AF_INET ? sizeof(ADDR_IPv4) : sizeof(ADDR_IPv6)) == SOCKET_ERROR)
		return false;
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::shutdown_write(EventHandle socket_handle)
{
#ifdef __TAURUS_WIN_ENV
	// Windows.
	if (::shutdown(socket_handle, SD_SEND) == SOCKET_ERROR)
		return false;

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
aldebaran::IOTYPE
aldebaran::core::net::connect(
	EventHandle socket_handle,
	ADDR_IP_COMMON const* addr)
{
	if (socket_handle == INVALID_HANDLE || !addr)
		return IOTYPE::Error;

#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/Winsock2/nf-winsock2-connect
	//--------------------------------------------------------------------
	//	The connect function is used to create a connection to the specified destination.
	//	If socket s, is unbound, unique values are assigned to the
	//	local association by the system, and the socket is marked as bound.
	//	For connection - oriented sockets(for example, type SOCK_STREAM),
	//	an active connection is initiated to the foreign host using name
	//	(an address in the namespace of the socket; for a detailed description,
	//	see bindand sockaddr).
	//--------------------------------------------------------------------
	if (::connect(
		socket_handle,
		addr,
		addr->sa_family == AF_INET ? sizeof(ADDR_IPv4) : sizeof(ADDR_IPv6))
		!= SOCKET_ERROR)
		return IOTYPE::OK;

	auto ret = ::WSAGetLastError();

	switch (ret)
	{
	case WSAEWOULDBLOCK:
		return IOTYPE::InProgress;
	case WSANOTINITIALISED:
	case WSAENETDOWN:
	case WSAEADDRINUSE:
	case WSAEINTR:
	case WSAEINPROGRESS:
	case WSAEALREADY:
	case WSAEADDRNOTAVAIL:
	case WSAEAFNOSUPPORT:
	case WSAECONNREFUSED:
	case WSAEFAULT:
	case WSAEINVAL:
	case WSAEISCONN:
	case WSAENETUNREACH:
	case WSAEHOSTUNREACH:
	case WSAENOBUFS:
	case WSAENOTSOCK:
	case WSAETIMEDOUT:
	case WSAEACCES:
	default:
		return IOTYPE::Error;
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV
}

//--------------------------------------------------------------------
std::tuple<long long, aldebaran::IOTYPE>
aldebaran::core::net::read(
	EventHandle socket_handle,
	char* buf,
	long long max_size)
{
	if (!buf || max_size == 0ll)
		return { 0ll, aldebaran::IOTYPE::Error };

#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recv
	//--------------------------------------------------------------------
	//	If no error occurs, recv returns the number of bytes received
	//	and the buffer pointed to by the buf parameter will contain
	//	this data received. If the connection has been gracefully closed,
	//	the return value is zero.
	//	Otherwise, a value of SOCKET_ERROR is returned,
	//	and a specific error code can be retrieved by calling WSAGetLastError.
	//--------------------------------------------------------------------
	long long ret = ::recv(socket_handle, buf, max_size, 0);
	if (ret != SOCKET_ERROR)
		return { ret, aldebaran::IOTYPE::Error };

	auto error_code = ::WSAGetLastError();
	switch (error_code)
	{
		// The socket is marked as nonblocking and the receive operation would block.
	case WSAEWOULDBLOCK:
		return { ret, aldebaran::IOTYPE::InProgress };
	default:
		return { ret, aldebaran::IOTYPE::Error };
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return { ret, aldebaran::IOTYPE::Error };
}

//--------------------------------------------------------------------
std::tuple<long long, aldebaran::IOTYPE>
aldebaran::core::net::send(
	EventHandle socket_handle,
	char const* buf,
	long long size)
{
	if (!buf || size == 0ll)
		return { 0ll, IOTYPE::Error };

#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send
	//--------------------------------------------------------------------
	//	The send function is used to write outgoing data on a connected socket.
	//	For message - oriented sockets(address family of AF_INET or AF_INET6,
	//	type of SOCK_DGRAM, and protocol of IPPROTO_UDP, for example),
	//	care must be taken not to exceed the maximum packet size of the underlying provider.
	//	The maximum message packet size for a provider can be obtained by calling getsockopt
	//	with the optname parameter set to SO_MAX_MSG_SIZE to retrieve the value of socket option.
	//	If the data is too long to pass atomically through the underlying protocol,
	//	the error WSAEMSGSIZE is returned, and no data is transmitted.
	//	The successful completion of a send function does not indicate that
	//	the data was successfully deliveredand received to the recipient.
	//	This function only indicates the data was successfully sent.
	//	If no buffer space is available within the transport system to hold
	//	the data to be transmitted, send will block unless the socket has been
	//	placed in nonblocking mode.On nonblocking stream oriented sockets,
	//	the number of bytes written can be between 1 and the requested length,
	//	depending on buffer availability on both the clientand server computers.
	//	The select, WSAAsyncSelect or WSAEventSelect functions can be used to
	//	determine when it is possible to send more data.
	//--------------------------------------------------------------------
	long long ret = ::send(socket_handle, buf, size, 0);
	if (ret != SOCKET_ERROR)
		return { ret, IOTYPE::OK };

	auto error_code = ::WSAGetLastError();
	switch (error_code)
	{
		// The socket is marked as nonblocking and the receive operation would block.
	case WSAEWOULDBLOCK:
		return { ret, aldebaran::IOTYPE::InProgress };
	default:
		return { ret, aldebaran::IOTYPE::Error };
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return { 0ll, IOTYPE::Error };
}

//--------------------------------------------------------------------
unsigned short
aldebaran::core::net::host_to_net(unsigned short port)
{
	unsigned short net_port = 0u;

#ifdef __TAURUS_WIN_ENV
	// Windows.
	net_port = ::htons(port);
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return net_port;
}

//--------------------------------------------------------------------
unsigned short
aldebaran::core::net::net_to_host(unsigned short port)
{
	unsigned short host_port = 0u;

#ifdef __TAURUS_WIN_ENV
	// Windows.
	host_port = ::ntohs(port);
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return host_port;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::from_ip_port(
	std::wstring const& ip,
	unsigned short port,
	ADDR_IPv4& addr)
{
	if (ip.length() == 0)
		return false;

	addr.sin_family = AF_INET;
	addr.sin_port = net::host_to_net(port);

#ifdef __TAURUS_WIN_ENV
	// Windows.
	if (::InetPtonW(
		AF_INET,
		ip.c_str(),
		static_cast<void*>(&addr.sin_addr.S_un.S_addr)) != 1)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: IP address converting failed!"s);
		return false;
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::from_ip_port(
	std::wstring const& ip,
	unsigned short port,
	ADDR_IPv6& addr)
{
	if (ip.length() == 0)
		return false;

	addr.sin6_family = AF_INET6;
	addr.sin6_port = net::host_to_net(port);

#ifdef __TAURUS_WIN_ENV
	// Windows.
	if (::InetPtonW(
		AF_INET,
		ip.c_str(),
		static_cast<void*>(&addr.sin6_addr)) != 1)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: IP address converting failed!"s);
		return false;
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
std::optional<aldebaran::ADDR_IPv4>
aldebaran::core::net::get_local_addr_ipv4(EventHandle socket_handle)
{
	ADDR_IPv4 addr4 = { 0 };
	int len = sizeof addr4;

#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-getsockname
	//--------------------------------------------------------------------
	//	The getsockname function retrieves the current name for the specified
	//	socket descriptor in name.
	//	It is used on the bound or connected socket specified by the s parameter.
	//	The local association is returned. This call is especially useful
	//	when a connect call has been made without doing a bind first;
	//	the getsockname function provides the only way to determine the
	//	local association that has been set by the system.
	//--------------------------------------------------------------------
	if (::getsockname(
		socket_handle,
		reinterpret_cast<ADDR_IP_COMMON*>(&addr4),
		&len) == SOCKET_ERROR)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Get local address failed!"s);
		return std::nullopt;
	}

	if (len != sizeof addr4)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Get local address failed!"s);
		return std::nullopt;
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return addr4;
}

//--------------------------------------------------------------------
std::optional<aldebaran::ADDR_IPv6>
aldebaran::core::net::get_local_addr_ipv6(EventHandle socket_handle)
{
	ADDR_IPv6 addr6 = { 0 };
	int len = sizeof addr6;

#ifdef __TAURUS_WIN_ENV
	//	Windows.
	//	The getsockname function retrieves the local name for a socket .
	if (::getsockname(
		socket_handle,
		reinterpret_cast<ADDR_IP_COMMON*>(&addr6),
		&len) == SOCKET_ERROR)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Get local address failed!"s);
		return std::nullopt;
	}

	if (len != sizeof addr6)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Get local address failed!"s);
		return std::nullopt;
	}

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return addr6;
}

//--------------------------------------------------------------------
std::optional<std::wstring>
aldebaran::core::net::ip_to_string(ADDR_IPv4 const& addr)
{
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetntopw
	//--------------------------------------------------------------------
	//	For an IPv4 address, this buffer should be
	//	large enough to hold at least 16 characters.
	//--------------------------------------------------------------------
	wchar_t buf[20];

#ifdef __TAURUS_WIN_ENV
	// Windows.
	if (::InetNtopW(AF_INET6, &addr.sin_addr, buf, 20))
		return std::wstring(buf);
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return std::nullopt;
}

//--------------------------------------------------------------------
std::optional<std::wstring>
aldebaran::core::net::ip_to_string(ADDR_IPv6 const& addr)
{
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetntopw
	//--------------------------------------------------------------------
	//	For an IPv6 address, this buffer should be
	//	large enough to hold at least 46 characters.
	//--------------------------------------------------------------------
	wchar_t buf[50];

#ifdef __TAURUS_WIN_ENV
	// Windows.
	//::RtlIpv6AddressToStringW(&addr.sin6_addr, buf);
	if (::InetNtopW(AF_INET6, &addr.sin6_addr, buf, 50))
		return std::wstring(buf);
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return std::nullopt;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::address_from_name_v6(
	std::wstring const& host_name,
	ADDR_IPv6& addr)
{
#ifdef __TAURUS_WIN_ENV
	// Windows.
	ADDRINFOW hints;
	ADDRINFOW* result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_family = AF_INET6;

	if (::GetAddrInfoW(host_name.c_str(), L"", &hints, &result) != 0)
		return false;

	ADDR_IPv6* paddr = (ADDR_IPv6*)(result->ai_addr);
	addr = *paddr;
	FreeAddrInfoW(result);

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::address_from_name(
	std::wstring const& host_name,
	ADDR_IPv4& addr)
{
#ifdef __TAURUS_WIN_ENV
	// Windows.
	ADDRINFOW hints;
	ADDRINFOW* result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_family = AF_INET;
	if (::GetAddrInfoW(host_name.c_str(), L"", &hints, &result) != 0)
		return false;

	ADDR_IPv4* paddr = (ADDR_IPv4*)(result->ai_addr);
	addr = *paddr;
	::FreeAddrInfoW(result);

#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::set_no_delay(
	EventHandle socket_handle,
	bool enable)
{
#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/winsock/ipproto-tcp-socket-options
	//--------------------------------------------------------------------
	//	Enables or disables the Nagle algorithm for TCP sockets.
	//	This option is disabled (set to FALSE) by default.
	//--------------------------------------------------------------------
	if (::setsockopt(
		socket_handle,
		IPPROTO_TCP,
		TCP_NODELAY,
		reinterpret_cast<char const*>(&enable),
		sizeof enable) == SOCKET_ERROR)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Socket setting no-delay failed!"s);
		return false;
	}

#else
	// Linux / BSD-Unix.
	::setsockopt(
		socket_handle,
		IPPROTO_TCP,
		TCP_NODELAY,
		reinterpret_cast<char const*>(&enable),
		sizeof enable);

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::set_keep_alive(
	EventHandle socket_handle,
	bool enable)
{
#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/winsock/sol-socket-socket-options
	//--------------------------------------------------------------------
	//	Enables keep-alive for a socket connection.
	//	Valid only for protocols that support the notion of keep-alive
	//	(connection-oriented protocols). For TCP, the default keep-alive
	//	timeout is 2 hours and the keep-alive interval is 1 second.
	//	The default number of keep-alive probes varies based on the version of Windows.
	//	See the SO_KEEPALIVE reference for more information.
	//--------------------------------------------------------------------
	if (::setsockopt(
		socket_handle,
		SOL_SOCKET,
		SO_KEEPALIVE,
		reinterpret_cast<char const*>(&enable),
		sizeof enable) == SOCKET_ERROR)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Socket setting no-delay failed!"s);
		return false;
	}

#else
	// Linux / BSD-Unix.
	::setsockopt(
		socket_handle,
		SOL_SOCKET,
		SO_KEEPALIVE,
		reinterpret_cast<char const*>(&enable),
		sizeof enable);

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::set_reuse_addr(
	EventHandle socket_handle,
	bool enable)
{
#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/winsock/sol-socket-socket-options
	//--------------------------------------------------------------------
	//	Allows a socket to bind to an address and port already in use.
	//	The SO_EXCLUSIVEADDRUSE option can prevent this.
	//--------------------------------------------------------------------
	if (::setsockopt(
		socket_handle,
		SOL_SOCKET,
		SO_REUSEADDR,
		reinterpret_cast<char const*>(&enable),
		sizeof enable) == SOCKET_ERROR)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Socket setting reuse-address failed!"s);
		return false;
	}

#else
	// Linux / BSD-Unix.
	::setsockopt(
		socket_handle,
		SOL_SOCKET,
		SO_REUSEADDR,
		reinterpret_cast<char const*>(&enable),
		sizeof enable);

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------
bool
aldebaran::core::net::set_reuse_port(
	EventHandle socket_handle,
	bool enable)
{
#ifdef __TAURUS_WIN_ENV
	//--------------------------------------------------------------------
	//	Windows.
	//--------------------------------------------------------------------
	//	referred to:
	//	https://docs.microsoft.com/en-us/windows/win32/winsock/sol-socket-socket-options
	//--------------------------------------------------------------------
	//	Allows a socket to bind to an address and port already in use.
	//	The SO_EXCLUSIVEADDRUSE option can prevent this.
	//--------------------------------------------------------------------
	if (::setsockopt(
		socket_handle,
		SOL_SOCKET,
		SO_REUSEADDR,
		reinterpret_cast<char const*>(&enable),
		sizeof enable) == SOCKET_ERROR)
	{
		nfx_print<info::SeverityType::Error>(L"Taurus Core: Socket setting reuse-address failed!"s);
		return false;
	}

#else
	// Linux / BSD-Unix.
	::setsockopt(
		socket_handle,
		SOL_SOCKET,
		SO_REUSEPORT,
		reinterpret_cast<char const*>(&enable),
		sizeof enable);

#endif // __TAURUS_WIN_ENV

	return true;
}

//--------------------------------------------------------------------