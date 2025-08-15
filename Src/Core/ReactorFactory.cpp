#include "./ReactorFactory.h"

std::vector <std::future<int>> aldebaran::core::ReactorFactory::fut;

std::shared_ptr<aldebaran::core::Reactor> aldebaran::core::ReactorFactory::createReactor()
{
	std::future <std::shared_ptr< aldebaran::core::Reactor>> ret = std::async([]()->std::shared_ptr< aldebaran::core::Reactor> {
		auto ret = aldebaran::make_obj<aldebaran::core::Reactor>();
		if (ret)
		{
			ret->init();
		}
		return ret;
		});
	auto reactor = ret.get();

	if (reactor)
	{
		fut.emplace_back(std::async([reactor]()->int {reactor->loop(); return 0; }));
	}

	return reactor;
}

int aldebaran::core::defOver()
{
	if (!ReactorFactory::fut.empty())
		for (auto& f : ReactorFactory::fut)
			f.get();
	return 0;
}