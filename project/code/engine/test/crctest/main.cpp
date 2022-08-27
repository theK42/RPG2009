
#include "engine/core/xcore.h"
#include "engine/algorithms/xchecksum.h"

int main()
{
	CRC( 0x8D677589, "text lined!" );
	CRC( 0xA63E5A62, "text lined!\n" );
	CRC( 0x37F3672E, "text lined!\\n" );
}
