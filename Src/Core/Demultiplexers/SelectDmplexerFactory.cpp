//--------------------------------------------------------------------
//	SelectDmplexerFactory.cpp.
//	07/08/2020.				by bubo.
//	07/10/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "SelectDemultiplexer.h"
#include "SelectDmplexerFactory.h"

//--------------------------------------------------------------------
aldebaran::core::SelectDmplexerFactory::SelectDmplexerFactory()
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::SelectDmplexerFactory::~SelectDmplexerFactory()
{
	//
}

//--------------------------------------------------------------------
std::shared_ptr<aldebaran::core::Demultiplexer>
aldebaran::core::SelectDmplexerFactory::create_demultiplexer(std::shared_ptr<Reactor> reactor)
{
	return std::shared_ptr<Demultiplexer>(make_obj<SelectDemultiplexer>(reactor));
}

//--------------------------------------------------------------------