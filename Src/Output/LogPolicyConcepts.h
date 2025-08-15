//--------------------------------------------------------------------
//	LogPolicyConcepts.h.
//	03/05/2020.				created.
//	04/09/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions, 
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _LOG_POLICY_CONCEPTS_H
#define _LOG_POLICY_CONCEPTS_H


//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include <concepts>
#include <string>
#include "SeverityType.h"


//--------------------------------------------------------------------
namespace aldebaran::info
{
	//--------------------------------------------------------------------
	template <typename T>
	concept RLogPolicy = requires(T t, std::wstring const& u, SeverityType v)
	{
		{ t.open_output_stream(u) } -> std::same_as<bool>;	// u: name of file to open.
		t.close_output_stream();
		t.write(u, v);	// u: message.

	};


	//--------------------------------------------------------------------

}


//--------------------------------------------------------------------
#endif // !_LOG_POLICY_CONCEPTS_H





