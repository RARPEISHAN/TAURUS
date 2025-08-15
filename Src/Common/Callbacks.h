//--------------------------------------------------------------------
//	Callbacks.h.
//	07/08/2020.				by bubo.
//	08/02/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _CALLBACKS_H
#define _CALLBACKS_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "Basics.h"

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
	//	incomplete types.
	//--------------------------------------------------------------------
	namespace core
	{
		class NetAddress;
		class Connection;
		class IBuffer;
	}

	//--------------------------------------------------------------------
	// callback types.
	//--------------------------------------------------------------------
	using Functor = std::function<void(void)>;
	using CallbackFunc = std::function<void(void)>;
	using TimerCallback = std::function<void(void)>;
	using ReadCallback = std::function<void(std::chrono::system_clock::time_point time_stamp)>;
	using NewConnectionCallback = std::function<void(EventHandle, std::shared_ptr<core::NetAddress>)>;
	using ConnectionCallback = std::function<void(std::shared_ptr<core::Connection>)>;
	using ConnectionCloseCallback = std::function<void(std::shared_ptr<core::Connection>)>;
	using DataCallback = std::function<
		void(std::shared_ptr<core::Connection>,
			std::shared_ptr<core::IBuffer>,
			std::chrono::system_clock::time_point time_stamp)>;

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_CALLBACKS_H
