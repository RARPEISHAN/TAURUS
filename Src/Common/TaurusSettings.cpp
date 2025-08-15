//--------------------------------------------------------------------
//	TaurusSettings.cpp.
//	07/20/2020.				by bubo.
//	07/23/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "TaurusSettings.h"

//--------------------------------------------------------------------
//	initialize static variables.
//--------------------------------------------------------------------
std::shared_ptr<aldebaran::TaurusSettings> aldebaran::TaurusSettings::_taurus_settings;
std::shared_mutex aldebaran::TaurusSettings::_mx_access;

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::TaurusSettings>
aldebaran::TaurusSettings::instance(void)
{
	/* checks if needs to make a new one. */
	if (!_taurus_settings)
	{
		std::lock_guard lck(_mx_access);
		if (!_taurus_settings)
			_taurus_settings.reset(new TaurusSettings, &TaurusSettings::destroy);
	}

	return _taurus_settings;
}

//--------------------------------------------------------------------
void
aldebaran::TaurusSettings::destroy_instance(void)
{
	if (_taurus_settings)
	{
		std::lock_guard lck(_mx_access);
		if (_taurus_settings)
			_taurus_settings.reset();
	}
}

//--------------------------------------------------------------------
void
aldebaran::TaurusSettings::destroy(TaurusSettings* taurus_settings)
{
	if (taurus_settings)
		delete taurus_settings;
}

//--------------------------------------------------------------------
aldebaran::TaurusSettings::TaurusSettings()
{
	//
}

//--------------------------------------------------------------------
aldebaran::TaurusSettings::~TaurusSettings()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::TaurusSettings::set_demultiplexer_type(DemultiplexerType dmlexer_type)
{
	std::unique_lock lck(_mx_access);
	_dmlexer_type = dmlexer_type;
}

//--------------------------------------------------------------------
aldebaran::DemultiplexerType
aldebaran::TaurusSettings::demultiplexer_type(void) const
{
	std::shared_lock sh_lck(_mx_access);
	return _dmlexer_type;
}

//--------------------------------------------------------------------
void
aldebaran::TaurusSettings::set_protocol(ProtocolType protocol)
{
	_protocol = protocol;
}

//--------------------------------------------------------------------
aldebaran::ProtocolType
aldebaran::TaurusSettings::protocol(void)
const
{
	return _protocol;
}

//--------------------------------------------------------------------