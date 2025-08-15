//--------------------------------------------------------------------
//	Taurus.cpp.
//	07/20/2020.				created.
//	07/21/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "Taurus.h"

//--------------------------------------------------------------------
void
aldebaran::nfx_start(void)
{
	// install the global logger.
	std::shared_ptr<aldebaran::info::Logger<aldebaran::info::DBGStreamPolicy>> dbg_logger;
	dbg_logger.reset(new aldebaran::info::Logger<aldebaran::info::DBGStreamPolicy>(L"DBG_LOG"));
	dbg_logger->set_thread_name(L"[Taurus]");
	dbg_logger->severity_switch(info::SeverityType::Error);
	dbg_logger->severity_switch(info::SeverityType::Debug); //
	dbg_logger->name_output(true);
	dbg_logger->severity_output(false);
	aldebaran::info::LogDaemon::instance()->add_logger(dbg_logger);
}

//--------------------------------------------------------------------
void
aldebaran::nfx_shutdown(void)
{
	// destroy the global logger.
	aldebaran::info::LogDaemon::destroy_instance();
}

//--------------------------------------------------------------------