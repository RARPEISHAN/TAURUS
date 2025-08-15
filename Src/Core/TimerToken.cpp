//--------------------------------------------------------------------
//	TimerToken.cpp.
//	07/13/2020.				by bubo.
//	07/13/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "TimerToken.h"

//--------------------------------------------------------------------
aldebaran::core::TimerToken::TimerToken() : _identifier(0ull)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::TimerToken::TimerToken(
	std::shared_ptr<Timer> timer,
	unsigned long long id) : _timer(timer), _identifier(id)
{
	//
}

//--------------------------------------------------------------------
aldebaran::core::TimerToken::~TimerToken()
{
	//
}

auto aldebaran::core::TimerToken::id()const
{
	return _identifier;
}

bool aldebaran::core::TimerToken::operator<(TimerToken& other)const
{
	return _identifier < other._identifier;
}

bool aldebaran::core::TimerToken::operator==(TimerToken& other)const
{
	return _identifier == other._identifier;
}

std::weak_ptr<aldebaran::core::Timer> aldebaran::core::TimerToken::get()const
{
	return _timer;
}

//--------------------------------------------------------------------