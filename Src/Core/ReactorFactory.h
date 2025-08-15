#pragma once

//--------------------------------------------------------------------
//	ReactorFactory.h.
//	06/29/2021..				by heming.
//	06/29/2021.				lasted modified.
//--------------------------------------------------------------------

#ifndef _REACTORFACTORY_H
#define _REACTORFACTORY_H
#include "../Common/B_Object.hpp"
#include "./Reactor.h"
#include <future>

namespace aldebaran::core
{
	int defOver();

	class ReactorFactory :public B_Object_NonCopyable<ReactorFactory>
	{
		static std::vector <std::future<int>> fut;

		friend int defOver();
	public:
		ReactorFactory() = default;
		ReactorFactory(const ReactorFactory&) = delete;
		ReactorFactory(ReactorFactory&&) = delete;
		~ReactorFactory() = default;

		std::shared_ptr<Reactor> createReactor();
	};
}

#endif // !_REACTORFACTORY_H
