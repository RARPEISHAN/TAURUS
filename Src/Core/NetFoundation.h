//--------------------------------------------------------------------
//	*** Abstract Layer for Underlaying Network ***
//--------------------------------------------------------------------
//	NetFoundation.h.
//	07/08/2020.				by bubo.
//	07/14/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _NET_FOUNDATION_H
#define _NET_FOUNDATION_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <optional>
#include "../Common/Basics.h"
#include "NetFoundationTraits.h"

//--------------------------------------------------------------------
namespace aldebaran::core::net
{
	//--------------------------------------------------------------------
	EventHandle create_nonblocking_socket(Family family);
	void close(EventHandle);

	template <typename Family T>
	std::tuple<EventHandle, typename traits::AddressTraits<T>::Addr>
		accept(EventHandle socket_handle);

	bool listen(EventHandle socket_handle);
	bool bind(EventHandle socket_handle, ADDR_IP_COMMON const* addr);
	bool shutdown_write(EventHandle socket_handle);
	IOTYPE connect(EventHandle socket_handle, ADDR_IP_COMMON const* addr);

	std::tuple<long long, aldebaran::IOTYPE>
		read(EventHandle socket_handle, char* buf, long long max_size);
	std::tuple<long long, aldebaran::IOTYPE>
		send(EventHandle socket_handle, char const* buf, long long size);

	unsigned short host_to_net(unsigned short port);
	unsigned short net_to_host(unsigned short port);
	bool from_ip_port(std::wstring const& ip, unsigned short port, ADDR_IPv4& addr);
	bool from_ip_port(std::wstring const& ip, unsigned short port, ADDR_IPv6& addr);
	std::optional<ADDR_IPv4> get_local_addr_ipv4(EventHandle socket_handle);
	std::optional<ADDR_IPv6> get_local_addr_ipv6(EventHandle socket_handle);

	std::optional<std::wstring> ip_to_string(ADDR_IPv4 const& addr);
	std::optional<std::wstring> ip_to_string(ADDR_IPv6 const& addr);

	//--------------------------------------------------------------------
	//	host names to ip addresses.
	//--------------------------------------------------------------------
	bool address_from_name_v6(std::wstring const& host_name, ADDR_IPv6& addr);
	bool address_from_name(std::wstring const& host_name, ADDR_IPv4& addr);

	//--------------------------------------------------------------------
	//	features.
	//--------------------------------------------------------------------
	bool set_no_delay(EventHandle socket_handle, bool enable);
	bool set_keep_alive(EventHandle socket_handle, bool enable);
	bool set_reuse_addr(EventHandle socket_handle, bool enable);
	bool set_reuse_port(EventHandle socket_handle, bool enable);

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
//	Implementation.
//--------------------------------------------------------------------
#include "NetFoundationImp.h"

//--------------------------------------------------------------------
#endif // !_NET_FOUNDATION_H
