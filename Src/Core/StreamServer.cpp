//--------------------------------------------------------------------
//	StreamServer.cpp.
//	07/08/2020.				by bubo.
//	08/02/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "StreamServer.h"
#include "NetAddress.h"
#include "NetFoundation.h"
#include "Connection.h"
#include "Acceptor.h"
#include "Reactor.h"
#include "../Taurus.h"

//--------------------------------------------------------------------
aldebaran::core::StreamServer::StreamServer(
	std::shared_ptr<Reactor> reactor,
	std::shared_ptr<NetAddress> listen_addr) : _reactor(reactor),
	_acceptor(make_obj<Acceptor>(reactor, listen_addr))
{
	auto _thread_set = threadPool2::getThreadsPoolInstance();
	set_threadsPool(_thread_set);
	_pbuf = make_obj<bufferManager>();
	//
}

//--------------------------------------------------------------------
aldebaran::core::StreamServer::~StreamServer()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::StreamServer::start(void)
{
	// will call listen later. connect the callback function.
	if (_acceptor)
	{
		std::function<void(EventHandle, std::shared_ptr<NetAddress>)> new_con
			= std::bind(&StreamServer::new_connection,
				this,
				std::placeholders::_1,
				std::placeholders::_2);
		_acceptor->set_new_connection_callback(new_con);
		_acceptor->listen();
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::StreamServer::set_connection_callback(ConnectionCallback const& cb)
{
	_connection_callback = cb;
}

//void aldebaran::core::StreamServer::set_read_callback(ReadCallback const& cb)
//{
//	_read_callback = cb;
//}
//
//void aldebaran::core::StreamServer::set_write_callback(WriteCallback const& cb)
//{
//	_write_callback = cb;
//}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void aldebaran::core::StreamServer::set_threadsPool(std::weak_ptr<threadPool2::thread_set> pThreadsPool)
{
	_pThreadsPool = pThreadsPool;
	_pbuf->setthreadContext(_pThreadsPool);
}

//--------------------------------------------------------------------
void
aldebaran::core::StreamServer::new_connection(
	EventHandle event_handle,
	std::shared_ptr<core::NetAddress> peer_addr)
{
	if (event_handle == INVALID_HANDLE || !peer_addr)
		return;

	std::shared_ptr<core::NetAddress> local_addr;

	if (peer_addr->family() == AF_INET)
	{
		auto opt = net::get_local_addr_ipv4(event_handle);
		if (!opt.has_value())
		{
			nfx_print<info::SeverityType::Error>(L"Stream Server: Getting local address failed!"s);
			return;
		}

		local_addr = make_obj<core::NetAddress>(opt.value());
	}
	else if (peer_addr->family() == AF_INET6)
	{
		auto opt = net::get_local_addr_ipv6(event_handle);
		if (!opt.has_value())
		{
			nfx_print<info::SeverityType::Error>(L"Stream Server: Getting local address failed!"s);
			return;
		}

		local_addr = make_obj<core::NetAddress>(opt.value());
	}
	else
	{
		nfx_print<info::SeverityType::Error>(L"Stream Server: Wrong Family Type!"s);
		return;
	}

	// make the connection instance.
	auto connection = make_obj<Connection>(
		_reactor.lock(),
		event_handle,
		local_addr,
		peer_addr);

	// sets the connection identifier from the memory address.
	connection->_id = reinterpret_cast<unsigned long long>(&(*connection));

	_connections[connection->_id] = connection;
	connection->set_connection_callback(_connection_callback);
	connection->set_data_callback(_data_callback);
	connection->set_close_callback(
		std::bind(&StreamServer::remove_connection,
			this,
			std::placeholders::_1));

	/*connection->set_threadsPool(_pThreadsPool);
	connection->createBuffer(64 * 1024, 1024 * 1024);*/

	connection->setBuffer(_pbuf);
	connection->createBuffer(64 * 1024, 1024 * 1024);

	_connectTasks.push_back(connection);
}

//--------------------------------------------------------------------
void
aldebaran::core::StreamServer::remove_connection(
	std::shared_ptr<Connection> connection)
{
	if (!connection)
		return;

	_connections.erase(connection->_id);
	if (auto reactor = _reactor.lock())
		reactor->run_in_reactor(
			std::bind(&Connection::connect_dropped, connection));
}

//--------------------------------------------------------------------