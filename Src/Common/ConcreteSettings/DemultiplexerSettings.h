//--------------------------------------------------------------------
//	DemultiplexerSettings.h.
//	07/20/2020.				by bubo.
//	07/21/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _DEMULTIPLEXER_SETTINGS_H
#define _DEMULTIPLEXER_SETTINGS_H

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
	enum class DemultiplexerType
	{
		Select = 0,
		KQueue,
		EPoll,
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_DEMULTIPLEXER_SETTINGS_H
