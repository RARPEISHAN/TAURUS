//--------------------------------------------------------------------
//	Connector.cpp.
//	07/31/2020.				by bubo.
//	07/31/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "Connector.h"

//--------------------------------------------------------------------

aldebaran::core::Connector::Connector(std::weak_ptr<Reactor> reactor,
	std::shared_ptr<Socket> socket,
	std::shared_ptr<EventSwitcher> eventSwitcher)
	:_reactor(reactor),
	_socket(socket),
	_event_handle(socket->event_handle()),
	_eventSwitcher(std::move(eventSwitcher))
{
	_eventSwitcher->set_close_handler(std::bind(&Connector::close_handler, this));
	_eventSwitcher->set_read_handler(std::bind(&Connector::read_handler, this));
	_eventSwitcher->set_write_handler(std::bind(&Connector::write_handler, this));
	_eventSwitcher->set_error_handler(std::bind(&Connector::error_handler, this));
}

//--------------------------------------------------------------------
aldebaran::core::Connector::~Connector()
{
	//
}

void aldebaran::core::Connector::set_close_callback(ConnectorCloseCallback const& cb)
{
	_connection_close_callback = cb;
}
void aldebaran::core::Connector::task::set_data_calllback(std::function<void(void)> const& cb)
{
	_cb = cb;
}

void aldebaran::core::Connector::createBuffer(size_t bufferSize, size_t maxBufSize)
{
	if (!_buf.expired())
	{
		_fd = _buf.lock()->createBuffer(_event_handle, bufferSize, maxBufSize);
	}
}

void aldebaran::core::Connector::setBuffer(std::weak_ptr<bufferManager> buf)
{
	_buf = buf;
}

void aldebaran::core::Connector::setReadBufMaxSize(size_t maxBufSize)
{
	if (!_buf.expired())
		_buf.lock()->set_bufSize(_fd, maxBufSize, 1);
}

void aldebaran::core::Connector::setWriteBufMaxSize(size_t maxBufSize)
{
	if (!_buf.expired())
		_buf.lock()->set_bufSize(_fd, maxBufSize, 2);
}

//void aldebaran::core::Connector::setReadFinished(ReadCallback const& cb)
//{
//	std::weak_ptr<Connector> connector = this_obj();
//	typename IBuffer::ReadFinishCallBack _cb = std::bind(cb, std::placeholders::_1, std::placeholders::_2);
//
//	_streamBuffer->setReadFinishedCallBack([this, _cb](long long s, IOTYPE t) {
//		if (s == 0)
//		{
//			if (t == IOTYPE::OK)
//			{
//				if (_cb)
//				{
//					_cb(s, t);
//				}
//			}
//			else
//				close();
//		}
//		else if (_cb)
//		{
//			_cb(s, t);
//		}
//		});
//}

//void aldebaran::core::Connector::setWriteFinished(WriteCallback const& cb)
//{
//	std::weak_ptr<Connector> connection = this_obj();
//	typename IBuffer::WriteFinishCallBack _cb = std::bind(cb, std::placeholders::_1, std::placeholders::_2);
//	_streamBuffer->setWriteFinishedCallBack(_cb);
//}

aldebaran::core::Connector::Task
aldebaran::core::Connector::send(void const* Source, unsigned int _maxCharCount)
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

aldebaran::core::Connector::Task
aldebaran::core::Connector::recv(char* Dest, unsigned int _maxCharCount)
{
	if (Dest)
	{
		if (!(_eventSwitcher->event_type() & EventType::Read))
			enable_reading(true);
		return task{ _fd,_buf,static_cast<void*>(Dest),_maxCharCount,1 };
	}
	return std::nullopt;
}

void aldebaran::core::Connector::enable_writing(bool starUp)
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

void aldebaran::core::Connector::enable_reading(bool starUp)
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

//--------------------------------------------------------------------

void aldebaran::core::Connector::connect_dropped(void)
{
	set_state(State::Disconnected);

	_eventSwitcher->disable_all();

	if (auto reactor = _reactor.lock())
		reactor->remove_event_switcher(_eventSwitcher);
}

bool aldebaran::core::Connector::is_connected(void) const
{
	return _state == State::Connected;
}

bool aldebaran::core::Connector::is_disconnected(void) const
{
	return _state == State::Disconnected;
}

void aldebaran::core::Connector::shutdown(void)
{
	if (_state == State::Connected)
	{
		set_state(State::Disconnecting);
		if (auto reactor = _reactor.lock())
			reactor->run_in_reactor(std::bind(&Connector::shutdown_handler, this));
	}
}

void aldebaran::core::Connector::force_close(void)
{
	if (_state == State::Connected || _state == State::Disconnecting)
	{
		set_state(State::Disconnecting);
		if (auto reactor = _reactor.lock())
			reactor->run_in_reactor(
				std::bind(&Connector::force_close_handler, this_obj()));
	}
}

void aldebaran::core::Connector::force_close(std::chrono::milliseconds delay)
{
	if (delay.count() == 0ll)
		force_close();

	if (auto reactor = _reactor.lock())
		reactor->run_after(
			delay,
			std::bind(&Connector::force_close_handler, this_obj()));
}

void aldebaran::core::Connector::close()
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

void aldebaran::core::Connector::set_state(State state)
{
	_state = state;
}

void aldebaran::core::Connector::read_handler(void)
{
	enable_reading(false);
	int num = 0;
	int error = 0;
	if (!_buf.expired())
		_buf.lock()->recv(_fd, &num, &error);
}

void aldebaran::core::Connector::close_handler(void)
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

void aldebaran::core::Connector::write_handler(void)
{
	enable_writing(false);
	int num = 0;
	int error = 0;
	if (!_buf.expired())
		_buf.lock()->send(_fd, &num, &error);
}

void
aldebaran::core::Connector::error_handler(void)
{
	close_handler();
}

void
aldebaran::core::Connector::shutdown_handler(void)
{
	if (!_eventSwitcher->is_writing())
	{
		// we are not writing
		_socket->shutdown_write();
	}
}

void aldebaran::core::Connector::force_close_handler(void)
{
	if (_state == State::Connected || _state == State::Disconnecting)
	{
		// as if we received 0 byte in method of read_handler();
		close_handler();
	}
}

aldebaran::core::Connector::task::task(fd_t fd,
	std::weak_ptr<bufferManager> pbuf,
	void* Dest,
	size_t maxCharCount,
	int iotype) :_fd(fd), _pbuf(pbuf), _Dest(Dest), _maxCharCount(maxCharCount), _iotype(iotype)
{
}

int aldebaran::core::Connector::task::get()
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