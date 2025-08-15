//--------------------------------------------------------------------
//	DBGStreamPolicy.cpp.
//	03/05/2020.				by bubo.
//	04/09/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "./DBGStreamPolicy.h"
#include <iostream>

//--------------------------------------------------------------------
aldebaran::info::DBGStreamPolicy::DBGStreamPolicy()
{
	//
}

//--------------------------------------------------------------------
aldebaran::info::DBGStreamPolicy::~DBGStreamPolicy()
{
	//
}

//--------------------------------------------------------------------
bool
aldebaran::info::DBGStreamPolicy::open_output_stream(std::wstring const& filename)
{
	return true;
}

//--------------------------------------------------------------------
void
aldebaran::info::DBGStreamPolicy::close_output_stream()
{
	//
}

//--------------------------------------------------------------------
void
aldebaran::info::DBGStreamPolicy::write(
	std::wstring const& msg,
	SeverityType type)
{
	std::wcout << msg;
}

//--------------------------------------------------------------------