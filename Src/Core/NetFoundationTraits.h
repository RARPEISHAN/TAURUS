//--------------------------------------------------------------------
//	*** Abstract Layer for Underlaying Network ***
//--------------------------------------------------------------------
//	NetFoundationTraits.h.
//	07/30/2020.				by bubo.
//	07/30/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _NET_FOUNDATION_TRAITS_H
#define _NET_FOUNDATION_TRAITS_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/Basics.h"

//--------------------------------------------------------------------
namespace aldebaran::core::net::traits
{
	//--------------------------------------------------------------------
	//	traits for net address.
	//--------------------------------------------------------------------
	template <typename Family T>
	class AddressTraits
	{
	public:
		using Addr = ADDR_IP_COMMON;
	};

	//--------------------------------------------------------------------
	template <>
	class AddressTraits<AF_INET>
	{
	public:
		using Addr = ADDR_IPv4;
	};

	//--------------------------------------------------------------------
	template <>
	class AddressTraits<AF_INET6>
	{
	public:
		using Addr = ADDR_IPv6;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_NET_FOUNDATION_TRAITS_H
