//--------------------------------------------------------------------
//	Logger.h.
//	03/04/2020.				created.
//	04/19/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _LOGGER_H
#define _LOGGER_H

//--------------------------------------------------------------------
//	headers...
//--------------------------------------------------------------------
#include <atomic>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <condition_variable>
#include "LogPolicyConcepts.h"
#include "SeverityType.h"

//--------------------------------------------------------------------
namespace aldebaran::info
{
	//--------------------------------------------------------------------
	//	incomplete types...
	//--------------------------------------------------------------------
	class LogDaemon;

	//--------------------------------------------------------------------
	template<typename LogPolicy>
	requires RLogPolicy<LogPolicy>
		class Logger
	{
	public:
		// constructor and destructor
		Logger(
			const std::wstring& name,
			bool instance_mode = false);
		Logger() {}
		~Logger();

		// sets human-readable name for current thread
		void set_thread_name(std::wstring const& name);

		void set_auto_feedline(bool enable = true);
		bool is_auto_feedline(void) const;

		// print message (varies based on severity level)
		template<SeverityType severity>
		void print(std::wstringstream const& stream);

		template<SeverityType severity>
		void print(std::wstring const& msg);

		void fn_print2(std::wstringstream const& stream, SeverityType severity);
		void fn_print(std::wstring const& msg, SeverityType severity);

		//--------------------------------------------------------------------
		//	info setters / getters.
		//--------------------------------------------------------------------
		void linenum_output(bool is_enable = true);
		bool linenum_output(void) const;

		void time_output(bool is_enable = true);
		bool time_output(void) const;

		void severity_output(bool is_enable = true);
		bool severity_output(void) const;

		void name_output(bool is_enable = true);
		bool name_output(void) const;

		void all_output(bool is_enable = true);

		//--------------------------------------------------------------------
		void severity_switch(SeverityType type, bool is_enable = true);
		bool severity_switch(SeverityType type) const;
		void severity_all_switch(bool is_enable = true);

		//--------------------------------------------------------------------
		static void release_daemon(std::shared_ptr<void> logger);
		static void wait_daemon(std::shared_ptr<void> logger);

		//--------------------------------------------------------------------

	private:
		// used to save the current line number
		unsigned int							_log_line_number;
		// defines a human-readable name for each thread
		std::map<std::thread::id, std::wstring>	_thread_name;
		// the log policy (i.e. write to file, ...)
		LogPolicy								_policy;
		// mutual exclusive writer
		std::timed_mutex						_write_mutex;
		// the content to log
		std::vector<std::pair<SeverityType, std::wstring>>	_log_buffer;
		// the actual logging daemon
		std::thread								_daemon;
		// lock-free boolean to check whether our daemon is still running or not
		std::atomic_flag						_is_still_running = ATOMIC_FLAG_INIT;
		std::condition_variable_any				_cv;

		/* settings. */
		bool									_is_linenum_output{ false };
		bool									_is_time_output{ false };
		bool									_is_severity_output{ false };
		bool									_is_name_output{ false };
		bool									_instance_mode{ false };
		bool									_is_auto_feedline{ false };

		std::unordered_map<SeverityType, bool>	_severity_switches;

		friend class LogDaemon;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
//	Implementation.
//--------------------------------------------------------------------
#include "LoggerImp.h"

//--------------------------------------------------------------------
#endif // !_LOGGER_H
