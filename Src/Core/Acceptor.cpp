//--------------------------------------------------------------------
//	Acceptor.cpp.
//	07/08/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "Acceptor.h"
#include "Socket.h"
#include "NetAddress.h"
#include "NetFoundation.h"
#include "EventSwitcher.h"

//--------------------------------------------------------------------
aldebaran::core::Acceptor::Acceptor(
	std::shared_ptr<Reactor> reactor,
	std::shared_ptr<NetAddress> listen_addr) : _reactor(reactor)
{
	// listening socket.
	_accept_socket = make_obj<Socket>(net::create_nonblocking_socket(listen_addr->family()));
	_accept_event_switcher = make_obj<EventSwitcher>(reactor, _accept_socket->event_handle());
	_accept_socket->set_reuse_addr(true);
	_accept_socket->bind(listen_addr);

	_accept_event_switcher->set_read_handler(
		std::bind(
			&Acceptor::read_handler,
			this,
			std::placeholders::_1));
}

//--------------------------------------------------------------------
aldebaran::core::Acceptor::~Acceptor()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::Acceptor::set_new_connection_callback(NewConnectionCallback const& cb)
{
	_new_connection_callback = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::Acceptor::listen(void)
{
	// reactor checks the current thread!
	_accept_socket->listen();
	_is_listenning = true;
	if (auto reactor = _reactor.lock())
	{
		reactor->run_after(0ms, [this]() {
			_accept_event_switcher->enable_reading();
			});
	}
}

//--------------------------------------------------------------------
bool
aldebaran::core::Acceptor::is_listenning(void)
const
{
	return _is_listenning;
}

//--------------------------------------------------------------------
void
aldebaran::core::Acceptor::read_handler(
	std::chrono::system_clock::time_point time_stamp)
{
	// reactor checks the current thread!
	auto [conn_handle, peer_addr] = _accept_socket->accept();

	if (conn_handle != INVALID_HANDLE)
		if (_new_connection_callback)
			_new_connection_callback(conn_handle, peer_addr);
		else
			net::close(conn_handle);
}

//--------------------------------------------------------------------