//--------------------------------------------------------------------
//	DBGStreamPolicy.h.
//	03/05/2020.				created.
//	04/09/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
#pragma once

#ifndef _DBG_STREAM_POLICY_H
#define _DBG_STREAM_POLICY_H

//--------------------------------------------------------------------
//	headers...
//--------------------------------------------------------------------
#include <fstream>
#include "../SeverityType.h"

//--------------------------------------------------------------------
namespace aldebaran::info
{
	//--------------------------------------------------------------------
	//	implementation of the concept of 'RLogPolicy' to write to the
	//  output stream.
	//--------------------------------------------------------------------
	class DBGStreamPolicy
	{
	public:
		DBGStreamPolicy();
		~DBGStreamPolicy();

		bool open_output_stream(std::wstring const& filename);
		void close_output_stream(void);
		void write(std::wstring const& msg, SeverityType type);

	private:
		std::wofstream					_outputStream;
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_DBG_STREAM_POLICY_H
