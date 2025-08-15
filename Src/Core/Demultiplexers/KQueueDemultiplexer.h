//--------------------------------------------------------------------
//	KQueueDemultiplexer.h.
//	07/08/2020.				by bubo.
//	07/08/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _KQUEUE_DEMULTIPLEXER_H
#define _KQUEUE_DEMULTIPLEXER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Demultiplexer.h"
#include <sys/event.h>

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class KQueueDemultiplexer : public Demultiplexer
	{
	public:
		KQueueDemultiplexer(std::shared_ptr<Reactor> reactor);
		~KQueueDemultiplexer();
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_KQUEUE_DEMULTIPLEXER_H
