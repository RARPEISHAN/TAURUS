//--------------------------------------------------------------------
//	SelectDemultiplexer.cpp.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include <algorithm>
#include <chrono>
#include <execution>
#include "SelectDemultiplexer.h"
#include "../EventSwitcher.h"

//--------------------------------------------------------------------
using namespace std::chrono_literals;

//--------------------------------------------------------------------
aldebaran::core::SelectDemultiplexer::SelectDemultiplexer(
	std::shared_ptr<Reactor> reactor) : Demultiplexer(reactor)
{
	FD_ZERO(&_read_handles);
	FD_ZERO(&_write_handles);
	FD_ZERO(&_error_handles);
	FD_ZERO(&_read_handles_copy);
	FD_ZERO(&_write_handles_copy);
	FD_ZERO(&_error_handles_copy);
}

//--------------------------------------------------------------------
aldebaran::core::SelectDemultiplexer::~SelectDemultiplexer()
{
	//
}

//--------------------------------------------------------------------
std::chrono::system_clock::time_point
aldebaran::core::SelectDemultiplexer::wait(
	std::chrono::milliseconds timeout,
	std::vector<std::shared_ptr<EventSwitcher>>& active_event_switchers)
{
	// time calculation.
	struct timeval* ptmval{ nullptr };
	auto seconds_tmp = std::chrono::duration_cast<std::chrono::seconds>(timeout);
	auto micro_sec_tmp = std::chrono::duration_cast<std::chrono::microseconds>(timeout - seconds_tmp);
	struct timeval tmval { seconds_tmp.count(), micro_sec_tmp.count() };

	if (timeout < (std::chrono::milliseconds::max)())
		ptmval = &tmval;
	_read_handles_copy = _read_handles;
	_write_handles_copy = _write_handles;
	_error_handles_copy = _error_handles;
	// polls events.
	int ret = ::select(
		0,
		&_read_handles_copy,
		&_write_handles_copy,
		&_error_handles_copy,
		ptmval);

	if (ret != SOCKET_ERROR)
		fillout_active_event_switchers(
			ret,
			active_event_switchers);
	//else
	//{
	//	int i = WSAGetLastError();
	//	int a = 0;
	//}

	return std::chrono::system_clock::now();
}

//--------------------------------------------------------------------
void
aldebaran::core::SelectDemultiplexer::update_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (!switcher)
		return;

	// adds the new switcher.
	auto it = _event_switchers.find(switcher->event_handle());
	if (it == _event_switchers.end())
		_event_switchers[switcher->event_handle()] = switcher;

	//--------------------------------------------------------------------
	// updates 3 handle sets.
	//--------------------------------------------------------------------
	// adds a reading handle.
	if (switcher->event_type() & EventType::Read &&
		switcher->index(SwitcherHandleType::Read) < 0ll)
	{
		_read_handles.fd_array[_read_handles.fd_count++] = switcher->event_handle();

		switcher->set_index(SwitcherHandleType::Read, _read_handles.fd_count - 1);
	}
	// removes a reading handle.
	else if (!(switcher->event_type() & EventType::Read) &&
		switcher->index(SwitcherHandleType::Read) >= 0ll)
	{
		// exchanges the current handle with the last one.
		exchange_handle(
			_read_handles,
			switcher->index(SwitcherHandleType::Read),
			_read_handles.fd_count);;

		// finds and modifies the last switcher.
		if (switcher->index(SwitcherHandleType::Read) < _read_handles.fd_count - 1)
			set_last_handle(SwitcherHandleType::Read, switcher->index(SwitcherHandleType::Read));

		switcher->set_index(SwitcherHandleType::Read, -1ll);
		--_read_handles.fd_count;
	}

	// adds a writing handle.
	if (switcher->event_type() & EventType::Write &&
		switcher->index(SwitcherHandleType::Write) < 0ll)
	{
		_write_handles.fd_array[_write_handles.fd_count++] = switcher->event_handle();
		switcher->set_index(SwitcherHandleType::Write, _write_handles.fd_count - 1);
	}
	// removes a writing handle.
	else if (!(switcher->event_type() & EventType::Write) &&
		switcher->index(SwitcherHandleType::Write) >= 0ll)
	{
		// exchanges the current handle with the last one.
		exchange_handle(
			_write_handles,
			switcher->index(SwitcherHandleType::Write),
			_write_handles.fd_count);

		// finds and modifies the last switcher.
		if (switcher->index(SwitcherHandleType::Write) < _write_handles.fd_count - 1)
			set_last_handle(SwitcherHandleType::Write, switcher->index(SwitcherHandleType::Write));

		switcher->set_index(SwitcherHandleType::Write, -1ll);
		--_write_handles.fd_count;
	}

	// adds an error handle.
	if (switcher->event_type() & EventType::Error &&
		switcher->index(SwitcherHandleType::Error) < 0ll)
	{
		_error_handles.fd_array[_error_handles.fd_count++] = switcher->event_handle();
		switcher->set_index(SwitcherHandleType::Error, _error_handles.fd_count - 1);
	}
	// removes an error handle.
	else if (!(switcher->event_type() & EventType::Error) &&
		switcher->index(SwitcherHandleType::Error) >= 0ll)
	{
		// exchanges the current handle with the last one.
		exchange_handle(
			_error_handles,
			switcher->index(SwitcherHandleType::Error),
			_error_handles.fd_count);

		// finds and modifies the last switcher.
		if (switcher->index(SwitcherHandleType::Error) < _error_handles.fd_count - 1)
			set_last_handle(SwitcherHandleType::Error, switcher->index(SwitcherHandleType::Error));

		switcher->set_index(SwitcherHandleType::Error, -1ll);
		--_error_handles.fd_count;
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::SelectDemultiplexer::remove_event_switcher(std::shared_ptr<EventSwitcher> switcher)
{
	if (!switcher)
		return;
}

//--------------------------------------------------------------------
void
aldebaran::core::SelectDemultiplexer::fillout_active_event_switchers(
	long long event_cnt,
	std::vector<std::shared_ptr<EventSwitcher>>& active_switchers)
{
	std::map<EventHandle, std::shared_ptr<EventSwitcher>> switcher_tmp;

	// checks the reading fd set.
	for (unsigned i = 0u; i < _read_handles_copy.fd_count && event_cnt > 0ll; ++i)
	{
		if (FD_ISSET(_read_handles_copy.fd_array[i], /*_read_handles.fd_array*/ &_read_handles_copy))
		{
			--event_cnt;

			std::map<EventHandle, std::shared_ptr<EventSwitcher>>::const_iterator it =
				_event_switchers.find(_read_handles_copy.fd_array[i]);
			if (it == _event_switchers.cend())
				continue;

			// enables the read flag.
			it->second->set_active_event_type(it->second->event_type() & EventType::Read);
			switcher_tmp[_read_handles_copy.fd_array[i]] = it->second;
		}
	}

	// checks the writing fd set.
	for (unsigned i = 0u; i < _write_handles_copy.fd_count && event_cnt > 0ll; ++i)
	{
		if (FD_ISSET(_write_handles_copy.fd_array[i],/* _write_handles.fd_array*/&_write_handles_copy))
		{
			--event_cnt;

			std::map<EventHandle, std::shared_ptr<EventSwitcher>>::const_iterator it =
				_event_switchers.find(_write_handles_copy.fd_array[i]);
			if (it == _event_switchers.cend())
				continue;

			// enables the write flag.
			auto ch = switcher_tmp.find(_write_handles_copy.fd_array[i]);
			if (ch == switcher_tmp.cend())
				switcher_tmp[_write_handles_copy.fd_array[i]] = /*ch*/it->second;//¸ÄÐ´

			it->second->set_active_event_type(it->second->event_type() & EventType::Write);
		}
	}

	// checks the error fd set.
	for (unsigned i = 0u; i < _error_handles_copy.fd_count && event_cnt > 0ll; ++i)
	{
		if (FD_ISSET(_error_handles_copy.fd_array[i],/* _error_handles.fd_array*/&_error_handles_copy))
		{
			--event_cnt;

			std::map<EventHandle, std::shared_ptr<EventSwitcher>>::const_iterator it =
				_event_switchers.find(_error_handles_copy.fd_array[i]);
			if (it == _event_switchers.cend())
				continue;

			// enables the error flag.
			auto ch = switcher_tmp.find(_error_handles_copy.fd_array[i]);
			if (ch == switcher_tmp.cend())
				switcher_tmp[_error_handles_copy.fd_array[i]] = it->second;

			it->second->set_active_event_type(it->second->event_type() & EventType::Error);
		}
	}

	if (active_switchers.size() < switcher_tmp.size())
		active_switchers.resize(switcher_tmp.size());

	std::transform(
		std::execution::par,
		switcher_tmp.cbegin(),
		switcher_tmp.cend(),
		active_switchers.begin(),
		[](std::pair<EventHandle, std::shared_ptr<EventSwitcher>> const& ch_pr) ->
		std::shared_ptr<EventSwitcher>
		{
			return ch_pr.second;
		});
}

//--------------------------------------------------------------------
inline
bool
aldebaran::core::SelectDemultiplexer::exchange_handle(
	fd_set& fds,
	int idx1,
	int idx2)
{
	if (idx1 == idx2)
		return false;
	if (idx1 < 0 || idx2 < 0 || idx1 >= fds.fd_count || idx2 >= fds.fd_count)
		return false;

	// exchange indexes.
	auto handle = fds.fd_array[idx1];
	fds.fd_array[idx1] = fds.fd_array[idx2];
	fds.fd_array[idx1] = handle;

	return true;
}

//--------------------------------------------------------------------
inline
bool
aldebaran::core::SelectDemultiplexer::set_last_handle(
	SwitcherHandleType handle_type,
	EventHandle handle)
{
	long long last_idx = -1ll;
	if (handle_type == SwitcherHandleType::Read)
		last_idx = _read_handles.fd_count - 1;
	else if (handle_type == SwitcherHandleType::Write)
		last_idx = _write_handles.fd_count - 1;
	else if (handle_type == SwitcherHandleType::Error)
		last_idx = _error_handles.fd_count - 1;

	if (last_idx < 0ll)
		return false;

	auto it = _event_switchers.find(last_idx);
	if (it == _event_switchers.end())
		return false;

	it->second->set_index(
		handle_type,
		handle);

	return true;
}

//--------------------------------------------------------------------