//--------------------------------------------------------------------
//	Basics.cpp.
//	07/08/2020.				created.
//	07/21/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#include "Basics.h"

//--------------------------------------------------------------------
namespace aldebaran
{
	//--------------------------------------------------------------------
#ifdef __TAURUS_WIN_ENV
	// Windows
	std::function<void(struct tm* const, time_t const* const)> local_time =
		std::bind(&localtime_s, std::placeholders::_1, std::placeholders::_2);

#else
	// Linux / Unix
	std::function<void(struct tm* const, time_t const* const)> local_time =
		std::bind(&localtime_r, std::placeholders::_1, std::placeholders::_2);
#endif // __TAURUS_WIN_ENV

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------