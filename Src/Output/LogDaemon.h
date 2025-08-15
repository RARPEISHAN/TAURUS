//--------------------------------------------------------------------
//					Espresso(TM) v0.6
//		A CLR Parser Generating Engine for Business
//--------------------------------------------------------------------
//			 Daemon for Espresso Output System.
//--------------------------------------------------------------------
//	LogDaemon.h.
//	03/04/2020.				created.
//	04/17/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _LOG_DAEMON_H
#define _LOG_DAEMON_H

//--------------------------------------------------------------------
//	headers...
//--------------------------------------------------------------------
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include "LogPolicyConcepts.h"

//--------------------------------------------------------------------

namespace aldebaran::info
{
	//--------------------------------------------------------------------
	//	incomplete types.
	//--------------------------------------------------------------------

	template<typename T> requires RLogPolicy<T> class Logger;

	//--------------------------------------------------------------------

	class LogDaemon
	{
	public:
		//--------------------------------------------------------------------
		//	singleton methods...
		//--------------------------------------------------------------------
		static std::shared_ptr<LogDaemon> instance(void);
		static void destroy_instance(void);

		//--------------------------------------------------------------------
		//	the actual logger daemon.
		//--------------------------------------------------------------------
		template<typename Policy>
		requires RLogPolicy<Policy>
			static void logging_daemon(std::shared_ptr<Logger<Policy>> logger);

		template<typename Policy>
		requires RLogPolicy<Policy>
			static void logging_queue(std::shared_ptr<Logger<Policy>> logger);

		template<typename Policy>
		requires RLogPolicy<Policy>
			static void logging_instance(std::shared_ptr<Logger<Policy>> logger);

		template<typename Policy>
		requires RLogPolicy<Policy>
			std::shared_ptr<Logger<Policy>> logger(void);

		template<typename Policy>
		requires RLogPolicy<Policy>
			void stop(void);

		template<typename Policy>
		requires RLogPolicy<Policy>
			void start(void);

		template<typename Policy>
		requires RLogPolicy<Policy>
			void add_logger(std::shared_ptr<Logger<Policy>> logger, bool is_start = true);

		//--------------------------------------------------------------------

	private:
		LogDaemon(void);
		LogDaemon(const LogDaemon&) = delete;
		LogDaemon(LogDaemon&& other) = delete;
		LogDaemon& operator = (const LogDaemon&) = delete;
		~LogDaemon(void);

		//--------------------------------------------------------------------
		/* used for shared smart pointer. */
		//--------------------------------------------------------------------

		static void destroy(LogDaemon* daemon);

		//--------------------------------------------------------------------

	private:
		static std::shared_ptr<LogDaemon>					_log_daemon;
		std::unordered_map<std::type_index,
			std::shared_ptr<void>>							_loggers;

		/* release fun, wait fun. */
		std::unordered_map<std::type_index,
			std::tuple<void(*)(std::shared_ptr<void>),
			void(*)(std::shared_ptr<void>)>>				_fns;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
//	Implementation.
//--------------------------------------------------------------------

#include "LogDaemonImp.h"

//--------------------------------------------------------------------
#endif // !_LOG_DAEMON_H
