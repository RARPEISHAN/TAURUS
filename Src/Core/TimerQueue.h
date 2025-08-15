//--------------------------------------------------------------------
//	TimerQueue.h.
//	07/08/2020.				by bubo.
//	07/08/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _TIMER_QUEUE_H
#define _TIMER_QUEUE_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <chrono>
#include <map>
#include <set>
#include <future>
#include <condition_variable>
#include "../Common/B_Object.hpp"
#include "../Common/Callbacks.h"
#include "TimerToken.h"
#include "Reactor.h"
#include "./WindowsSupporting/alarm.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete classes.
	//--------------------------------------------------------------------
	class Reactor;
	class alarm;
	using time_point = std::chrono::system_clock::time_point;
	//--------------------------------------------------------------------
	class TimerQueue : public B_Object<TimerQueue>
	{
	public:
		TimerQueue(std::shared_ptr<Reactor> reactor);
		// non copyable.
		TimerQueue(TimerQueue const&) = delete;
		TimerQueue(TimerQueue&&) = delete;
		~TimerQueue();

		// handle timer alarm event.
		void handle_alarm(void);

		bool init(void);

		// be thread safe. may be called from other threads.

		TimerToken add_timer(std::shared_ptr<Timer> timer);

		void cancel(TimerToken const& id);

		std::vector<std::shared_ptr<Timer>> expired_timers(
			std::chrono::system_clock::time_point now);

		int run_alarm(void);

		void quit(void);

	private:
		std::chrono::milliseconds
			time_form_now(std::chrono::time_point<std::chrono::system_clock> when);

	private:
		std::weak_ptr<Reactor>				_reactor;
		std::multimap<std::chrono::system_clock::time_point, std::shared_ptr<Timer>>
			_timers;
		std::mutex                          _timersMx;

		std::condition_variable             _alarmCv;
		std::mutex                          _alarmMx;

		std::shared_ptr<alarm>              _alarm;

		std::atomic_bool                    _quit = false;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_TIMER_QUEUE_H
