
#pragma once

namespace XEngine
{
	XChecksum XChecksum_CalculateFromMemory( void const * pMemory, uaddr nSize );
	XChecksum XChecksum_CalculateFromString( char const * pString );
	XChecksum XChecksum_VerifyFromString( XChecksum nChecksum, char const * pString );
}

inline XEngine::XChecksum XEngine::XChecksum_VerifyFromString( XChecksum nChecksum, char const * pString )
{
	XASSERT( nChecksum == XChecksum_CalculateFromString( pString ) );
	return nChecksum;
}

#if defined XVERIFY_CHECKSUMS
#	define CRC(value, string)		XEngine::XChecksum_VerifyFromString( ((XEngine::XChecksum)(value)), (string) )
#else
#	define CRC(value, string)		((XEngine::XChecksum)(value))
#endif

#define CRC_CONST(value, string)	((XEngine::XChecksum)(value))
