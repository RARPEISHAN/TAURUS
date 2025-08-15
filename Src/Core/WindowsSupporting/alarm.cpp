#include "alarm.h"

aldebaran::core::alarm::alarm(std::shared_ptr<TimerQueue> timerQueue)
	:_timers(timerQueue)
{
}

std::optional<std::vector<std::shared_ptr<aldebaran::core::Timer>>>
aldebaran::core::alarm::wait(std::chrono::milliseconds waitTime)
{
	if (waitTime <= 5ms)
	{
		if (auto timers = _timers.lock())
			return { timers->expired_timers(std::chrono::system_clock::now()) };
		else
			return std::nullopt;
	}

	std::unique_lock lck(_alarmMx);
	auto status = _alarmCv.wait_for(lck, waitTime);
	switch (status)
	{
	case std::cv_status::no_timeout:
		if (auto timers = _timers.lock())
			return { timers->expired_timers(std::chrono::system_clock::now()) };
		break;
	case std::cv_status::timeout:

		if (auto timers = _timers.lock())
			return { timers->expired_timers(std::chrono::system_clock::now()) };
		break;
	default:
		return std::nullopt;
	}
}

void aldebaran::core::alarm::interrupt(void)
{
	_alarmCv.notify_one();
}