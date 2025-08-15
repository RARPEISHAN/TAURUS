//--------------------------------------------------------------------
//	Timer.h.
//	07/08/2020.				by bubo.
//	07/08/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _TIMER_H
#define _TIMER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <chrono>
#include "../Common/B_Object.hpp"
#include "../Common/Callbacks.h"

//--------------------------------------------------------------------
using namespace std::literals::chrono_literals;

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class Timer : public B_Object<Timer>
	{
	public:
		Timer(
			TimerCallback const& cb,
			std::chrono::system_clock::time_point when,
			std::chrono::duration<long long, std::milli> interval,
			bool disposable);

		~Timer();

		operator std::chrono::system_clock::time_point();

		bool update(void);

		void close(void);

		bool expire(void);

		operator std::function<void(void)>();

		bool isDisposable(void);
	private:

		TimerCallback                                  _timerCallback;

		std::chrono::system_clock::time_point          _when;

		std::chrono::duration<long long, std::milli>   _interval;

		bool                                           _expired = false;

		bool                                           _disposable;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_TIMER_H
