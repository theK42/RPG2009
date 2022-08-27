
#include "engine/core/xcore.h"
#include "engine/core/xmswin.h"

#if defined( TARGET_MSWIN )

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

bool XEngine::XAssertMSWin( BOOL bCondition, char const * pFile, int nLine, char const * pExpression, DWORD dwError )
{
	static int const MAX_ERROR_STRING = 1024;
	static char pBuffer[ MAX_ERROR_STRING ];

	if( bCondition != FALSE )
	{
		return false;
	}

	DWORD dwResult = ::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
		0,
		pBuffer,
		MAX_ERROR_STRING,
		NULL );

	DWORD dwFormatError = ::GetLastError();
	XASSERT_MSG( dwResult != 0, "In XAssertMSWin: FormatMessage returned error code 0x%04X.", dwFormatError );

	return XAssertInternal( false, pFile, nLine, pExpression, pBuffer );
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

#endif
