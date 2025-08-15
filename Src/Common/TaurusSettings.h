//--------------------------------------------------------------------
//	TaurusSettings.h.
//	07/20/2020.				by bubo.
//	07/21/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _TAURUS_SETTINGS_H
#define _TAURUS_SETTINGS_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <memory>
#include <shared_mutex>
#include "ConcreteSettings/DemultiplexerSettings.h"
#include "ConcreteSettings/Protocols.h"

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
	class TaurusSettings
	{
	public:
		static std::shared_ptr<TaurusSettings> instance(void);
		static void destroy_instance(void);

		//--------------------------------------------------------------------
		void set_demultiplexer_type(DemultiplexerType dmlexer_type);
		DemultiplexerType demultiplexer_type(void) const;

		void set_protocol(ProtocolType protocol);
		ProtocolType protocol(void) const;

		//--------------------------------------------------------------------

	private:
		//--------------------------------------------------------------------
		/*	used for shared smart pointer. */
		//--------------------------------------------------------------------
		static void destroy(TaurusSettings* taurus_settings);

		//--------------------------------------------------------------------
		TaurusSettings();
		TaurusSettings(TaurusSettings const&) = delete;
		TaurusSettings(TaurusSettings&&) = delete;
		~TaurusSettings();

		//--------------------------------------------------------------------

	private:
		//--------------------------------------------------------------------
		static std::shared_ptr<TaurusSettings>		_taurus_settings;
		static std::shared_mutex					_mx_access;

		DemultiplexerType							_dmlexer_type{ DemultiplexerType::Select };
		ProtocolType								_protocol{ ProtocolType::IPv4 };

		//--------------------------------------------------------------------
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_TAURUS_SETTINGS_H
