//--------------------------------------------------------------------
//	KQueueDmplexerFactory.h.
//	07/20/2020.				by bubo.
//	07/21/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _KQUEUE_DMPLEXER_FACTORY_H
#define _KQUEUE_DMPLEXER_FACTORY_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "IDemultiplexerFactory.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class KQueueDmplexerFactory : public IDemultiplexerFactory
	{
	public:
		KQueueDmplexerFactory();
		~KQueueDmplexerFactory();

		virtual std::shared_ptr<Demultiplexer>
			create_demultiplexer(std::shared_ptr<Reactor> reactor) override;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_KQUEUE_DMPLEXER_FACTORY_H
