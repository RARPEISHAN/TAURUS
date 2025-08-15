//--------------------------------------------------------------------
//	TimerQueue.cpp.
//	07/08/2020.				by bubo.
//	07/14/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "TimerQueue.h"
#include <algorithm>
#include <execution>

//--------------------------------------------------------------------
aldebaran::core::TimerQueue::TimerQueue(std::shared_ptr<Reactor> reactor) :
	_reactor(reactor)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::TimerQueue::~TimerQueue()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::TimerQueue::handle_alarm(void)
{
	//
	std::thread t(std::bind(&TimerQueue::run_alarm, this));
	t.detach();
}

bool aldebaran::core::TimerQueue::init(void)
{
	_alarm = std::shared_ptr<alarm>(new alarm(this_obj()));
	if (_alarm)
		return true;
	return false;
}

//--------------------------------------------------------------------

aldebaran::core::TimerToken
aldebaran::core::TimerQueue::add_timer(std::shared_ptr<Timer> timer)
{
	std::lock_guard lck(_timersMx);
	if (timer)
		_timers.insert({ (time_point)(*timer),timer });

	_alarmCv.notify_one();

	if (((time_point)(*timer) - std::chrono::system_clock::now()) <= 5ms)
		_alarm->interrupt();

	return TimerToken(timer, reinterpret_cast<long long>(timer.get()));
}

//--------------------------------------------------------------------
void
aldebaran::core::TimerQueue::cancel(TimerToken const& id)
{
	//
	auto timer = id.get();
	if (!timer.expired())
		timer.lock()->close();
}

//--------------------------------------------------------------------
std::vector<std::shared_ptr<aldebaran::core::Timer>>
aldebaran::core::TimerQueue::expired_timers(std::chrono::system_clock::time_point now)
{
	std::lock_guard lck(_timersMx);
	std::vector<std::shared_ptr<Timer>> expired_timers_tmp(_timers.size());
	std::multimap<std::chrono::system_clock::time_point, std::shared_ptr<Timer>>::const_iterator it =
		_timers.upper_bound(now);

	std::transform(
		std::execution::par,
		_timers.cbegin(),
		it,
		/*std::back_inserter(expired_timers_tmp),*/
		expired_timers_tmp.begin(),
		[](std::pair<std::chrono::system_clock::time_point,
			std::shared_ptr<Timer>> const& ch_pr) -> std::shared_ptr<Timer>
		{
			return ch_pr.second;
		});

	_timers.erase(_timers.begin(), it);

	return expired_timers_tmp;
}

//--------------------------------------------------------------------
int aldebaran::core::TimerQueue::run_alarm(void)
{
	while (!_quit)
	{
		if (_timers.empty())
		{
			std::unique_lock lck(_alarmMx);
			_alarmCv.wait(lck);
		}

		if (_timers.empty())
			continue;

		auto first = _timers.begin();
		auto dur = time_form_now(first->first);
		auto timers = _alarm->wait(dur);
		if (timers)
		{
			for (auto i : timers.value())
			{
				if (i && !i->expire())
				{
					_reactor.lock()->run_in_reactor(i->operator std::function<void()>());
					if (i->update())
						add_timer(i);
				}
			}
		}
	}
	return 0;
}

void aldebaran::core::TimerQueue::quit(void)
{
	_quit = true;
	_alarm->interrupt();
}

//--------------------------------------------------------------------
inline
std::chrono::milliseconds
aldebaran::core::TimerQueue::time_form_now(
	std::chrono::time_point<std::chrono::system_clock> when)
{
	const std::chrono::time_point<std::chrono::system_clock> now =
		std::chrono::system_clock::now();

	auto dur = (when - now) / 1ms;

	return std::chrono::milliseconds(dur);
}

//--------------------------------------------------------------------