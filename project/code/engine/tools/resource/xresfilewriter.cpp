
#include "engine/core/xcore.h"
#include "engine/tools/resource/xresfilewriter.h"
#include "engine/algorithms/xutil.h"

/// for alloca
#include <malloc.h>


/// for byte swapping
#include <stdlib.h>

#pragma intrinsic( _byteswap_ushort, _byteswap_ulong, _byteswap_uint64 )

XEngine::u8 SwapBytesU8( bool bSwap, XEngine::u8 n )
{
	return n;
}

XEngine::u16 SwapBytesU16( bool bSwap, XEngine::u16 n )
{
	return bSwap ? _byteswap_ushort( n ) : n;
}

XEngine::u32 SwapBytesU32( bool bSwap, XEngine::u32 n )
{
	return bSwap ? _byteswap_ulong( n ) : n;
}

XEngine::u64 SwapBytesU64( bool bSwap, XEngine::u64 n )
{
	return bSwap ? _byteswap_uint64( n ) : n;
}

XEngine::s8 SwapBytesS8( bool bSwap, XEngine::s8 n )
{
	return (XEngine::s8)SwapBytesU8( bSwap, (XEngine::u8)n );
}

XEngine::s16 SwapBytesS16( bool bSwap, XEngine::s16 n )
{
	return (XEngine::s16)SwapBytesU16( bSwap, (XEngine::u16)n );
}

XEngine::s32 SwapBytesS32( bool bSwap, XEngine::s32 n )
{
	return (XEngine::s32)SwapBytesU32( bSwap, (XEngine::u32)n );
}

XEngine::s64 SwapBytesS64( bool bSwap, XEngine::s64 n )
{
	return (XEngine::s64)SwapBytesU64( bSwap, (XEngine::u64)n );
}

static XEngine::u32 const RESOURCE_FILE_MAGIC	= 0x52455321;
static XEngine::u32 const RESOURCE_FILE_VERSION	= 0x00010000;

XEngine::XTools::XResourceFileWriter::XResourceFileWriter()
{
	m_oRootObjectReference.m_nSection	= 0;
	m_oRootObjectReference.m_nOffset	= 0;

	m_nDefaultSectionFlags	= 0;
	m_nNumSections			= 0;
	m_nOpenSection			= 0;
}

void XEngine::XTools::XResourceFileWriter::SetAllSectionFlags( unsigned nFlags, unsigned nMask )
{
	nFlags &= nMask;
	nMask = ~nMask;

	for( unsigned i = 0; i < m_nNumSections; ++i )
	{
		SectionHeader & rSection = m_oSectionHeaders[i];

		rSection.m_nFlags &= nMask;
		rSection.m_nFlags |= nFlags;
	}

	m_nDefaultSectionFlags = nFlags;
}

void XEngine::XTools::XResourceFileWriter::CreateSectionIfNecessary()
{
	static u32 const DEFAULT_SECTION_ALIGNMENT	= 32;

	SectionHeader oEmptySectionHeader;
	oEmptySectionHeader.m_nOffset			= 0;
	oEmptySectionHeader.m_nCompressedSize	= 0;
	oEmptySectionHeader.m_nSize				= 0;
	oEmptySectionHeader.m_nAlignment		= DEFAULT_SECTION_ALIGNMENT;
	oEmptySectionHeader.m_nFlags			= m_nDefaultSectionFlags;
	oEmptySectionHeader.m_nDataAddress		= 0;

	while( m_nNumSections <= m_nOpenSection )
	{
		m_oSectionHeaders.push_back( oEmptySectionHeader );
		m_oSectionData.push_back( XMemIO() );
		m_oSectionLinks.push_back( SectionLinkMap() );
		++m_nNumSections;
	}
}

void XEngine::XTools::XResourceFileWriter::OpenSection( unsigned nSection )
{
	m_nOpenSection = nSection;
}

void XEngine::XTools::XResourceFileWriter::OpenNextSection()
{
	++m_nOpenSection;
}

void XEngine::XTools::XResourceFileWriter::SetSectionFlags( unsigned nFlags )
{
	CreateSectionIfNecessary();
	m_oSectionHeaders[m_nOpenSection].m_nFlags = nFlags;
}

void XEngine::XTools::XResourceFileWriter::SetSectionAlignment( uaddr nAlignment )
{
	CreateSectionIfNecessary();
	m_oSectionHeaders[m_nOpenSection].m_nAlignment = (u32)nAlignment;
}

void XEngine::XTools::XResourceFileWriter::SetSectionMinAlignment( uaddr nAlignment )
{
	CreateSectionIfNecessary();
	nAlignment = Max( nAlignment, (uaddr)m_oSectionHeaders[m_nOpenSection].m_nAlignment );
	m_oSectionHeaders[m_nOpenSection].m_nAlignment = (u32)nAlignment;
}

void XEngine::XTools::XResourceFileWriter::SetRootObjectReference()
{
	m_oRootObjectReference.m_nSection	= m_nOpenSection;
	m_oRootObjectReference.m_nOffset	= (u32)Tell();
}

void XEngine::XTools::XResourceFileWriter::WriteBytes( uaddr nNumBytes, void const * pBuffer )
{
	CreateSectionIfNecessary();
	m_oSectionData[m_nOpenSection].Write( nNumBytes, pBuffer );
}

void XEngine::XTools::XResourceFileWriter::WriteLink( handle h )
{
	CreateSectionIfNecessary();
	XMemIO & rDataIO = m_oSectionData[m_nOpenSection];

	u32 nOffset = (u32)rDataIO.Tell();
	m_oSectionLinks[m_nOpenSection][nOffset] = h;

	void * value = nullptr;
	rDataIO.Write( sizeof( void * ), &value );
}

void XEngine::XTools::XResourceFileWriter::ResolveLink( handle h )
{
	Reference oReference;
	oReference.m_nSection	= m_nOpenSection;
	oReference.m_nOffset	= (u32)Tell();

	m_oLinkTargets[h] = oReference;
}

void XEngine::XTools::XResourceFileWriter::Align( uaddr nAlignment )
{
	SetSectionMinAlignment( nAlignment );
	Seek( AlignUp( Tell(), nAlignment ) );
}

void XEngine::XTools::XResourceFileWriter::Seek( uaddr nOffset )
{
	CreateSectionIfNecessary();
	m_oSectionData[m_nOpenSection].Seek( nOffset, XSeekOrigin::Begin );
}

XEngine::uaddr XEngine::XTools::XResourceFileWriter::Tell() const
{
	return (m_nOpenSection < m_nNumSections) ? (uaddr)m_oSectionData[m_nOpenSection].Tell() : 0u;
}

void XEngine::XTools::XResourceFileWriter::Finalize( XSeekIO & rStream )
{
	u32 nFileHeaderOffset			= 0;
	u32 nFileHeaderSize				= sizeof( Header );
	u32 nSectionHeaderArrayOffset	= nFileHeaderOffset + nFileHeaderSize;
	u32 nSectionHeaderArraySize		= sizeof( SectionHeader ) * m_nNumSections;
	u32 nSectionDataOffset			= nSectionHeaderArrayOffset + nSectionHeaderArraySize;
	u32 nFixupDataOffset;
	u32 nFixupDataSize;

	/// Write section data
	rStream.Seek( nSectionDataOffset, XSeekOrigin::Begin );
	for( unsigned i = 0; i < m_nNumSections; ++i )
	{
		SectionHeader & rHeader		= m_oSectionHeaders[i];
		XMemIO const & rData		= m_oSectionData[i];

		rHeader.m_nOffset			= (u32)rStream.Tell();
		rHeader.m_nCompressedSize	= (u32)rData.GetDataSize();
		rHeader.m_nSize				= (u32)rData.GetDataSize();
		rHeader.m_nState			= SectionState::Unloaded;
		rHeader.m_pData				= nullptr;

		/// TODO: Support compression
		rHeader.m_nFlags			&= ~XResourceSectionFlags::Compressed;

		rStream.Write( rData.GetDataSize(), rData.GetData() );
		rStream.Seek( AlignUp( rStream.Tell(), sizeof( u32 ) ), XSeekOrigin::Begin );
	}

	/// Write fixup data
	nFixupDataOffset = (u32)rStream.Tell();

	u32 nFixupBaseArraySize		= 1 + m_nNumSections;
	u32 nFixupBaseArrayBytes	= nFixupBaseArraySize * sizeof( u32 );
	u32 nFixupItemArrayOffset	= nFixupDataOffset + nFixupBaseArrayBytes;

	u32 * pFixupBaseArray = (u32*)alloca( nFixupBaseArrayBytes );

	rStream.Seek( nFixupItemArrayOffset, XSeekOrigin::Begin );

	u32 nNumFixupItems = 0;
	for( unsigned i = 0; i < m_nNumSections; ++i )
	{
		pFixupBaseArray[i] = nNumFixupItems;

		SectionLinkMap const & rSectionLinks = m_oSectionLinks[i];
		for( SectionLinkMap::const_iterator p = rSectionLinks.begin(); p != rSectionLinks.end(); ++p )
		{
			u32 nSectionLinkOffset	= (*p).first;
			handle hLinkName		= (*p).second;

			LinkTargetMap::const_iterator pLinkResolution = m_oLinkTargets.find( hLinkName );

			if( pLinkResolution == m_oLinkTargets.end() )
			{
				/// Link was unresolved, pointer will be (null) at runtime!
				continue;
			}

			Reference oTargetReference = (*pLinkResolution).second;

			FixupItem oItem;
			oItem.m_nOffset							= SwapBytesU32( false, nSectionLinkOffset );
			oItem.m_oReferencedObject.m_nSection	= SwapBytesU32( false, oTargetReference.m_nSection );
			oItem.m_oReferencedObject.m_nOffset		= SwapBytesU32( false, oTargetReference.m_nOffset );

			rStream.Write( sizeof( FixupItem ), &oItem );
			++nNumFixupItems;
		}
	}
	pFixupBaseArray[m_nNumSections] = nNumFixupItems;

	u32 nFixupDataEndOffset = (u32)rStream.Tell();
	nFixupDataSize = nFixupDataEndOffset - nFixupDataOffset;

	rStream.Seek( nFixupDataOffset, XSeekOrigin::Begin );
	rStream.Write( nFixupBaseArrayBytes, pFixupBaseArray );
	XASSERT( (u32)rStream.Tell() == nFixupItemArrayOffset );

	/// Write section headers
	rStream.Seek( nSectionHeaderArrayOffset, XSeekOrigin::Begin );
	for( unsigned i = 0; i < m_nNumSections; ++i )
	{
		SectionHeader oSectionHeader = m_oSectionHeaders[i];

		oSectionHeader.m_nOffset			= SwapBytesU32( false, oSectionHeader.m_nOffset );
		oSectionHeader.m_nCompressedSize	= SwapBytesU32( false, oSectionHeader.m_nCompressedSize );
		oSectionHeader.m_nSize				= SwapBytesU32( false, oSectionHeader.m_nSize );
		oSectionHeader.m_nAlignment			= SwapBytesU32( false, oSectionHeader.m_nAlignment );
		oSectionHeader.m_nFlags				= SwapBytesU32( false, oSectionHeader.m_nFlags );
		oSectionHeader.m_nState				= SwapBytesU32( false, oSectionHeader.m_nState );

		rStream.Write( sizeof( SectionHeader ), &oSectionHeader );
	}
	XASSERT( (u32)rStream.Tell() == nSectionDataOffset );

	/// Write file header
	rStream.Seek( nFileHeaderOffset, XSeekOrigin::Begin );

	Header oHeader;
	oHeader.m_nMagic							= SwapBytesU32( false, RESOURCE_FILE_MAGIC );
	oHeader.m_nVersion							= SwapBytesU32( false, RESOURCE_FILE_VERSION );
	oHeader.m_nFixupDataOffset					= SwapBytesU32( false, nFixupDataOffset );
	oHeader.m_nFixupDataSize					= SwapBytesU32( false, nFixupDataSize );
	oHeader.m_nNumSections						= SwapBytesU32( false, m_nNumSections );
	oHeader.m_oRootObjectReference.m_nSection	= SwapBytesU32( false, m_oRootObjectReference.m_nSection );
	oHeader.m_oRootObjectReference.m_nOffset	= SwapBytesU32( false, m_oRootObjectReference.m_nOffset );

	rStream.Write( sizeof( Header ), &oHeader );
	XASSERT( (u32)rStream.Tell() == nSectionHeaderArrayOffset );
}

void XEngine::XTools::XResourceFileWriter::WriteInt8( s8 n )
{
	n = SwapBytesS8( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteInt16( s16 n )
{
	n = SwapBytesS16( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteInt32( s32 n )
{
	n = SwapBytesS32( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteInt64( s64 n )
{
	n = SwapBytesS64( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteUInt8( u8 n )
{
	n = SwapBytesU8( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteUInt16( u16 n )
{
	n = SwapBytesU16( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteUInt32( u32 n )
{
	n = SwapBytesU32( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteUInt64( u64 n )
{
	n = SwapBytesU64( false, n );
	WriteBytes( sizeof( n ), &n );
}

void XEngine::XTools::XResourceFileWriter::WriteFloat( float f )
{
	union { float f; u32 n; } u = { f };
	u.n = SwapBytesU32( false, u.n );
	WriteBytes( sizeof( u.n ), &u.n );
}

void XEngine::XTools::XResourceFileWriter::WriteDouble( double d )
{
	union { double d; u64 n; } u = { d };
	u.n = SwapBytesU64( false, u.n );
	WriteBytes( sizeof( u.n ), &u.n );
}
