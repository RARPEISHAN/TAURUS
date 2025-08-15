//--------------------------------------------------------------------
//	KQueueDmplexerFactory.cpp.
//	07/20/2020.				by bubo.
//	07/21/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "KQueueDmplexerFactory.h"

//--------------------------------------------------------------------
aldebaran::core::KQueueDmplexerFactory::KQueueDmplexerFactory()
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::KQueueDmplexerFactory::~KQueueDmplexerFactory()
{
	//
}

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::core::Demultiplexer>
aldebaran::core::KQueueDmplexerFactory::create_demultiplexer(std::shared_ptr<Reactor> reactor)
{
	if (!reactor)
		return nullptr;

	return nullptr;
}

//--------------------------------------------------------------------