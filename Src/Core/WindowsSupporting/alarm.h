#pragma once
#ifndef _ALARM_H
#define _ALARM_H

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <optional>
#include "../TimerQueue.h"

namespace aldebaran::core
{
	using namespace std::chrono_literals;

	class TimerQueue;
	class Timer;
	class alarm
	{
		std::condition_variable                 _alarmCv;
		std::mutex                              _alarmMx;
		std::weak_ptr<TimerQueue>               _timers;
	public:
		alarm(std::shared_ptr<TimerQueue> timerQueue);
		std::optional<std::vector<std::shared_ptr<Timer>>> wait(std::chrono::milliseconds waitTime);
		void interrupt(void);
	};
}

#endif // !_ALARM_H
