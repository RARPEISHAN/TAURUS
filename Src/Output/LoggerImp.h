//--------------------------------------------------------------------
//	LoggerImp.h.
//	03/04/2020.				created.
//	03/31/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include <iomanip>
#include "../Common/Common.h"
#include "Logger.h"

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
aldebaran::info::Logger<LogPolicy>::Logger(
	const std::wstring& name, bool instance_mode /*= false*/) :
	_log_line_number(0), _instance_mode(instance_mode)
{
	if (!_policy.open_output_stream(name))
		return;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
aldebaran::info::Logger<LogPolicy>::~Logger()
{
	//
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::set_thread_name(const std::wstring& name)
{
	_thread_name[std::this_thread::get_id()] = name;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::set_auto_feedline(bool enable /*= true*/)
{
	_is_auto_feedline = enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
bool
aldebaran::info::Logger<LogPolicy>::is_auto_feedline(void)
const
{
	return _is_auto_feedline;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
template<aldebaran::info::SeverityType severity>
inline
void
aldebaran::info::Logger<LogPolicy>::print(std::wstringstream const& stream)
{
	/* check if the severity type is enabled. */
	auto it = _severity_switches.find(severity);
	if (it == _severity_switches.end())
		return;
	else if (!it->second)
		return;

	std::wstringstream logStream;

	if (_is_auto_feedline && _log_line_number != 0)
		logStream << L"\r\n";

	++_log_line_number;

	if (_is_linenum_output)
		logStream << _log_line_number << L": ";

	if (_is_time_output)
	{
		// get time
		std::time_t t = std::time(nullptr);
		std::tm tm;
		local_time(&tm, &t);

		logStream << std::put_time(&tm, L"%c") << "\t";
	}

	// write down warning level
	if (_is_severity_output)
		switch (severity)
		{
		case SeverityType::Info:
			logStream << L"INFO: ";
			break;
		case SeverityType::Debug:
			logStream << L"DEBUG: ";
			break;
		case SeverityType::Warning:
			logStream << L"WARNING: ";
			break;
		case SeverityType::Error:
			logStream << L"ERROR: ";
			break;
		};

	// write thread name
	if (_is_name_output)
	{
		auto it = _thread_name.find(std::this_thread::get_id());
		if (it != _thread_name.end())
			logStream << it->second << L": ";
	}

	// write the actual message
	logStream << stream.str();
	std::lock_guard<std::timed_mutex> lock(_write_mutex);
	_log_buffer.emplace_back(std::pair{ severity, logStream.str() });

	if (_instance_mode)
		_cv.notify_all();
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
template<aldebaran::info::SeverityType severity>
inline
void
aldebaran::info::Logger<LogPolicy>::print(std::wstring const& msg)
{
	this->print<severity>(std::wstringstream(msg.c_str()));
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::fn_print2(
	std::wstringstream const& stream,
	SeverityType severity)
{
	/* check if the severity type is enabled. */
	auto it = _severity_switches.find(severity);
	if (it == _severity_switches.end())
		return;
	else if (!it->second)
		return;

	std::wstringstream logStream;

	if (_is_auto_feedline && _log_line_number != 0)
		logStream << L"\r\n";

	++_log_line_number;

	if (_is_linenum_output)
		logStream << _log_line_number << L": ";

	if (_is_time_output)
	{
		// get time
		std::time_t t = std::time(nullptr);
		std::tm tm;
		local_time(&tm, &t);

		logStream << std::put_time(&tm, L"%c") << "\t";
	}

	// write down warning level
	if (_is_severity_output)
		switch (severity)
		{
		case SeverityType::Info:
			logStream << L"INFO: ";
			break;
		case SeverityType::Debug:
			logStream << L"DEBUG: ";
			break;
		case SeverityType::Warning:
			logStream << L"WARNING: ";
			break;
		case SeverityType::Error:
			logStream << L"ERROR: ";
			break;
		};

	// write thread name
	if (_is_name_output)
	{
		auto it = _thread_name.find(std::this_thread::get_id());
		if (it != _thread_name.end())
			logStream << it->second << L": ";
	}

	// write the actual message
	logStream << stream.str();
	std::lock_guard<std::timed_mutex> lock(_write_mutex);
	_log_buffer.emplace_back(std::pair{ severity, logStream.str() });

	if (_instance_mode)
		_cv.notify_all();
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::fn_print(
	std::wstring const& msg,
	SeverityType severity)
{
	fn_print2(std::wstringstream(msg.c_str()), severity);
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::linenum_output(bool is_enable /*= true*/)
{
	_is_linenum_output = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
bool
aldebaran::info::Logger<LogPolicy>::linenum_output(void)
const
{
	return _is_linenum_output;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::time_output(bool is_enable /*= true*/)
{
	_is_time_output = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
bool
aldebaran::info::Logger<LogPolicy>::time_output(void)
const
{
	return _is_time_output;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::severity_output(bool is_enable /*= true*/)
{
	_is_severity_output = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
bool
aldebaran::info::Logger<LogPolicy>::severity_output(void)
const
{
	return _is_severity_output;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::name_output(bool is_enable /*= true*/)
{
	_is_name_output = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
bool
aldebaran::info::Logger<LogPolicy>::name_output(void)
const
{
	return _is_name_output;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::all_output(bool is_enable /*= true*/)
{
	_is_linenum_output = _is_time_output = _is_severity_output =
		_is_name_output = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void aldebaran::info::Logger<LogPolicy>::severity_switch(
	SeverityType type,
	bool is_enable /*= true*/)
{
	_severity_switches[type] = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
bool
aldebaran::info::Logger<LogPolicy>::severity_switch(SeverityType type)
const
{
	auto it = _severity_switches.find(type);
	if (it == _severity_switches.end())
		return false;

	return it->second;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::severity_all_switch(bool is_enable /*= true*/)
{
	_severity_switches[SeverityType::Debug] = is_enable;
	_severity_switches[SeverityType::Error] = is_enable;
	_severity_switches[SeverityType::Info] = is_enable;
	_severity_switches[SeverityType::Warning] = is_enable;
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::release_daemon(std::shared_ptr<void> logger)
{
	auto sp_logger = std::reinterpret_pointer_cast<Logger<LogPolicy>>(logger);
	if (!sp_logger)
		return;

	sp_logger->_is_still_running.clear();
}

//--------------------------------------------------------------------
template<typename LogPolicy>
requires RLogPolicy<LogPolicy>
inline
void
aldebaran::info::Logger<LogPolicy>::wait_daemon(std::shared_ptr<void> logger)
{
	auto sp_logger = std::reinterpret_pointer_cast<Logger<LogPolicy>>(logger);
	if (!sp_logger)
		return;

	if (sp_logger->_instance_mode)
		sp_logger->_cv.notify_all();

	if (sp_logger->_daemon.joinable())
		sp_logger->_daemon.join();
}

//--------------------------------------------------------------------
