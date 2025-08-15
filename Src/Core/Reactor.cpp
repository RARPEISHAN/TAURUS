//--------------------------------------------------------------------
//	Reactor.cpp.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "Reactor.h"
#include "../Taurus.h"
#include "../Common/Basics.h"
#include "Demultiplexers/SelectDemultiplexer.h"
#include "NetAddress.h"
#include "EventSwitcher.h"
#include "Socket.h"
#include "../Common/TaurusSettings.h"
#include "Demultiplexers/NfxDmplexerFactory.h"
#include "Demultiplexers/IDemultiplexerFactory.h"
#include "NetFoundation.h"
#include <algorithm>

#ifdef __TAURUS_WIN_ENV
	// Windows.
#include "WindowsSupporting/WakeupDaemon.h"
#include "WindowsSupporting/WakeupSupporter.h"
#endif // __TAURUS_WIN_ENV

//--------------------------------------------------------------------
using namespace std::string_literals;

//--------------------------------------------------------------------
//	initialize static variables.
//--------------------------------------------------------------------
std::shared_ptr<aldebaran::core::win::WakeupDaemon>	aldebaran::core::Reactor::_wakeup_daemon;
std::mutex aldebaran::core::Reactor::_wakeup_access_mx;

//--------------------------------------------------------------------
aldebaran::core::Reactor::Reactor() : _thread_id(std::this_thread::get_id())
{
	/*auto dmplexer_factory = nfx_dmplexer_factory(TaurusSettings::instance()->demultiplexer_type());
	_demutiplexer = dmplexer_factory->create_demultiplexer(this_obj());

	create_wakeup_handle();*/
}

//--------------------------------------------------------------------
aldebaran::core::Reactor::~Reactor()
{
	//
}

void aldebaran::core::Reactor::init()
{
	auto dmplexer_factory = nfx_dmplexer_factory(TaurusSettings::instance()->demultiplexer_type());
	_demutiplexer = dmplexer_factory->create_demultiplexer(this_obj());
	create_wakeup_handle();
	_timerQueue = make_obj<TimerQueue>(this_obj());
	_timerQueue->init();
	_timerQueue->handle_alarm();
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::loop(void)
{
	if (!_demutiplexer)
		return;

	_is_looping = true;
	_is_quit = false;

	while (!_is_quit)
	{
		_active_event_switchers.clear();

		auto tp = _demutiplexer->wait(
			(std::chrono::milliseconds::max)(),
			_active_event_switchers);

		for (auto const& active_switcher : _active_event_switchers)
			active_switcher->handle_event();

		call_pending_functors();
	}

	// calls the logger...
	_is_looping = false;
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::quit()
{
	_is_quit = true;
	wakeup();
}

//--------------------------------------------------------------------
std::chrono::system_clock::time_point
aldebaran::core::Reactor::last_wait_time(void)
const
{
	return std::chrono::system_clock::time_point();
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::run_in_reactor(Functor const& func)
{
	if (is_in_reactor_thread())
		func();
	else
		queue_in_reactor(func);
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::queue_in_reactor(Functor const& func)
{
	if (!func)
		return;

	{
		std::lock_guard lck(_pending_func_access_mx);
		_pending_functors.emplace_back(func);
	}

	if (_calling_pending_functors || !is_in_reactor_thread())
		wakeup();
}

//--------------------------------------------------------------------
unsigned long long
aldebaran::core::Reactor::queue_size()
const
{
	std::lock_guard lck(const_cast<std::mutex&>(_pending_func_access_mx));

	return _pending_functors.size();
}

//--------------------------------------------------------------------
aldebaran::core::TimerToken
aldebaran::core::Reactor::run_at(
	std::chrono::system_clock::time_point time,
	TimerCallback const& cb)
{
	return _timerQueue->add_timer(std::shared_ptr<Timer>(new Timer(cb, time, 0ms, true)));
}

//--------------------------------------------------------------------
aldebaran::core::TimerToken
aldebaran::core::Reactor::run_after(
	std::chrono::duration<long long, std::milli> delay,
	TimerCallback const& cb)
{
	if (delay <= 5ms)
	{
		run_in_reactor(cb);
		return {};
	}

	return _timerQueue->add_timer(std::shared_ptr<Timer>(new Timer(cb, std::chrono::system_clock::now() + delay, 0ms, true)));
}

//--------------------------------------------------------------------
aldebaran::core::TimerToken
aldebaran::core::Reactor::run_every(
	std::chrono::duration<long long, std::milli> interval,
	TimerCallback const& cb)
{
	return _timerQueue->add_timer(std::shared_ptr<Timer>(new Timer(cb, std::chrono::system_clock::now(), interval, false)));
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::cancel(TimerToken const& timer_id)
{
	//
	_timerQueue->cancel(timer_id);
}

//--------------------------------------------------------------------
inline
void
aldebaran::core::Reactor::wakeup(void)
{
	//

	_wakeup_supporter->wakeup('a');
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::update_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (!switcher)
		return;

	// calls the logger.
	if (switcher->owner_reactor() != this_obj() ||
		std::this_thread::get_id() != _thread_id)
	{
		nfx_print<info::SeverityType::Error>(L"Reactor: update Event Switcher failed!"s);
		return;
	}

	_demutiplexer->update_event_switcher(switcher);
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::remove_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (!switcher)
		return;

	/*assert(switcher->ownerLoop() == this);
	assertInLoopThread();*/

	_demutiplexer->remove_event_switcher(switcher);
}

//--------------------------------------------------------------------
inline
bool
aldebaran::core::Reactor::has_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (switcher->owner_reactor() != this_obj())
		return false;

	return _demutiplexer->has_event_switcher(switcher);
}

//--------------------------------------------------------------------
inline
void
aldebaran::core::Reactor::create_wakeup_handle(void)
{
	// creates the global wakeup daemon.
	if (!_wakeup_daemon)
	{
		std::lock_guard lck(_wakeup_access_mx);
		if (!_wakeup_daemon)
		{
			auto listening_addr = make_obj<NetAddress>(static_cast<unsigned short>(3752u), true);
			//EventHandle eh = net::create_nonblocking_socket(listening_addr->family());
			_wakeup_daemon = make_obj<win::WakeupDaemon>(this_obj(), listening_addr);
			_wakeup_daemon->listen();
		}
	}

	auto connected_addr = make_obj<NetAddress>(static_cast<unsigned short>(3752u), true);
	//EventHandle conn_eh = net::create_nonblocking_socket(connected_addr->family());
	//auto localBindaddr = make_obj<NetAddress>(static_cast<unsigned short>(3751u), true);
	_wakeup_supporter = make_obj<win::WakeupSupporter>(this_obj(), connected_addr, nullptr);
	/*_wakeup_daemon->add_new_conn_callback(
		localBindaddr,
		std::bind(&Reactor::new_wakeup_connection,
		this,
		std::placeholders::_1,
		std::placeholders::_2));*/
	_wakeup_daemon->setNewConnectionCallback(std::bind(&Reactor::new_wakeup_connection,
		this,
		std::placeholders::_1,
		std::placeholders::_2));
	_wakeup_supporter->connect();
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::new_wakeup_connection(
	EventHandle event_handle,
	std::shared_ptr<core::NetAddress> peer_addr)
{
	_wakeup_eventSwitcher = make_obj<EventSwitcher>(this_obj(), event_handle);

	_wakeup_eventSwitcher->set_read_handler(
		std::bind(
			&Reactor::wakeup_read_handler,
			this,
			std::placeholders::_1));

	_wakeup_eventSwitcher->enable_reading();
}

//--------------------------------------------------------------------
//	wakeup signal arrived.
//--------------------------------------------------------------------
void
aldebaran::core::Reactor::wakeup_read_handler(std::chrono::system_clock::time_point time_stamp)
{
	// read id token from wakeup eventSwitcher.
	char buf[256] = { 0 };
	auto [ret, ret_type] = net::read(
		_wakeup_eventSwitcher->event_handle(),
		buf,
		sizeof buf);

	if (ret_type == IOTYPE::Error)
	{
		// errors.
		return;
	}
	else if (ret_type == IOTYPE::InProgress)
	{
		return;
	}
	else if (ret == 0ll)
	{
		// closed.
	}
	else if (ret > 1ll)
	{
		nfx_print<info::SeverityType::Error>(L"Reactor: multiple wakeup signal arrived!"s);
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::remove_wakeup_daemon_handler(void)
{
	if (_wakeup_daemon)
		_wakeup_daemon.reset();
}

//--------------------------------------------------------------------
void
aldebaran::core::Reactor::call_pending_functors(void)
{
	std::list<Functor> calling_functors;
	// atomic.
	_calling_pending_functors = true;

	{
		std::lock_guard lck(_pending_func_access_mx);
		calling_functors.swap(_pending_functors);
	}

	for (Functor const& functor : calling_functors)
		functor();

	// atomic.
	_calling_pending_functors = false;
}

//--------------------------------------------------------------------
inline
bool
aldebaran::core::Reactor::is_in_reactor_thread(void)
const
{
	// for multi-threading calling.
	return _thread_id == std::this_thread::get_id();
}

//--------------------------------------------------------------------