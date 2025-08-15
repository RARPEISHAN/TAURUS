//--------------------------------------------------------------------
//	SelectDemultiplexer.h.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _SELECT_DEMULTIPLEXER_H
#define _SELECT_DEMULTIPLEXER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#ifdef __TAURUS_WIN_ENV
#pragma comment(lib, "Ws2_32.lib")
#else

#endif // __TAURUS_WIN_ENV

#include "../Demultiplexer.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class SelectDemultiplexer : public Demultiplexer
	{
	public:
		SelectDemultiplexer(std::shared_ptr<Reactor> reactor);
		~SelectDemultiplexer();

		//	wait the I/O events.
		//	must be called in the reactor thread.
		virtual std::chrono::system_clock::time_point wait(
			std::chrono::milliseconds timeout,
			std::vector<std::shared_ptr<EventSwitcher>>& active_event_switchers) override;

		//	modifies the interested I/O events.
		//	must be called in the reactor thread.
		virtual void update_event_switcher(std::shared_ptr<EventSwitcher> event_switcher) override;
		virtual void remove_event_switcher(std::shared_ptr<EventSwitcher> event_switcher) override;

	protected:
		// used for select function.
		void fillout_active_event_switchers(
			long long event_cnt,
			std::vector<std::shared_ptr<EventSwitcher>>& active_event_switchers);
		bool exchange_handle(fd_set& fds, int idx1, int idx2);
		bool set_last_handle(SwitcherHandleType handle_type, EventHandle handle);

	protected:
		fd_set					_read_handles;
		fd_set					_write_handles;
		fd_set					_error_handles;

		fd_set					_read_handles_copy;
		fd_set					_write_handles_copy;
		fd_set					_error_handles_copy;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_SELECT_DEMULTIPLEXER_H
