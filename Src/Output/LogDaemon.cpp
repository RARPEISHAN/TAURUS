//--------------------------------------------------------------------
//	LogDaemon.cpp.
//	03/04/2020.				created.
//	03/05/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "./LogDaemon.h"

//--------------------------------------------------------------------
//	initialize static variables...
//--------------------------------------------------------------------
std::shared_ptr<aldebaran::info::LogDaemon> aldebaran::info::LogDaemon::_log_daemon;

//--------------------------------------------------------------------
aldebaran::info::LogDaemon::LogDaemon(void)
{
	/* mark the logging daemon as running. */
	/*_is_still_running.test_and_set();
	_daemon = std::move(std::thread{ loggingDaemon<LogPolicy>, this });*/
}

//--------------------------------------------------------------------
aldebaran::info::LogDaemon::~LogDaemon(void)
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::info::LogDaemon::destroy(LogDaemon* daemon)
{
	if (daemon)
	{
		/* release daemons. */
		for (auto& [id, tup] : daemon->_fns)
		{
			(*std::get<0>(tup))(daemon->_loggers[id]);
		}

		/* wait daemons. */
		for (auto& [id, tup] : daemon->_fns)
		{
			(*std::get<1>(tup))(daemon->_loggers[id]);
		}

		delete daemon;
	}
}

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::info::LogDaemon>
aldebaran::info::LogDaemon::instance(void)
{
	/* check if needs to make a new one. */
	if (!_log_daemon)
		_log_daemon.reset(
			new LogDaemon(),
			&LogDaemon::destroy);

	return _log_daemon;
}

//--------------------------------------------------------------------
void
aldebaran::info::LogDaemon::destroy_instance(void)
{
	if (_log_daemon)
		_log_daemon.reset();
}

//--------------------------------------------------------------------