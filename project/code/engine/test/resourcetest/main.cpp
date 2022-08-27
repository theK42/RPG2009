
#include "engine/core/xcore.h"
#include "engine/core/xallocator.h"
#include "engine/resource/xresfile.h"
#include "engine/tools/resource/xresfilewriter.h"
#include "engine/graphics/xtexture.h"

using namespace XEngine;
using namespace XEngine::XTools;

class MyAllocator : public XEngine::XAllocator
{
public:
	MyAllocator()
	{
		Register( (void *)0, (void *)~0 );
	}

	virtual ~MyAllocator()
	{
		Unregister();
	}

	virtual void *	Allocate(
						uaddr nSize,
						uaddr nAlignment,
						uaddr nAlignmentOffset,
						char const * pFile,
						int nLine )
	{
		return _aligned_offset_malloc( nSize, nAlignment, nAlignmentOffset );
	}

	virtual void *	Reallocate(
						void * p,
						uaddr nNewSize,
						uaddr nNewAlignment,
						uaddr nAlignmentOffset,
						char const * pFile,
						int nLine )
	{
		return _aligned_offset_realloc( p, nNewSize, nNewAlignment, nAlignmentOffset );
	}

	virtual void	Free( void * p )
	{
		return _aligned_free( p );
	}
};

class StepAsyncIOWrapper : public XAsyncIOWrapper
{
public:
	StepAsyncIOWrapper( XSeekIO & rSyncIO )
		: XAsyncIOWrapper( rSyncIO )
	{
		m_nRelease = 0;
	}

	void Release( unsigned n )
	{
		m_nRelease = n;
	}

public:
	virtual bool IsAsyncTransferComplete( handle hTransfer ) const
	{
		if( m_nRelease != 0 )
		{
			--m_nRelease;
			return true;
		}
		return false;
	}

private:
	mutable unsigned	m_nRelease;
};

void WriteResourceFile( XSeekIO & rStream, XAllocator * pAllocator )
{
	static uint const TX_WIDTH	= 1024;
	static uint const TX_HEIGHT	= 1024;
	static int const TX_FORMAT	= XTextureFormat::RGBA_8888;
	static int const TX_FLAGS	= XTextureFlags::Discardable | XTextureFlags::GenerateMips;
	static uaddr const TX_SIZE	= 4 * TX_WIDTH * TX_HEIGHT;

	u8 (*pTexels)[TX_WIDTH][4] = XNEW( pAllocator ) u8[TX_HEIGHT][TX_WIDTH][4];

	for( unsigned i = 0; i < TX_HEIGHT; ++i )
	{
		for( unsigned j = 0; j < TX_WIDTH; ++j )
		{
			pTexels[i][j][0] = 0x1F;
			pTexels[i][j][1] = 0x3F;
			pTexels[i][j][2] = 0x5F;
			pTexels[i][j][3] = 0xFF;
		}
	}

	XResourceFileWriter oWriter;

	oWriter.SetRootObjectReference();
	oWriter.WriteUInt16( TX_FORMAT );
	oWriter.WriteUInt16( TX_WIDTH );
	oWriter.WriteUInt16( TX_HEIGHT );
	oWriter.WriteUInt16( TX_FLAGS );
	oWriter.WriteUInt32( 0 );
	oWriter.WriteLink( pTexels );

	oWriter.OpenNextSection();
	oWriter.SetSectionFlags( XResourceSectionFlags::Discardable );
	oWriter.ResolveLink( pTexels );
	oWriter.WriteBytes( TX_SIZE, pTexels );

	oWriter.Finalize( rStream );

	XDELETE_ARRAY( pTexels, pAllocator );
}

void ReadResourceFile( XSeekIO & rStream, XAllocator * pAllocator )
{
	StepAsyncIOWrapper oStream( rStream );

	XResourceReader oResource;
	oResource.Init( pAllocator );
	oResource.Load( oStream );

	for( int i = 0; true; ++i )
	{
		for( int j = 0; j < i; ++j )
		{
			oStream.Release(1);
			oResource.Process();
		}

		if( oResource.IsLoadComplete() )
		{
			break;
		}

		oResource.Unload();
		for( int k = 0; true; ++k )
		{
			for( int l = 0; l < k; ++l )
			{
				oStream.Release(1);
				oResource.Process();
			}

			if( oResource.IsLoadComplete() )
			{
				break;
			}

			oResource.Load( oStream );
			oResource.WaitForLoad();
			oResource.Unload();
		}
		oResource.Load( oStream );
	}

	oResource.LoadAllSections();

	for( int i = 0; true; ++i )
	{
		for( int j = 0; j < i; ++j )
		{
			oStream.Release(1);
			oResource.Process();
		}

		if( oResource.IsLoadComplete() )
		{
			break;
		}

		oResource.UnloadAllSections();
		for( int k = 0; true; ++k )
		{
			for( int l = 0; l < k; ++l )
			{
				oStream.Release(1);
				oResource.Process();
			}

			if( oResource.IsLoadComplete() )
			{
				break;
			}

			oResource.LoadAllSections();
			oResource.WaitForLoad();
			oResource.UnloadAllSections();
		}
		oResource.LoadAllSections();
	}

	oResource.Unload();
	oResource.WaitForLoad();
	oResource.Load( oStream );
	oResource.WaitForLoad();
	oResource.LoadAllSections();

	for( int i = 0; true; ++i )
	{
		for( int j = 0; j < i; ++j )
		{
			oStream.Release(1);
			oResource.Process();
		}

		if( oResource.IsLoadComplete() )
		{
			break;
		}

		oResource.Unload();
		for( int k = 0; true; ++k )
		{
			for( int l = 0; l < k; ++l )
			{
				oStream.Release(1);
				oResource.Process();
			}

			if( oResource.IsLoadComplete() )
			{
				break;
			}

			oResource.Load( oStream );
			oResource.WaitForLoad();
			oResource.LoadAllSections();
			oResource.WaitForLoad();
			oResource.Unload();
		}
		oResource.Load( oStream );
		oResource.WaitForLoad();
		oResource.LoadAllSections();
	}

	XTexture * pTexture = (XTexture *)oResource.GetRootObject();
	oResource.UnloadDiscardableSections();

	oResource.Unload();
	oResource.Deinit();
}

int main()
{
	XAllocator::InitModule();

	MyAllocator oAllocator;
	XMemIO oResourceData;

	WriteResourceFile( oResourceData, &oAllocator );
	ReadResourceFile( oResourceData, &oAllocator );

	XAllocator::DeinitModule();

	return 0;
}
