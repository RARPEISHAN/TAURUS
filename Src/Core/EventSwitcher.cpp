//--------------------------------------------------------------------
//	EventSwitcher.cpp.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "EventSwitcher.h"
#include "Reactor.h"

//--------------------------------------------------------------------
aldebaran::core::EventSwitcher::EventSwitcher(
	std::shared_ptr<Reactor> reactor,
	EventHandle handle) :_reactor(reactor), _handle(handle)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::EventSwitcher::~EventSwitcher()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::handle_event(void)
{
	// tests all event types.
	if (_active_event_type & EventType::Read)
		if (_read_handler)
		{
			_read_handler(std::chrono::system_clock::now());
			_active_event_type &= (~EventType::Read);
		}

	if (_active_event_type & EventType::Write)
		if (_write_handler)
		{
			_write_handler();
			_active_event_type &= (~EventType::Write);
		}

	if (_active_event_type & EventType::Error)
		if (_error_handler)
		{
			_error_handler();
			_active_event_type &= (~EventType::Error);
		}

	if (_active_event_type & EventType::Close)
		if (_close_handler)
		{
			_close_handler();
			_active_event_type &= (~EventType::Close);
		}
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_read_handler(ReadCallback const& cb)
{
	_read_handler = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_write_handler(CallbackFunc const& cb)
{
	_write_handler = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_error_handler(CallbackFunc const& cb)
{
	_error_handler = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_close_handler(CallbackFunc const& cb)
{
	_close_handler = cb;
}

//--------------------------------------------------------------------
aldebaran::EventHandle
aldebaran::core::EventSwitcher::event_handle(void)
const
{
	return _handle;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::reset_handle(EventHandle handle)
{
	_handle = handle;
}

//--------------------------------------------------------------------
int
aldebaran::core::EventSwitcher::event_type(void)
const
{
	return _focused_event_type;
}

//--------------------------------------------------------------------
int
aldebaran::core::EventSwitcher::active_event_type(void)
const
{
	return _active_event_type;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_active_event_type(int type)
{
	_active_event_type |= type;//¸ÄÐ´
}

//--------------------------------------------------------------------
bool
aldebaran::core::EventSwitcher::is_none_event(void)
const
{
	return _focused_event_type == EventType::None;
}

//--------------------------------------------------------------------
bool
aldebaran::core::EventSwitcher::is_writing(void)
const
{
	return _focused_event_type & EventType::Write;
}

//--------------------------------------------------------------------
bool
aldebaran::core::EventSwitcher::is_reading(void)
const
{
	return _focused_event_type & EventType::Read;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::enable_reading(void)
{
	_focused_event_type |= EventType::Read;

	update();
}

void aldebaran::core::EventSwitcher::disable_reading(void)
{
	_focused_event_type &= ~EventType::Read;

	update();
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::enable_writing(void)
{
	_focused_event_type |= EventType::Write;

	update();
}

void aldebaran::core::EventSwitcher::enable_error(void)
{
	_focused_event_type |= EventType::Error;

	update();
}

void aldebaran::core::EventSwitcher::disable_error(void)
{
	_focused_event_type &= ~EventType::Error;

	update();
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::disable_writing(void)
{
	_focused_event_type &= ~EventType::Write;

	update();
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::disable_all(void)
{
	_focused_event_type = EventType::None;

	update();
}

//--------------------------------------------------------------------
long long
aldebaran::core::EventSwitcher::index(void)
const
{
	return _idx;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_index(long long idx)
{
	_idx = idx;
}

//--------------------------------------------------------------------
long long
aldebaran::core::EventSwitcher::index(SwitcherHandleType handle_type)
const
{
	if (handle_type == SwitcherHandleType::Read)
		return _rd_idx;
	else if (handle_type == SwitcherHandleType::Write)
		return _wd_idx;
	else if (handle_type == SwitcherHandleType::Error)
		return _err_idx;

	return -1ll;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::set_index(
	SwitcherHandleType handle_type,
	long long idx)
{
	if (handle_type == SwitcherHandleType::Read)
		_rd_idx = idx;
	else if (handle_type == SwitcherHandleType::Write)
		_wd_idx = idx;
	else if (handle_type == SwitcherHandleType::Error)
		_err_idx = idx;
}

//--------------------------------------------------------------------
void
aldebaran::core::EventSwitcher::remove()
{
	if (auto reactor = _reactor.lock())
		reactor->remove_event_switcher(this_obj());
}

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::core::Reactor>
aldebaran::core::EventSwitcher::owner_reactor(void)
const
{
	return _reactor.lock();
}

//--------------------------------------------------------------------
inline
void
aldebaran::core::EventSwitcher::update(void)
{
	if (auto reactor = _reactor.lock())
		reactor->update_event_switcher(this_obj());
}

//--------------------------------------------------------------------