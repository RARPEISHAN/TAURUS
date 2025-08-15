//--------------------------------------------------------------------
//	NetAddress.h.
//	07/08/2020.				by bubo.
//	07/28/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _ADDRESS_H
#define _ADDRESS_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "../Common/Basics.h"
#include <variant>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class NetAddress : public B_Object<NetAddress>
	{
	public:
		NetAddress(unsigned short port, bool is_loopback_only = false, bool is_ipv6 = false);
		NetAddress(std::wstring const& ip, unsigned short port, bool is_ipv6 = false);
		NetAddress(ADDR_IPv4 const& ipv4);
		NetAddress(ADDR_IPv6 const& ipv6);

		~NetAddress();

		bool operator == (NetAddress const& other) const;

		ADDR_IP_COMMON const* raw_addr(void) const;
		Family family(void) const;
		std::wstring to_ip_str(void) const;
		std::wstring to_ip_port_str(void) const;
		unsigned short port(void) const;
		unsigned short port_net_endian(void) const;

		// sets the scope ID for ipv6.
		void set_scope_id(unsigned int scope_id);

		//--------------------------------------------------------------------
		// resolves hostname to IP address, not changing port or sin_family
		// returns true on success.
		// thread safe
		//--------------------------------------------------------------------
		static std::shared_ptr<NetAddress> resolve_host(
			std::wstring const& hostname,
			bool is_ipv6 = false);

		//--------------------------------------------------------------------

	private:
		std::variant<ADDR_IPv4, ADDR_IPv6>		_addr_socket;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_ADDRESS_H
