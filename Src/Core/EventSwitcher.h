//--------------------------------------------------------------------
//	EventSwitcher.h.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _EVENT_SWITCHER_H
#define _EVENT_SWITCHER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "../Common/Basics.h"
#include "../Common/Callbacks.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	headers.
	//--------------------------------------------------------------------
	class Reactor;

	//--------------------------------------------------------------------
	//	the selectable I/O eventSwitcher class.
	//--------------------------------------------------------------------
	class EventSwitcher : public B_Object_NonCopyable<EventSwitcher>
	{
	public:
		EventSwitcher(std::shared_ptr<Reactor> reactor, EventHandle handle);
		~EventSwitcher();

		void handle_event(void);
		void set_read_handler(ReadCallback const& cb);
		void set_write_handler(CallbackFunc const& cb);
		void set_error_handler(CallbackFunc const& cb);
		void set_close_handler(CallbackFunc const& cb);

		EventHandle event_handle(void) const;
		void reset_handle(EventHandle handle);
		int event_type(void) const;
		int active_event_type(void) const;
		void set_active_event_type(int type);
		bool is_none_event(void) const;
		bool is_writing(void) const;
		bool is_reading(void) const;

		// event type toggles.
		void enable_reading(void);
		void disable_reading(void);
		void enable_writing(void);
		void enable_error(void);
		void disable_error(void);
		void disable_writing(void);
		void disable_all(void);

		// used by the demultiplexer.
		long long index(void) const;
		void set_index(long long idx);
		long long index(SwitcherHandleType handle_type) const;
		void set_index(SwitcherHandleType handle_type, long long idx);

		void remove();
		std::shared_ptr<Reactor> owner_reactor(void) const;

	private:
		void update(void);

	private:
		std::weak_ptr<Reactor>	_reactor;
		// SOCKET / File Descriptor.
		EventHandle				_handle{ INVALID_HANDLE };

		// event handlers.
		ReadCallback			_read_handler;
		CallbackFunc			_write_handler;
		CallbackFunc			_error_handler;
		CallbackFunc			_close_handler;

		// used by the demultiplexer.
		long long				_idx{ -1ll };
		long long				_rd_idx{ -1ll };
		long long				_wd_idx{ -1ll };
		long long				_err_idx{ -1ll };

		// bit pattern.
		int						_active_event_type{ EventType::None };

		// bit pattern.
		int						_focused_event_type{ EventType::None };
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_EVENT_SWITCHER_H
