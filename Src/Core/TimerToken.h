//--------------------------------------------------------------------
//	TimerToken.h.
//	07/13/2020.				by bubo.
//	07/13/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _TIMER_TOKEN_H
#define _TIMER_TOKEN_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <memory>
#include "Timer.h"

//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------
	class TimerToken
	{
	public:
		TimerToken();
		TimerToken(std::shared_ptr<Timer> timer, unsigned long long id);
		~TimerToken();
		auto id()const;
		bool operator < (TimerToken& other)const;
		bool operator == (TimerToken& other)const;
		std::weak_ptr<Timer> get()const;
	private:
		std::weak_ptr<Timer>	_timer;
		unsigned long long		_identifier;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_TIMER_TOKEN_H
