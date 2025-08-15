//--------------------------------------------------------------------
//	Connection.cpp.
//	07/08/2020.				by bubo.
//	08/03/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "Connection.h"
#include "NetFoundation.h"
#include "EventSwitcher.h"
#include "Reactor.h"
#include "Socket.h"

//--------------------------------------------------------------------
aldebaran::core::Connection::Connection(
	std::shared_ptr<Reactor> reactor,
	EventHandle event_handle,
	std::shared_ptr<NetAddress> local_addr,
	std::shared_ptr<NetAddress> peer_addr) : _reactor(reactor),
	_event_handle(event_handle), _local_addr(local_addr),
	_peer_addr(peer_addr), _eventSwitcher(make_obj<EventSwitcher>(reactor, event_handle))
{
	_eventSwitcher->set_close_handler(std::bind(&Connection::close_handler, this));
	_eventSwitcher->set_read_handler(std::bind(&Connection::read_handler, this));
	_eventSwitcher->set_write_handler(std::bind(&Connection::write_handler, this));
	_eventSwitcher->set_error_handler(std::bind(&Connection::error_handler, this));
}

//--------------------------------------------------------------------
aldebaran::core::Connection::~Connection()
{
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::set_connection_callback(ConnectionCallback const& cb)
{
	_connection_callback = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::set_data_callback(DataCallback const& cb)
{
	_data_callback = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::set_close_callback(ConnectionCloseCallback const& cb)
{
	_connection_close_callback = cb;
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::connect_established(void)
{
	/*  test  */
	_eventSwitcher->enable_reading();
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::connect_dropped(void)
{
	set_state(State::Disconnected);

	_eventSwitcher->disable_all();

	if (_connection_callback)
		_connection_callback(this_obj());
	if (auto reactor = _reactor.lock())
		reactor->remove_event_switcher(_eventSwitcher);
}

//--------------------------------------------------------------------
bool
aldebaran::core::Connection::is_connected(void)
const
{
	return _state == State::Connected;
}

//--------------------------------------------------------------------
bool
aldebaran::core::Connection::is_disconnected(void)
const
{
	return _state == State::Disconnected;
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::shutdown(void)
{
	if (_state == State::Connected)
	{
		set_state(State::Disconnecting);
		if (auto reactor = _reactor.lock())
			reactor->run_in_reactor(std::bind(&Connection::shutdown_handler, this));
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::force_close(void)
{
	if (_state == State::Connected || _state == State::Disconnecting)
	{
		set_state(State::Disconnecting);
		if (auto reactor = _reactor.lock())
			reactor->run_in_reactor(
				std::bind(&Connection::force_close_handler, this_obj()));
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::force_close(std::chrono::milliseconds delay)
{
	if (delay.count() == 0ll)
		force_close();

	if (auto reactor = _reactor.lock())
		reactor->run_after(
			delay,
			std::bind(&Connection::force_close_handler, this_obj()));
}

//--------------------------------------------------------------------

void
aldebaran::core::Connection::close()
{
	if (_event_handle != INVALID_SOCKET)
	{
		net::close(_event_handle);
		_event_handle = INVALID_SOCKET;
		_reactor.lock()->run_after(0ms, [this]()->void {
			_eventSwitcher->disable_all();
			});

		_reactor.lock()->run_after(1000ms, [this]()->void {
			close_handler();
			});
	}
}

void aldebaran::core::Connection::enable_writing(bool starUp)
{
	std::function<void()> fun;
	if (starUp)
		fun = [this]() {
		if (!(_eventSwitcher->event_type() & EventType::Write))
			_eventSwitcher->enable_writing();
	};
	else
		fun = [this]() {
		if (_eventSwitcher->event_type() & EventType::Write)
			_eventSwitcher->disable_writing();
	};
	_reactor.lock()->run_after(0ms, fun);
}

void aldebaran::core::Connection::enable_reading(bool starUp)
{
	std::function<void()> fun;
	if (starUp)
		fun = [this]() {
		if (!(_eventSwitcher->event_type() & EventType::Read))
			_eventSwitcher->enable_reading();
	};
	else
		fun = [this]() {
		if (_eventSwitcher->event_type() & EventType::Read)
			_eventSwitcher->disable_reading();
	};
	_reactor.lock()->run_after(0ms, fun);
}
aldebaran::core::
TimerToken aldebaran::core::Connection::setTimer(std::shared_ptr<Timer> timer)
{
	return TimerToken();
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::set_state(State state)
{
	_state = state;
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::read_handler(void)
{
	enable_reading(false);
	int num = 0;
	int error = 0;
	if (!_buf.expired())
		_buf.lock()->recv(_fd, &num, &error);
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::close_handler(void)
{
	_eventSwitcher->disable_all();
	if (!_buf.expired())
	{
		_buf.lock()->close(_fd);
		_fd = 0;
	}
	if (_connection_close_callback)
		_connection_close_callback(this_obj());
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::write_handler(void)
{
	enable_writing(false);
	int num = 0;
	int error = 0;
	if (!_buf.expired())
		_buf.lock()->send(_fd, &num, &error);
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::error_handler(void)
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::shutdown_handler(void)
{
	//_reactor->is_in_reactor_thread();
	if (!_eventSwitcher->is_writing())
	{
		// we are not writing
		_socket->shutdown_write();
	}
}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::force_close_handler(void)
{
	// _reactor->is_in_reactor_thread();
	if (_state == State::Connected || _state == State::Disconnecting)
	{
		// as if we received 0 byte in method of read_handler();
		close_handler();
	}
}

//--------------------------------------------------------------------
//void
//aldebaran::core::Connection::set_threadsPool(
//	std::weak_ptr<threadPool2::thread_set>  pthread_set)
//{
//	_thread_set = pthread_set;
//}

//--------------------------------------------------------------------
void
aldebaran::core::Connection::createBuffer(size_t initialSize, size_t maxBufSize)
{
	if (!_buf.expired())
	{
		_fd = _buf.lock()->createBuffer(_event_handle, initialSize, maxBufSize);
	}
}

void aldebaran::core::Connection::setBuffer(std::weak_ptr<bufferManager> buf)
{
	_buf = buf;
}

//--------------------------------------------------------------------
void aldebaran::core::Connection::setReadBufMaxSize(size_t maxBufSize)
{
	if (!_buf.expired())
		_buf.lock()->set_bufSize(_fd, maxBufSize, 1);
}
//--------------------------------------------------------------------
void
aldebaran::core::Connection::setWriteBufMaxSize(
	size_t maxBufSize)
{
	if (!_buf.expired())
		_buf.lock()->set_bufSize(_fd, maxBufSize, 2);
}
//--------------------------------------------------------------------
//void
//aldebaran::core::Connection::setReadFinished(
//	ReadCallback cb)
//{
//
//}
//
////--------------------------------------------------------------------
//void
//aldebaran::core::Connection::setWriteFinished(
//	WriteCallback cb)
//{
//
//}

//--------------------------------------------------------------------
aldebaran::core::Connection::Task
aldebaran::core::Connection::send(
	void const* Source,
	unsigned int _maxCharCount)
{
	if (Source && _maxCharCount)
	{
		auto fun = [this]() {
			if (!(_eventSwitcher->event_type() & EventType::Write))
				enable_writing(true);
		};
		task t{ _fd,_buf,const_cast<void*>(Source),_maxCharCount,2 };
		t.set_data_calllback(fun);
		return t;
	}
	return std::nullopt;
}

//--------------------------------------------------------------------
aldebaran::core::Connection::Task
aldebaran::core::Connection::recv(char* Dest,
	unsigned int _maxCharCount)
{
	if (Dest)
	{
		if (!(_eventSwitcher->event_type() & EventType::Read))
			_eventSwitcher->enable_reading();
		return task{ _fd,_buf,static_cast<void*>(Dest),_maxCharCount,1 };
	}
	return std::nullopt;
}

//--------------------------------------------------------------------

int aldebaran::core::Connection::task::get()
{
	int e = -1;
	if (!_pbuf.expired())
		e = _pbuf.lock()->wait(_fd, _iotype);
	if (e >= 0)
	{
		switch (_iotype)
		{
		case 1:
			e = _pbuf.lock()->read(_fd, _Dest, _maxCharCount);
			break;

		case 2:
			e = _pbuf.lock()->write(_fd, _Dest, _maxCharCount);
			break;
		default:
			break;
		}
	}
	if (_cb)
		_cb();
	return e;
}

aldebaran::core::Connection::task::task(fd_t fd,
	std::weak_ptr<bufferManager> pbuf,
	void* Dest,
	size_t maxCharCount,
	int iotype) :_fd(fd), _pbuf(pbuf), _Dest(Dest), _maxCharCount(maxCharCount), _iotype(iotype)
{
}

void aldebaran::core::Connection::task::set_data_calllback(std::function<void(void)> const& cb)
{
	_cb = cb;
}