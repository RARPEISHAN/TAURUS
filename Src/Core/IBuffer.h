//--------------------------------------------------------------------
//	IBuffer.h.
//	07/08/2020.				by bubo.
//	07/08/2020.				lasted modified.
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//--------------------------------------------------------------------
#pragma once

#ifndef _IBUFFER_H
#define _IBUFFER_H

//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "../Common/B_Object.hpp"
#include "../Common/Basics.h"
//--------------------------------------------------------------------
namespace aldebaran::core
{
	//--------------------------------------------------------------------

	class IBuffer : public B_Object<IBuffer>
	{
	public:
		typedef std::function<void(long long, aldebaran::IOTYPE)> ReadFinishCallBack;
		typedef std::function<void(long long, aldebaran::IOTYPE)> WriteFinishCallBack;

		virtual unsigned int read(void* _DstBuf, unsigned int _MaxCharCount, int _flags) = 0;
		virtual unsigned int write(void const* _Buf, _In_ unsigned int _MaxCharCount, int _flags) = 0;
		virtual void setWriteBufMaxSize(size_t) = 0;
		virtual void setReadBufMaxSize(size_t) = 0;
		virtual EventHandle getHandle() = 0;
		virtual bool isEmpty(int) = 0;
		virtual bool isFull(int) = 0;
		virtual ~IBuffer() {}
	};

	//--------------------------------------------------------------------
}

//--------------------------------------------------------------------
#endif // !_IBUFFER_H
