//--------------------------------------------------------------------
//	LogDaemonImp.h.
//	03/04/2020.				created.
//	04/17/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
//	headers...
//--------------------------------------------------------------------
#include "./ConcreteLogPolicies/FileLogPolicy.h"
#include "LogDaemon.h"
//#include "LogPolicyConcepts.h"
#include <mutex>

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
void
aldebaran::info::LogDaemon::logging_daemon(std::shared_ptr<Logger<Policy>> logger)
{
	if (!logger)
		return;

	if (logger->_instance_mode)
		logging_instance(logger);
	else
		logging_queue(logger);
}

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
void
aldebaran::info::LogDaemon::logging_queue(std::shared_ptr<Logger<Policy>> logger)
{
	/* dump log data if present. */
	std::unique_lock<std::timed_mutex> lock(
		logger->_write_mutex,
		std::defer_lock);

	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		if (logger->_log_buffer.size())
		{
			if (!lock.try_lock_for(std::chrono::milliseconds(50)))
				continue;

			for (auto& x : logger->_log_buffer)
				logger->_policy.write(x.second, x.first);

			logger->_log_buffer.clear();
			lock.unlock();
		}
	} while (logger->_is_still_running.test_and_set() ||
		logger->_log_buffer.size());
}

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
void
aldebaran::info::LogDaemon::logging_instance(std::shared_ptr<Logger<Policy>> logger)
{
	/* dump log data if present. */
	std::unique_lock<std::timed_mutex> lck(
		logger->_write_mutex,
		std::defer_lock);

	do
	{
		lck.lock();
		logger->_cv.wait(lck);
		lck.unlock();

		while (logger->_log_buffer.size() > 0u)
		{
			lck.lock();
			std::vector<std::pair<SeverityType, std::wstring>> out_buf;
			out_buf.swap(logger->_log_buffer);
			lck.unlock();

			for (auto& x : out_buf)
				logger->_policy.write(x.second, x.first);
		}

		//	true if the flag was set before the call.
		//	false otherwise.
	} while (logger->_is_still_running.test_and_set());
}

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
std::shared_ptr<aldebaran::info::Logger<Policy>>
aldebaran::info::LogDaemon::logger()
{
	auto it = _loggers.find(typeid(Policy));

	if (it != _loggers.end())
		return std::reinterpret_pointer_cast<aldebaran::info::Logger<Policy>>(it->second);

	return nullptr;
}

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
void
aldebaran::info::LogDaemon::stop(void)
{
	auto it = _loggers.find(typeid(Policy));
	if (it == _loggers.end())
		return;

	/* terminate the daemon by clearing the still running flag */
	if (auto logger = std::reinterpret_pointer_cast<Logger<Policy>>(it->second))
	{
		logger->_is_still_running.clear();
		logger->_daemon.join();
	}
}

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
void
aldebaran::info::LogDaemon::start(void)
{
	auto it = _loggers.find(typeid(Policy));
	if (it == _loggers.end())
		return;

	/* mark the logging daemon as running */
	if (auto logger = std::reinterpret_pointer_cast<Logger<Policy>>(it->second))
	{
		/* already started. */
		if (logger->_is_still_running.test_and_set())
			return;

		logger->_daemon = std::move(std::thread{ logging_daemon<Policy>, logger });
	}
}

//--------------------------------------------------------------------
template<typename Policy>
requires RLogPolicy<Policy>
inline
void
aldebaran::info::LogDaemon::add_logger(
	std::shared_ptr<Logger<Policy>> logger,
	bool is_start /*= true*/)
{
	if (!logger)
		return;

	_loggers[typeid(Policy)] = logger;
	_fns[typeid(Policy)] = { &Logger<Policy>::release_daemon, &Logger<Policy>::wait_daemon };

	if (is_start)
	{
		logger->_is_still_running.test_and_set();
		logger->_daemon = std::move(std::thread{ logging_daemon<Policy>, logger });
	}
}

//--------------------------------------------------------------------
