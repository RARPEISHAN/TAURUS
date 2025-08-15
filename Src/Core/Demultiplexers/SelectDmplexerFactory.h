//--------------------------------------------------------------------
//	SelectDmplexerFactory.h.
//	07/08/2020.				by bubo.
//	07/10/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _SELECT_DMPLEXER_FACTORY_H
#define _SELECT_DMPLEXER_FACTORY_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "IDemultiplexerFactory.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class SelectDmplexerFactory : public IDemultiplexerFactory
	{
	public:
		SelectDmplexerFactory();
		~SelectDmplexerFactory();

		virtual std::shared_ptr<Demultiplexer>
			create_demultiplexer(std::shared_ptr<Reactor> reactor) override;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_SELECT_DMPLEXER_FACTORY_H
