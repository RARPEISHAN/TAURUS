//--------------------------------------------------------------------
//	Demultiplexer.cpp.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "Demultiplexer.h"
#include "EventSwitcher.h"

//--------------------------------------------------------------------
aldebaran::core::Demultiplexer::Demultiplexer(std::shared_ptr<Reactor> reactor) :
	_reactor(reactor)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::Demultiplexer::~Demultiplexer()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::Demultiplexer::update_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (!switcher)
		return;
}

//--------------------------------------------------------------------
bool
aldebaran::core::Demultiplexer::has_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (!switcher)
		return false;

	auto it = _event_switchers.find(switcher->event_handle());
	return it != _event_switchers.end();
}

//--------------------------------------------------------------------