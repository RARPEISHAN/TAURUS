//--------------------------------------------------------------------
//	Demultiplexer.h.
//	07/08/2020.				created.
//	07/15/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _DEMULTIPLEXER_H
#define _DEMULTIPLEXER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "../Common/Basics.h"
#include <vector>
#include <chrono>
#include <map>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete classes.
	//--------------------------------------------------------------------
	class Reactor;
	class EventSwitcher;

	//--------------------------------------------------------------------
	//	the life time of IDemultiplexer equals to the own reactor.
	//--------------------------------------------------------------------
	class Demultiplexer : public B_Object_NonCopyable<Demultiplexer>
	{
	public:
		Demultiplexer(std::shared_ptr<Reactor> reactor);
		virtual ~Demultiplexer();

		//	wait the I/O events.
		//	must be called in the reactor thread.
		virtual std::chrono::system_clock::time_point wait(
			std::chrono::milliseconds timeout,
			std::vector<std::shared_ptr<EventSwitcher>>& active_event_switchers) = 0;

		//	modifies the interested I/O events.
		//	must be called in the reactor thread.
		virtual void update_event_switcher(std::shared_ptr<EventSwitcher> event_switcher) = 0;

		//	removes the switcher, when it destructs.
		//	must be called in the loop thread.
		virtual void remove_event_switcher(std::shared_ptr<EventSwitcher> event_switcher) = 0;
		virtual bool has_event_switcher(std::shared_ptr<EventSwitcher> event_switcher);

	protected:
		std::map<EventHandle, std::shared_ptr<EventSwitcher>>	_event_switchers;
		std::weak_ptr<Reactor>									_reactor;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_DEMULTIPLEXER_H
