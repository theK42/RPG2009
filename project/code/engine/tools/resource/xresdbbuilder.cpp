
#include "engine/core/xcore.h"
#include "engine/tools/resource/xresdbbuilder.h"
#include "engine/algorithms/xutil.h"
#include "engine/algorithms/xchecksum.h"

#include <cstring>
#include <algorithm>

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XTools::XResourceDatabaseBuilderNamedObject::SetName( char const * pName )
{
	int nSize = 1 + (int)std::strlen( pName );
	nSize = Min( nSize, MAX_NAME_LENGTH );
	std::memcpy( m_pName, pName, nSize * sizeof( char ) );
	m_pName[ MAX_NAME_LENGTH ] = '\0';
	m_nNameCRC = XChecksum_CalculateFromString( m_pName );
}

///----------------------------------------------------------------------------

static XEngine::u32 const RESOURCE_DATABASE_MAGIC	= 0x52446200;
static XEngine::u32 const RESOURCE_DATABASE_VERSION	= 0x00010000;

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

class CompareObjects
{
public:
	inline bool operator ()( XEngine::XTools::XResourceDatabaseBuilderObject * p, XEngine::XTools::XResourceDatabaseBuilderObject * q ) const
	{
		return (p->GetType() < q->GetType()) || ((p->GetType() == q->GetType()) && (p->GetNameCRC() < q->GetNameCRC()));
	}
};

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XEngine::handle XEngine::XTools::XResourceDatabaseBuilder::MapString( char const * pString )
{
	StringTable::const_iterator p = m_oStringTable.find( pString );
	if( p == m_oStringTable.end() )
	{
		m_oStringTable.insert( StringTable::value_type( pString, (handle)pString ) );
		return (handle)pString;
	}
	else
	{
		return (*p).second;
	}
}

///----------------------------------------------------------------------------

void XEngine::XTools::XResourceDatabaseBuilder::Finalize( XSeekIO & rStream, bool bWriteStringTable )
{
	m_oWriter.OpenSection( DEBUG_SECTION );
	m_oWriter.SetSectionFlags( XResourceSectionFlags::Compressed | XResourceSectionFlags::AutoLoadDebug );

	m_oWriter.OpenSection( DISCARDABLE_SECTION );
	m_oWriter.SetSectionFlags( XResourceSectionFlags::Compressed | XResourceSectionFlags::AutoLoad | XResourceSectionFlags::Discardable );

	m_oWriter.OpenSection( MAIN_SECTION );
	m_oWriter.SetSectionFlags( XResourceSectionFlags::Compressed | XResourceSectionFlags::AutoLoad );
	m_oWriter.WriteUInt32( RESOURCE_DATABASE_MAGIC );
	m_oWriter.WriteUInt32( RESOURCE_DATABASE_VERSION );
	m_oWriter.WriteUInt32( (u32)m_oObjects.size() );
	m_oWriter.WriteLink( &m_oObjects );

	std::sort( m_oObjects.begin(), m_oObjects.end(), CompareObjects() );

	m_oWriter.ResolveLink( &m_oObjects );
	for( ObjectList::const_iterator p = m_oObjects.begin(); p != m_oObjects.end(); ++p )
	{
		XResourceDatabaseBuilderObject * pObject = *p;
		char const * pName = pObject->GetName();

		m_oWriter.WriteUInt32( pObject->GetType() );
		m_oWriter.WriteUInt32( pObject->GetNameCRC() );
		m_oWriter.WriteLink( MapString( pName ) );
		m_oWriter.WriteLink( pObject );
	}

	for( ObjectList::const_iterator p = m_oObjects.begin(); p != m_oObjects.end(); ++p )
	{
		XResourceDatabaseBuilderObject * pObject = *p;
		pObject->Write( *this, m_oWriter, pObject );
	}

	if( bWriteStringTable )
	{
		m_oWriter.OpenSection( DEBUG_SECTION );
		for( StringTable::const_iterator p = m_oStringTable.begin(); p != m_oStringTable.end(); ++p )
		{
			char const * pString = (*p).first;
			handle h = (*p).second;

			m_oWriter.Align( 4 );
			m_oWriter.ResolveLink( h );
			m_oWriter.WriteBytes( 1 + std::strlen( pString ), pString );
		}
	}

	m_oWriter.Finalize( rStream );
}

///----------------------------------------------------------------------------