//--------------------------------------------------------------------
//	Reactor.h.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _REACTOR_H
#define _REACTOR_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <chrono>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include "../Common/Callbacks.h"
#include "../Common/B_Object.hpp"
#include "TimerToken.h"
#include "TimerQueue.h"

//--------------------------------------------------------------------
namespace aldebaran::core::win
{
	//--------------------------------------------------------------------
	//	incomplete types.
	//--------------------------------------------------------------------
	class WakeupDaemon;
	class WakeupSupporter;

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete types.
	//--------------------------------------------------------------------
	class Demultiplexer;
	class EventSwitcher;
	class TimerQueue;
	class TimerToken;

	//--------------------------------------------------------------------
	class Reactor : public B_Object_NonCopyable<Reactor>
	{
		//--------------------------------------------------------------------
	public:
		Reactor();
		Reactor(Reactor const&) = delete;
		Reactor(Reactor&&) = delete;
		~Reactor();
		void init();

		//--------------------------------------------------------------------
		// loops forever.
		// must be called in the same thread as creation of the object.
		//--------------------------------------------------------------------
		void loop(void);

		// quits loop.
		void quit();

		// the time when wait returns, usually means data arrival.
		std::chrono::system_clock::time_point last_wait_time(void) const;

		//--------------------------------------------------------------------
		// runs callback immediately in the loop thread.
		// it wakes up the loop, and run the cb.
		// if in the same loop thread, cb is run within the function.
		// safe to call from other threads.
		//--------------------------------------------------------------------
		void run_in_reactor(Functor const& func);

		//--------------------------------------------------------------------
		// queues callback in the loop thread.
		// runs after finish pooling.
		// safe to call from other threads.
		//--------------------------------------------------------------------
		void queue_in_reactor(Functor const& func);

		//--------------------------------------------------------------------
		unsigned long long queue_size() const;

		//--------------------------------------------------------------------
		// *** TIMERs ***
		//
		//--------------------------------------------------------------------
		// runs callback at 'time'.
		// safe to call from other threads.
		//--------------------------------------------------------------------
		TimerToken run_at(
			std::chrono::system_clock::time_point time,
			TimerCallback const& cb);

		//--------------------------------------------------------------------
		// runs callback after delay seconds.
		// safe to call from other threads.
		//--------------------------------------------------------------------
		TimerToken run_after(
			std::chrono::duration<long long, std::milli> delay,
			TimerCallback const& cb);

		//--------------------------------------------------------------------
		// runs callback every interval seconds.
		// safe to call from other threads.
		//--------------------------------------------------------------------
		TimerToken run_every(
			std::chrono::duration<long long, std::milli> interval,
			TimerCallback const& cb);

		//--------------------------------------------------------------------
		// cancels the timer.
		// safe to call from other threads.
		//--------------------------------------------------------------------
		void cancel(TimerToken const& timer_id);

		//--------------------------------------------------------------------
		void wakeup(void);
	private:
		// used for internal only.

		void update_event_switcher(std::shared_ptr<EventSwitcher> switcher);
		void remove_event_switcher(std::shared_ptr<EventSwitcher> switcher);
		bool has_event_switcher(std::shared_ptr<EventSwitcher> switcher);

		// used for internal only.
		void create_wakeup_handle(void);
		void new_wakeup_connection(
			EventHandle event_handle,
			std::shared_ptr<core::NetAddress> addr);
		void wakeup_read_handler(std::chrono::system_clock::time_point time_stamp);
		void remove_wakeup_daemon_handler(void);
		void call_pending_functors(void);
		bool is_in_reactor_thread(void) const;
		static void init_wakeup_daemon(void);

	private:
		std::vector<std::shared_ptr<EventSwitcher>>	_active_event_switchers;
		std::shared_ptr<Demultiplexer>				_demutiplexer;
		std::atomic<bool>							_is_looping = false;
		std::atomic<bool>							_is_quit = false;
		std::thread::id								_thread_id;

		std::shared_ptr<TimerQueue>					_timer_queue;

		// below, used for Linux / BSD Unix
		// int _wakeup_handle;
		// unlike in TimerQueue, which is an internal class,
		// we don't expose EventSwitcher to client.
		std::atomic_bool							_calling_pending_functors = false;
		std::shared_ptr<EventSwitcher>				_wakeup_eventSwitcher;
		std::list<Functor>							_pending_functors;
		std::mutex									_pending_func_access_mx;

		// used for windows only.
		static std::mutex							_wakeup_access_mx;
		static std::shared_ptr<win::WakeupDaemon>	_wakeup_daemon;
		std::shared_ptr<win::WakeupSupporter>		_wakeup_supporter;

		std::shared_ptr<TimerQueue>                 _timerQueue;

		//std::any _context;

		//--------------------------------------------------------------------
		friend class EventSwitcher;
		friend class Connection;
		friend class win::WakeupDaemon;
		friend class TimerQueue;
		friend class Connector;

		//--------------------------------------------------------------------
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_REACTOR_H
