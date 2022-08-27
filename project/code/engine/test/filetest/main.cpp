
#include "engine/core/xcore.h"
#include "engine/resource/xfileio.h"

#define _CRT_SECURE_NO_DEPRECATE

#include <cstring>
#include <cstdio>

using namespace XEngine;
using namespace std;

int main( int argc, char ** argv )
{
	XAsyncFileDescriptorIO::InitModule();

	static uaddr const dataSize = 4 * 1024;
	static char data[ dataSize ];

	char const string[] = "All work and no play makes Jack a dull boy. ";
	uaddr const stringLength = sizeof( string ) - 1;

	uaddr sizeRemaining = dataSize;
	char * p = data;
	while( sizeRemaining > stringLength)
	{
		memcpy( p, string, stringLength );
		sizeRemaining -= stringLength;
		p += stringLength;
	}
	memcpy( p, string, sizeRemaining );

	XAsyncFile oOutputFile1;
	oOutputFile1.Open( "test_out.dat", XFileFlags::Write | XFileFlags::Truncate, 0644 );
	handle hTransfer = oOutputFile1.BeginAsyncWrite( 0, dataSize, data );
	if( oOutputFile1.IsAsyncTransferComplete( hTransfer ) )
	{
		printf( "Write operation completed.\n" );
	}
	else
	{
		printf( "Write operation pending.\n" );
	}
	uaddr nTransferred = oOutputFile1.FinishAsyncTransfer( hTransfer );
#if defined( TARGET_MSWIN )
	printf( "Wrote %Iu/%Iu bytes.\n", nTransferred, dataSize );
#elif defined( TARGET_POSIX )
	printf( "Wrote %tu/%tu bytes.\n", nTransferred, dataSize );
#endif
	oOutputFile1.Close();

	XAsyncFile oInputFile1;
	oInputFile1.Open( "test_in.dat", XFileFlags::Read );
	hTransfer = oInputFile1.BeginAsyncRead( 0, dataSize, data );
	if( oInputFile1.IsAsyncTransferComplete( hTransfer ) )
	{
		printf( "Read operation completed.\n" );
	}
	else
	{
		printf( "Read operation pending.\n" );
	}
	nTransferred = oInputFile1.FinishAsyncTransfer( hTransfer );
#if defined( TARGET_MSWIN )
	printf( "Read %Iu/%Iu bytes.\n", nTransferred, dataSize );
#elif defined( TARGET_POSIX )
	printf( "Read %tu/%tu bytes.\n", nTransferred, dataSize );
#endif
	oInputFile1.Close();

	XAsyncFileDescriptorIO::DeinitModule();

	return 0;
}
