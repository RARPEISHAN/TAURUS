//--------------------------------------------------------------------
//	IDemultiplexerFactory.h.
//	07/08/2020.				by bubo.
//	07/10/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _IDEMULTIPLEXER_FACTORY_H
#define _IDEMULTIPLEXER_FACTORY_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../../Common/B_Object.hpp"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	//	incomplete classes.
	//--------------------------------------------------------------------
	class Demultiplexer;
	class Reactor;

	//--------------------------------------------------------------------
	class IDemultiplexerFactory : public B_Object_NonCopyable<IDemultiplexerFactory>
	{
	public:
		virtual std::shared_ptr<Demultiplexer>
			create_demultiplexer(std::shared_ptr<Reactor> reactor) = 0;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_IDEMULTIPLEXER_FACTORY_H
