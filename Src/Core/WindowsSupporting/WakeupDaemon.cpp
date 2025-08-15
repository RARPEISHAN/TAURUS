//--------------------------------------------------------------------
//	WakeupDaemon.cpp.
//	08/03/2020.				by bubo.
//	08/04/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "WakeupDaemon.h"
#include "../EventSwitcher.h"
#include "../Socket.h"
#include "../NetAddress.h"
#include "../NetFoundation.h"
#include "../Reactor.h"
#include <algorithm>
#include <execution>

//--------------------------------------------------------------------
aldebaran::core::win::WakeupDaemon::WakeupDaemon(
	std::shared_ptr<Reactor> reactor,
	std::shared_ptr<NetAddress> listening_addr) : _reactor(reactor)
{
	// listening socket.
	_listening_socket = std::shared_ptr<Socket>(
		new Socket{ net::create_nonblocking_socket(listening_addr->family()) });

	_listening_eventSwitcher = make_obj<EventSwitcher>(reactor, _listening_socket->event_handle());  //std::shared_ptr<EventSwitcher>(new EventSwitcher{ reactor, _listening_socket->event_handle() });
	_listening_socket->set_reuse_addr(true);

	_listening_socket->bind(listening_addr);
	_listening_eventSwitcher->set_read_handler(
		std::bind(
			&WakeupDaemon::read_handler,
			this,
			std::placeholders::_1));
}

//--------------------------------------------------------------------
aldebaran::core::win::WakeupDaemon::~WakeupDaemon()
{
	if (_listening_eventSwitcher)
	{
		_listening_eventSwitcher->disable_all();
		if (auto reactor = _reactor.lock())
			reactor->remove_event_switcher(_listening_eventSwitcher);
		net::close(_listening_eventSwitcher->event_handle());
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupDaemon::listen(void)
{
	// reactor checks the current thread!
	_is_listenning = true;
	_listening_socket->listen();
	_listening_eventSwitcher->enable_reading();
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupDaemon::add_new_conn_callback(
	std::shared_ptr<NetAddress> conn_addr,
	NewConnectionCallback const& cb)
{
	std::lock_guard lck(_new_cb_callbacks_mx);
	_new_connection_callbacks[conn_addr] = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupDaemon::read_handler(
	std::chrono::system_clock::time_point time_stamp)
{
	// reactor checks the current thread!
	auto [conn_handle, peer_addr] = _listening_socket->accept();

	// maybe reseted (closed) by client.
	if (conn_handle != INVALID_HANDLE)
	{
		//if (auto cb_opt = find_connection_callback(peer_addr))
		//{
		//	cb_opt.value()(conn_handle, peer_addr);
		//}
		if (_connection_callback)
			_connection_callback(conn_handle, peer_addr);
		else
			net::close(conn_handle);
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupDaemon::write_handler(void)
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupDaemon::error_handler(void)
{
	//
}

//--------------------------------------------------------------------
inline
std::optional<aldebaran::NewConnectionCallback>
aldebaran::core::win::WakeupDaemon::find_connection_callback(std::shared_ptr<NetAddress> addr)
{
	if (!addr)
		return std::nullopt;

	std::lock_guard lck(_new_cb_callbacks_mx);

	auto it = std::find_if(
		std::execution::par,
		_new_connection_callbacks.cbegin(),
		_new_connection_callbacks.cend(),
		[addr](std::pair<std::shared_ptr<NetAddress>, NewConnectionCallback> const& cb_pair) -> bool
		{
			return *cb_pair.first == *addr;
		});

	if (it != _new_connection_callbacks.cend())
		return it->second;

	return std::nullopt;
}

void aldebaran::core::win::WakeupDaemon::setNewConnectionCallback(NewConnectionCallback const& cb)
{
	_connection_callback = cb;
}

//--------------------------------------------------------------------