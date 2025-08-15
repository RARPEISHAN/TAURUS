//--------------------------------------------------------------------
//	Taurus.h.
//	07/21/2020.				created.
//	03/31/2021.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _TAURUS_H
#define _TAURUS_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <sstream>
#include "./Output/ConcreteLogPolicies/DBGStreamPolicy.h"
#include "./Output/LogDaemon.h"
#include "./Output/Logger.h"
#include "./Core/ReactorFactory.h"

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
	//	Net Framework Extensions of Taurus.
	//--------------------------------------------------------------------
	void nfx_start(void);
	void nfx_shutdown(void);

	template<info::SeverityType severity>
	void nfx_print(std::wstring const& msg);

	template<info::SeverityType severity>
	void nfx_print(std::wstringstream const& msg);

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
template<aldebaran::info::SeverityType severity>
void
aldebaran::nfx_print(std::wstring const& msg)
{
	if (auto logger = info::LogDaemon::instance()->logger<info::DBGStreamPolicy>())
		logger->print<severity>(msg);
}

//--------------------------------------------------------------------
template<aldebaran::info::SeverityType severity>
void
aldebaran::nfx_print(std::wstringstream const& msg)
{
	if (auto logger = info::LogDaemon::instance()->logger<info::DBGStreamPolicy>())
		logger->print<severity>(msg);
}

//--------------------------------------------------------------------
#endif // !_TAURUS_H
