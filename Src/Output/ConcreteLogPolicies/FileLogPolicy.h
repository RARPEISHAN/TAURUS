//--------------------------------------------------------------------
//	FileLogPolicy.h.
//	03/04/2020.				created.
//	03/05/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _FILE_LOG_POLICY_H
#define _FILE_LOG_POLICY_H

//--------------------------------------------------------------------
//	headers...
//--------------------------------------------------------------------
#include <fstream>
#include "../SeverityType.h"

//--------------------------------------------------------------------
namespace aldebaran::info
{
	//--------------------------------------------------------------------
	//	implementation of the concept of 'RLogPolicy' to write to a file
	//  on the hard drive.
	//--------------------------------------------------------------------
	class FileLogPolicy
	{
	public:
		FileLogPolicy();
		~FileLogPolicy();

		bool open_output_stream(const std::wstring& filename);
		void close_output_stream();
		void write(const std::wstring& msg, SeverityType type);

	private:
		std::wofstream					_outputStream;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_FILE_LOG_POLICY_H
