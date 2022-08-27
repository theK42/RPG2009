using System;
using System.Collections.Generic;
using System.Text;
using XWEngine;

namespace filetest
{
    class Program
    {
        static void Main(string[] args)
        {
            XWAsyncFile.InitModule();

	        const int dataSize = 4 * 1024;
	        byte[] data = new byte[ dataSize ];

            string sString = "All work and no play make Jack GO INSANE AND KILL EVERYONE.";
            int nStringLength = sString.Length;
            
            for (int nIndex = 0; nIndex < dataSize; nIndex++)
            {
                data[nIndex] = (byte)sString[nIndex % nStringLength];
            }

            XWAsyncFile rOutputFile1 = new XWAsyncFile();

            rOutputFile1.Open("test_out.dat", XWFileFlags.Write | XWFileFlags.Truncate, XWFileAccessMode.Default);
            XWHandle hTransfer = rOutputFile1.BeginAsyncWrite(0, dataSize, data);
            if (rOutputFile1.IsAsyncTransferComplete(hTransfer))
            {
                Console.WriteLine("Write operation completed.");
            }
            else
            {
                Console.WriteLine("Write operation pending.");
            }
            uint nTransferred = rOutputFile1.FinishAsyncTransfer(hTransfer);
            Console.WriteLine("Wrote {0} bytes.", nTransferred);
            
	        rOutputFile1.Close();

            XWAsyncFile rInputFile1 = new XWAsyncFile();
            rInputFile1.Open("test_in.dat", XWFileFlags.Read);
	        hTransfer = rInputFile1.BeginAsyncRead( 0, dataSize, data );
	        if( rInputFile1.IsAsyncTransferComplete( hTransfer ) )
	        {
		        Console.WriteLine( "Read operation completed." );
	        }
	        else
	        {
	        	Console.WriteLine( "Read operation pending." );
	        }
	        nTransferred = rInputFile1.FinishAsyncTransfer( hTransfer );
	        Console.WriteLine( "Read {0}/{1} bytes.", nTransferred, dataSize );
	        rInputFile1.Close();

	        XWAsyncFile.DeinitModule();
            Console.WriteLine("Press any key to continue:");
            Console.Read(); 
        }
    }
}
