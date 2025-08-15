//--------------------------------------------------------------------
//	NetAddress.cpp.
//	07/08/2020.				by bubo.
//	07/31/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "NetAddress.h"
#include "NetFoundation.h"
#include <sstream>

//--------------------------------------------------------------------
aldebaran::core::NetAddress::NetAddress(
	unsigned short port,
	bool is_loopback_only /*= false*/,
	bool is_ipv6 /*= false*/)
{
	// IPv6.
	if (is_ipv6)
	{
		ADDR_IPv6 addr6 = { 0 };
		addr6.sin6_family = AF_INET6;
		addr6.sin6_port = net::host_to_net(port);
		addr6.sin6_addr = is_loopback_only ? in6addr_loopback : in6addr_any;
		_addr_socket = addr6;
	}
	// IPv4.
	else
	{
		ADDR_IPv4 addr4 = { 0 };
		addr4.sin_family = AF_INET;
		addr4.sin_port = net::host_to_net(port);
		addr4.sin_addr = is_loopback_only ? in4addr_loopback : in4addr_any;
		_addr_socket = addr4;
	}
}

//--------------------------------------------------------------------
aldebaran::core::NetAddress::NetAddress(
	std::wstring const& ip,
	unsigned short port,
	bool is_ipv6)
{
	// IPv6.
	if (is_ipv6)
	{
		ADDR_IPv6 addr6 = { 0 };
		if (net::from_ip_port(ip, port, addr6))
			_addr_socket = addr6;
	}
	// IPv4.
	else
	{
		ADDR_IPv4 addr4 = { 0 };
		if (net::from_ip_port(ip, port, addr4))
			_addr_socket = addr4;
	}
}

//--------------------------------------------------------------------
aldebaran::core::NetAddress::NetAddress(ADDR_IPv4 const& ipv4)
{
	_addr_socket = ipv4;
}

//--------------------------------------------------------------------
aldebaran::core::NetAddress::NetAddress(ADDR_IPv6 const& ipv6)
{
	_addr_socket = ipv6;
}

//--------------------------------------------------------------------
aldebaran::core::NetAddress::~NetAddress()
{
	//
}

//--------------------------------------------------------------------
bool
aldebaran::core::NetAddress::operator==(NetAddress const& other) const
{
	if (_addr_socket.index() == 0 && other._addr_socket.index() == 0)
	{
		auto s4 = std::get<ADDR_IPv4>(_addr_socket);
		auto o4 = std::get<ADDR_IPv4>(other._addr_socket);
		if (s4.sin_addr.S_un.S_addr == o4.sin_addr.S_un.S_addr &&
			s4.sin_port == o4.sin_port)
			return true;
		else
			return false;
	}
	else if (_addr_socket.index() == 1 && other._addr_socket.index() == 1)
	{
		auto s6 = std::get<ADDR_IPv6>(_addr_socket);
		auto o6 = std::get<ADDR_IPv6>(other._addr_socket);
		if (std::memcmp(s6.sin6_addr.u.Byte, o6.sin6_addr.u.Byte,
			sizeof(s6.sin6_addr.u.Byte)) == 0 && s6.sin6_port == o6.sin6_port)
			return true;
		else
			return false;
	}

	return false;
}

//--------------------------------------------------------------------
aldebaran::ADDR_IP_COMMON const*
aldebaran::core::NetAddress::raw_addr(void)
const
{
	if (_addr_socket.index() == 0)
		return reinterpret_cast<ADDR_IP_COMMON const*>(&std::get<ADDR_IPv4>(_addr_socket));
	else if (_addr_socket.index() == 1)
		return reinterpret_cast<ADDR_IP_COMMON const*>(&std::get<ADDR_IPv6>(_addr_socket));

	return nullptr;
}

//--------------------------------------------------------------------
aldebaran::Family
aldebaran::core::NetAddress::family(void)
const
{
	if (auto addr = raw_addr())
		return addr->sa_family;

	return INVALID_FAMILY;
}

//--------------------------------------------------------------------
std::wstring
aldebaran::core::NetAddress::to_ip_str(void)
const
{
	if (_addr_socket.index() == 0)
		return net::ip_to_string(std::get<ADDR_IPv4>(_addr_socket)).value_or(L""s);
	else if (_addr_socket.index() == 1)
		return net::ip_to_string(std::get<ADDR_IPv6>(_addr_socket)).value_or(L""s);

	return L""s;
}

//--------------------------------------------------------------------
std::wstring
aldebaran::core::NetAddress::to_ip_port_str(void)
const
{
	auto ip_str = to_ip_str();
	if (ip_str.length() == 0)
		return L""s;

	std::wstringstream ss;
	ss << ip_str << L":" << port();
	return ss.str();
}

//--------------------------------------------------------------------
unsigned short
aldebaran::core::NetAddress::port(void)
const
{
	unsigned short port = 0u;

	if (_addr_socket.index() == 0)
		port = std::get<ADDR_IPv4>(_addr_socket).sin_port;
	else if (_addr_socket.index() == 1)
		port = std::get<ADDR_IPv6>(_addr_socket).sin6_port;

	return net::net_to_host(port);
}

//--------------------------------------------------------------------
unsigned short
aldebaran::core::NetAddress::port_net_endian(void)
const
{
	unsigned short port = 0u;

	if (_addr_socket.index() == 0)
		port = std::get<ADDR_IPv4>(_addr_socket).sin_port;
	else if (_addr_socket.index() == 1)
		port = std::get<ADDR_IPv6>(_addr_socket).sin6_port;

	return port;
}

//--------------------------------------------------------------------
void
aldebaran::core::NetAddress::set_scope_id(unsigned int scope_id)
{
	if (family() == AF_INET6)
		try
	{
		auto& addr6 = std::get<ADDR_IPv6>(_addr_socket);
		addr6.sin6_scope_id = scope_id;
	}
	catch (...)
	{
		return;
	}
}

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::core::NetAddress>
aldebaran::core::NetAddress::resolve_host(
	std::wstring const& hostname,
	bool is_ipv6 /*= false*/)
{
	if (hostname.length() == 0)
		return nullptr;

	if (is_ipv6)
	{
		ADDR_IPv6 addr6 = { 0 };
		if (net::address_from_name_v6(hostname, addr6))
			return make_obj<NetAddress>(addr6);
	}
	else
	{
		ADDR_IPv4 addr4 = { 0 };
		if (net::address_from_name(hostname, addr4))
			return make_obj<NetAddress>(addr4);
	}

	return nullptr;
}

//--------------------------------------------------------------------