
#include "engine/core/xcore.h"
#include "engine/core/xposix.h"

#if defined TARGET_POSIX

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

bool XEngine::XAssertPosix( bool bCondition, char const * pFile, int nLine, char const * pExpression, int nError )
{
	static int const MAX_ERROR_STRING = 1024;
	static char pBuffer[ MAX_ERROR_STRING ];

	if( bCondition != FALSE )
	{
		return false;
	}

	/* TODO: Formatted output for nError */
	std::sprintf( pBuffer, "Condition failed; POSIX error code 0x%08X", nError );

	return XAssertInternal( false, pFile, nLine, pExpression, pBuffer );
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

#endif
