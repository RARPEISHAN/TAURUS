//--------------------------------------------------------------------
//	StreamClient.cpp.
//	07/31/2020.				by bubo.
//	07/31/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "StreamClient.h"

using Task = typename aldebaran::core::StreamClient::Task;
//--------------------------------------------------------------------

aldebaran::core::StreamClient::StreamClient(std::shared_ptr<Reactor> reactor,
	std::shared_ptr<NetAddress> peer_addr,
	std::shared_ptr<NetAddress> local_addr)
	:_reactor(reactor),
	_peer_addr(peer_addr),
	_local_addr(local_addr)
{
	_socket = make_obj<Socket>(net::create_nonblocking_socket(peer_addr->family()));
	_eventSwitcher = make_obj<EventSwitcher>(reactor, _socket->event_handle());

	if (local_addr)
		_socket->bind(local_addr);
	_eventSwitcher->set_read_handler(
		std::bind(&StreamClient::read_handler, this, std::placeholders::_1));
	_eventSwitcher->set_write_handler(
		std::bind(&StreamClient::write_handler, this));
	_eventSwitcher->set_error_handler(
		std::bind(&StreamClient::error_handler, this));

	auto _thread_set = threadPool2::getThreadsPoolInstance();
	setThreadsPool(_thread_set.lock());
	_pbuf = make_obj<bufferManager>();
}

//--------------------------------------------------------------------
aldebaran::core::StreamClient::~StreamClient()
{
	//
}

std::optional<std::future<int>>
aldebaran::core::StreamClient::connect()
{
	if ((!_socket) || (!_peer_addr))
		return std::nullopt;
	std::promise<int> conp;
	_conP = std::move(conp);
	net::connect(_socket->event_handle(), _peer_addr->raw_addr());
	if (!_reactor.expired())
		_reactor.lock()->run_after(0ms, [this]() {
		_eventSwitcher->enable_writing();
			});
	return _conP.get_future();
}

aldebaran::core::StreamClient::Task
aldebaran::core::StreamClient::read(void* _DstBuf, unsigned int _MaxCharCount)
{
	if (!_DstBuf)
		return std::nullopt;
	return _connector->recv((char*)_DstBuf, _MaxCharCount);
}

aldebaran::core::StreamClient::Task
aldebaran::core::StreamClient::write(void const* _Buf, unsigned int _MaxCharCount)
{
	if (!_Buf)
		return std::nullopt;

	return _connector->send(_Buf, _MaxCharCount);
}

void aldebaran::core::StreamClient::closeHandle()
{
	_connector->close();
}

void
aldebaran::core::StreamClient::setThreadsPool(std::shared_ptr<threadPool2::thread_set> thread_set)
{
	_threa_set = thread_set;
	_pbuf->setthreadContext(_threa_set);
}

void aldebaran::core::StreamClient::read_handler(std::chrono::system_clock::time_point time_stamp)
{
}

void aldebaran::core::StreamClient::write_handler()
{
	_eventSwitcher->disable_writing();
	_connector = make_obj<Connector>(_reactor, _socket, _eventSwitcher);
	_connector->setBuffer(_pbuf);
	_connector->createBuffer(64 * 1024, 1024 * 1024);

	//_connector->set_threadsPool(_threa_set);
	_conP.set_value(1);
}

void aldebaran::core::StreamClient::error_handler()
{
	_conP.set_value(0);
}

//--------------------------------------------------------------------