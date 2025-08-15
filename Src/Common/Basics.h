//--------------------------------------------------------------------
//	Basics.h.
//	07/08/2020.				created.
//	07/21/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _BASICS_H
#define _BASICS_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <chrono>
#include <functional>
#include <limits>

#ifdef __TAURUS_WIN_ENV
	// 32KBytes in FDSet
#define FD_SETSIZE 8192
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <Ws2tcpip.h>
#include <ip2string.h>
#endif // __TAURUS_WIN_ENV

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
	//	Event Handle:
	//	a <SOCKET> for Windows platform.
	//	a <File Descriptor> for Linux / Unix platform.
	//--------------------------------------------------------------------
#ifdef __TAURUS_WIN_ENV
	// Windows
	using EventHandle = SOCKET;
	using Family = ADDRESS_FAMILY;
	typedef struct sockaddr_in ADDR_IPv4;
	typedef struct sockaddr_in6 ADDR_IPv6;
	typedef struct sockaddr ADDR_IP_COMMON;
	constexpr auto INVALID_HANDLE = INVALID_SOCKET;

#else
	// Linux / Unix
	using EventHandle = int;
	using Family = sa_family_t;
	typedef struct sockaddr_in ADDR_IPv4;
	typedef struct sockaddr_in6 ADDR_IPv6;
	typedef struct sockaddr ADDR_IP_COMMON;
	constexpr auto INVALID_HANDLE = -1;

#endif // __TAURUS_WIN_ENV

	//--------------------------------------------------------------------
	constexpr auto INVALID_FAMILY = (std::numeric_limits<unsigned short>::max)();

	//--------------------------------------------------------------------
	extern std::function<void(struct tm* const, time_t const* const)> local_time;

	//--------------------------------------------------------------------
	//	event type.
	//--------------------------------------------------------------------
	enum EventType
	{
		None = 0b0000,
		Read = 0b0001,
		Write = 0b0010,
		Error = 0b0100,
		Close = 0b1000,
	};

	//--------------------------------------------------------------------
	//	Event Switcher Handle Type. (used for internal only!)
	//--------------------------------------------------------------------
	enum class SwitcherHandleType
	{
		Read = 0,
		Write,
		Error
	};

	//--------------------------------------------------------------------
	enum class IOTYPE
	{
		OK = 0,
		Error,
		InProgress,
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_BASICS_H
