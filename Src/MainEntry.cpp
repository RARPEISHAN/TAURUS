//--------------------------------------------------------------------
//	MainEntry.cpp.
//	07/08/2020.				created.
//	07/31/2020.				lasted modified.
//--------------------------------------------------------------------
//	*	if has any questions,
//	*	please contact me at 'full1900@outlook.com'.
//	*	by bubo.
//--------------------------------------------------------------------
//	headers.
//--------------------------------------------------------------------
#include "Taurus.h"
#include "./Core/Reactor.h"
#include "./Core/ReactorFactory.h"
#include "./Core/StreamServer.h"
#include "./Core/NetAddress.h"
//--------------------------------------------------------------------

int main(void)
{
	aldebaran::nfx_start();

	WSADATA  wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 0;

	int alMain(void);

	alMain();

	aldebaran::nfx_shutdown();

	return 0;
}

//--------------------------------------------------------------------