//--------------------------------------------------------------------
//	NfxDmplexerFactory.h.
//	07/21/2020.				by bubo.
//	07/22/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _NFX_DMPLEXER_FACTORY_H
#define _NFX_DMPLEXER_FACTORY_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <memory>
#include "../../Common/ConcreteSettings/DemultiplexerSettings.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete classes.
	//--------------------------------------------------------------------
	class IDemultiplexerFactory;

	//--------------------------------------------------------------------
	std::shared_ptr<IDemultiplexerFactory> nfx_dmplexer_factory(DemultiplexerType type);

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_NFX_DMPLEXER_FACTORY_H
