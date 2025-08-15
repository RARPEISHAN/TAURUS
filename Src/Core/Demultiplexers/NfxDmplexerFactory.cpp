//--------------------------------------------------------------------
//	NfxDmplexerFactory.cpp.
//	07/21/2020.				by bubo.
//	07/22/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "NfxDmplexerFactory.h"
#include "SelectDmplexerFactory.h"
#include "KQueueDmplexerFactory.h"

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::core::IDemultiplexerFactory>
aldebaran::core::nfx_dmplexer_factory(DemultiplexerType type)
{
	if (type == DemultiplexerType::Select)
	{
		return make_obj<SelectDmplexerFactory>();
	}
	else if (type == DemultiplexerType::KQueue)
	{
		return make_obj<KQueueDmplexerFactory>();
	}
	else if (type == DemultiplexerType::EPoll)
	{
		return nullptr;
	}

	return nullptr;
}

//--------------------------------------------------------------------