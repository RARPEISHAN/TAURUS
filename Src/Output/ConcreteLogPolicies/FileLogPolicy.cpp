//--------------------------------------------------------------------
//	FileLogPolicy.cpp.
//	03/04/2020.				by bubo.
//	03/05/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#include "FileLogPolicy.h"

//--------------------------------------------------------------------
aldebaran::info::FileLogPolicy::FileLogPolicy()
{
	//
}

//--------------------------------------------------------------------
aldebaran::info::FileLogPolicy::~FileLogPolicy()
{
	//
}

//--------------------------------------------------------------------
bool
aldebaran::info::FileLogPolicy::open_output_stream(std::wstring const& filename)
{
	/* try to open the file */
	_outputStream.open(
		filename.c_str(),
		std::ios_base::binary | std::ios_base::out);

	if (!_outputStream.is_open())
		return false;

	/* set output precision */
	_outputStream.precision(20);

	/* return success */
	return true;
}

//--------------------------------------------------------------------
void
aldebaran::info::FileLogPolicy::close_output_stream()
{
	_outputStream.close();
}

//--------------------------------------------------------------------
void
aldebaran::info::FileLogPolicy::write(
	std::wstring const& msg,
	SeverityType type)
{
	/* add the message to the stream */
	_outputStream << msg << std::endl;
}

//--------------------------------------------------------------------