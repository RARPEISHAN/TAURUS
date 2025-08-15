//--------------------------------------------------------------------
//	Timer.cpp.
//	07/08/2020.				by bubo.
//	07/08/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "Timer.h"

//--------------------------------------------------------------------
aldebaran::core::Timer::Timer(
	TimerCallback const& cb,
	std::chrono::system_clock::time_point when,
	std::chrono::duration<long long, std::milli> interval,
	bool disposable)
	:_timerCallback(cb), _when(when), _interval(interval), _disposable(disposable)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::Timer::~Timer()
{
	//
}

aldebaran::core::Timer::operator std::chrono::system_clock::time_point()
{
	return _when;
}

bool aldebaran::core::Timer::update(void)
{
	if (_disposable)
	{
		close();
		return false;
	}
	_when += _interval;
	return true;
}

void aldebaran::core::Timer::close(void)
{
	_expired = true;
}

bool aldebaran::core::Timer::expire(void)
{
	return _expired;
}

bool aldebaran::core::Timer::isDisposable(void)
{
	return _disposable;
}

aldebaran::core::Timer::operator std::function<void(void)>()
{
	return _timerCallback;
}

//--------------------------------------------------------------------