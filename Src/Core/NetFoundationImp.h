//--------------------------------------------------------------------
//	*** Abstract Layer for Underlaying Network ***
//--------------------------------------------------------------------
//	NetFoundationImp.h.
//	07/30/2020.				by bubo.
//	07/30/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _NET_FOUNDATION_IMP_H
#define _NET_FOUNDATION_IMP_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <type_traits>
#include "../Taurus.h"

//--------------------------------------------------------------------
using namespace std::literals;

//--------------------------------------------------------------------
template <typename aldebaran::Family T>
inline
std::tuple<aldebaran::EventHandle, typename aldebaran::core::net::traits::AddressTraits<T>::Addr>
aldebaran::core::net::accept(aldebaran::EventHandle socket_handle)
{
	// check the type of address.
	typename traits::AddressTraits<T>::Addr handle_addr;

	if (std::is_same<typename traits::AddressTraits<T>::Addr, ADDR_IP_COMMON>())
	{
		nfx_print<info::SeverityType::Error>(L"Invalid Address!"s);
		return { INVALID_HANDLE, handle_addr };
	}

#ifdef __TAURUS_WIN_ENV
	// Windows.
	int len = sizeof(handle_addr);
	auto connected_handle = ::accept(
		socket_handle,
		reinterpret_cast<ADDR_IP_COMMON*>(&handle_addr),
		&len);

	// set the non-blocking state.
	auto val = 1ul;
	int ret = ::ioctlsocket(
		connected_handle,
		FIONBIO,
		static_cast<unsigned long*>(&val));

	if (ret == SOCKET_ERROR)
		nfx_print<info::SeverityType::Error>(L"Set non-blocking socket of accept failed!"s);
#else
	// Linux / BSD-Unix.

#endif // __TAURUS_WIN_ENV

	return { connected_handle, handle_addr };
}

//--------------------------------------------------------------------
#endif // !_NET_FOUNDATION_IMP_H
