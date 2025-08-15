//--------------------------------------------------------------------
//	WakeupSupporter.cpp.
//	08/03/2020.				by bubo.
//	08/15/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "WakeupSupporter.h"
#include "../EventSwitcher.h"
#include "../Socket.h"
#include "../NetAddress.h"
#include "../NetFoundation.h"

//--------------------------------------------------------------------
aldebaran::core::win::WakeupSupporter::WakeupSupporter(
	std::shared_ptr<Reactor> reactor,
	std::shared_ptr<NetAddress> connecting_addr,
	std::shared_ptr<NetAddress> local_binding_addr) : _reactor(reactor),
	_connecting_addr(connecting_addr)
{
	_conn_socket = make_obj<Socket>(net::create_nonblocking_socket(connecting_addr->family()));
	_conn_eventSwitcher = make_obj<EventSwitcher>(reactor, _conn_socket->event_handle());

	_conn_socket->set_no_delay(true);
	_conn_socket->set_reuse_addr(true);
	_conn_socket->set_keep_alive(true);
	if (local_binding_addr)
		_conn_socket->bind(local_binding_addr);
	_conn_eventSwitcher->set_read_handler(
		std::bind(&WakeupSupporter::read_handler, this, std::placeholders::_1));
	_conn_eventSwitcher->set_write_handler(
		std::bind(&WakeupSupporter::write_handler, this));
	_conn_eventSwitcher->set_write_handler(
		std::bind(&WakeupSupporter::error_handler, this));
}

//--------------------------------------------------------------------
aldebaran::core::win::WakeupSupporter::~WakeupSupporter()
{
	//
	net::close(_conn_socket->event_handle());
}

//--------------------------------------------------------------------
bool
aldebaran::core::win::WakeupSupporter::connect(void)
{
	auto ret_type = net::connect(
		_conn_socket->event_handle(),
		_connecting_addr->raw_addr());

	if (ret_type == IOTYPE::OK)
	{
		// connected.
	}
	else if (ret_type == IOTYPE::InProgress)
	{
		int a = 0;
		//_conn_eventSwitcher->enable_writing();
	}
	else
		return false;
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupSupporter::wakeup(char val)
{
	// sent val to _conn
	net::send(_conn_socket->event_handle(), &val, 1);
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupSupporter::read_handler(
	std::chrono::system_clock::time_point time_stamp)
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::win::WakeupSupporter::write_handler(void)
{
	// 1st: connected.
	_conn_eventSwitcher->disable_writing();
}

//--------------------------------------------------------------------
void aldebaran::core::win::WakeupSupporter::error_handler(void)
{
	//
}

//--------------------------------------------------------------------