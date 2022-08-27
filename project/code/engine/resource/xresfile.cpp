
#include "engine/core/xcore.h"
#include "engine/resource/xresfile.h"

#include "engine/core/xallocator.h"
#include "engine/resource/xfileio.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

static XEngine::u32 const RESOURCE_FILE_MAGIC	= 0x52455321;
static XEngine::u32 const RESOURCE_FILE_VERSION	= 0x00010000;

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XResourceReader::Init( XAllocator * pDefaultAllocator, XAllocator * pDiscardableAllocator )
{
	m_pDefaultAllocator		= pDefaultAllocator;
	m_pDiscardableAllocator	= (pDiscardableAllocator == nullptr) ? pDefaultAllocator : pDiscardableAllocator;
	m_pStream				= nullptr;
	m_pPendingStream		= nullptr;
	m_pSections				= nullptr;
	m_pFixupData			= nullptr;
	m_hTransfer				= nullptr;
	m_nState				= FileState::Unloaded;
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::Deinit()
{
	Unload();
	WaitForLoad();

	m_pDefaultAllocator		= nullptr;
	m_pDiscardableAllocator	= nullptr;
}

///----------------------------------------------------------------------------

inline void XEngine::XResourceReader::StartTransfer( int nStage )
{
	uaddr	nOffset	= 0;
	uaddr	nSize	= 0;
	void *	pBuffer	= nullptr;

	switch( nStage )
	{
	case FileState::LoadHeader:
		nOffset	= 0;
		nSize	= sizeof( Header );
		pBuffer	= &m_oHeader;
		break;

	case FileState::LoadSectionHeaders:
		nOffset	= sizeof( Header );
		nSize	= sizeof( SectionHeader ) * m_oHeader.m_nNumSections;
		pBuffer	= m_pSections = (SectionHeader *)m_pDefaultAllocator->Allocate( nSize, alignof( SectionHeader ), __FILE__, __LINE__ );
		break;

	case FileState::LoadFixupData:
		nOffset	= m_oHeader.m_nFixupDataOffset;
		nSize	= m_oHeader.m_nFixupDataSize;
		pBuffer	= m_pFixupData = m_pDefaultAllocator->Allocate( nSize, alignof( FixupItem ), __FILE__, __LINE__ );
		break;
	}

	if( nSize > 0 )
	{
		m_hTransfer	= m_pStream->BeginAsyncRead( nOffset, nSize, pBuffer );
	}

}

///----------------------------------------------------------------------------

inline bool XEngine::XResourceReader::FinishTransfer( int nStage, bool bWait )
{
	uaddr nSize = 0;

	switch( nStage )
	{
	case FileState::LoadHeader:
		nSize = sizeof( Header );
		break;

	case FileState::LoadSectionHeaders:
		nSize = sizeof( SectionHeader ) * m_oHeader.m_nNumSections;
		break;

	case FileState::LoadFixupData:
		nSize = m_oHeader.m_nFixupDataSize;
		break;
	}

	if( nSize == 0 )
	{
		return true;
	}
	else if( bWait || m_pStream->IsAsyncTransferComplete( m_hTransfer ) )
	{
		u64 nBytesRead = m_pStream->FinishAsyncTransfer( m_hTransfer );
		m_hTransfer = nullptr;
		XASSERT( nBytesRead == nSize );
		return true;
	}
	return false;
}

///----------------------------------------------------------------------------

inline bool XEngine::XResourceReader::FinishCanceledTransfer( bool bWait )
{
	if( bWait || m_pStream->IsAsyncTransferComplete( m_hTransfer ) )
	{
		m_pStream->FinishAsyncTransfer( m_hTransfer );
		m_hTransfer = nullptr;
		return true;
	}
	return false;
}

///----------------------------------------------------------------------------

inline void XEngine::XResourceReader::CancelTransfer()
{
	XASSERT_PTR( m_hTransfer );
	m_pStream->CancelAsyncTransfer( m_hTransfer );
}

///----------------------------------------------------------------------------

inline void XEngine::XResourceReader::TransitionTo( int nState )
{
	m_nState = nState;
}

///----------------------------------------------------------------------------

inline void XEngine::XResourceReader::TransitionSectionTo( SectionHeader * pSection, int nState )
{
	pSection->m_nState = nState;
}

///----------------------------------------------------------------------------

inline void XEngine::XResourceReader::StartSectionTransfer( SectionHeader * pSection, bool bAllocate )
{
	if( bAllocate )
	{
		XASSERT_MSG( pSection->m_pData == nullptr, "XResourceReader: Asked to allocate data buffer for section, but it already has a buffer. How did this happen?" );
		XAllocator * pAllocator = (pSection->m_nFlags & SectionFlags::Discardable) ? m_pDiscardableAllocator : m_pDefaultAllocator;
		pSection->m_pData = pAllocator->Allocate( pSection->m_nSize, pSection->m_nAlignment, __FILE__, __LINE__ );
	}
	if( pSection->m_nCompressedSize > 0 )
	{
		m_hTransfer = m_pStream->BeginAsyncRead( pSection->m_nOffset, pSection->m_nCompressedSize, pSection->m_pData );
	}
	pSection->m_nState = SectionState::Load;
	m_pLoadingSection = pSection;
}

///----------------------------------------------------------------------------

inline bool XEngine::XResourceReader::FinishSectionTransfer( bool bWait )
{
	u32 nSize = m_pLoadingSection->m_nCompressedSize;

	if( nSize == 0 )
	{
		return true;
	}
	else if( bWait || m_pStream->IsAsyncTransferComplete( m_hTransfer ) )
	{
		u64 nBytesRead = m_pStream->FinishAsyncTransfer( m_hTransfer );
		m_hTransfer = nullptr;
		XASSERT( nBytesRead == nSize );
		return true;
	}
	return false;
}

///----------------------------------------------------------------------------

inline bool XEngine::XResourceReader::StartPendingSectionTransfer()
{
	unsigned nNumSections = m_oHeader.m_nNumSections;
	for( unsigned i = 0; i < nNumSections; ++i )
	{
		SectionHeader * pSection = m_pSections + i;
		if( pSection->m_nState == SectionState::Pending )
		{
			StartSectionTransfer( pSection, true );
			return true;
		}
	}
	return false;
}

///----------------------------------------------------------------------------

inline void XEngine::XResourceReader::DeleteSection( SectionHeader * pSection )
{
	XAllocator * pAllocator = (pSection->m_nFlags & SectionFlags::Discardable) ? m_pDiscardableAllocator : m_pDefaultAllocator;
	pAllocator->Free( pSection->m_pData );
	pSection->m_pData = nullptr;
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::DeleteEverything()
{
	if( m_pFixupData != nullptr )
	{
		m_pDefaultAllocator->Free( m_pFixupData );
		m_pFixupData = nullptr;
	}

	if( m_pSections != nullptr )
	{
		unsigned nNumSections = m_oHeader.m_nNumSections;
		for( unsigned i = 0; i < nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;
			if( pSection->m_pData == nullptr )
			{
				continue;
			}

			DeleteSection( pSection );
		}

		m_pDefaultAllocator->Free( m_pSections );
		m_pSections = nullptr;
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::DoFixup()
{
	u32 * pSectionBases	= (u32*)m_pFixupData;
	FixupItem * pItems	= AlignUp( (FixupItem *)(pSectionBases + m_oHeader.m_nNumSections + 1), alignof( FixupItem ) );

	for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
	{
		SectionHeader * pSection = m_pSections + i;
		if( pSection->m_nState != SectionState::Loaded )
		{
			continue;
		}

		FixupItem * pBase	= pItems + pSectionBases[ i ];
		FixupItem * pEnd	= pItems + pSectionBases[ i + 1 ];

		uaddr nSectionDataAddress = pSection->m_nDataAddress;

		for( FixupItem * p = pBase; p < pEnd; ++p )
		{
			void ** pPtr = (void **)(nSectionDataAddress + p->m_nOffset);
			*pPtr = ResolveReference( p->m_oReferencedObject );
		}
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::Load( XAsyncIO & rStream )
{
	switch( m_nState )
	{
	case FileState::Unload:
		m_pPendingStream = &rStream;
		TransitionTo( FileState::Reload );
		break;

	case FileState::Unloaded:
		m_pStream = &rStream;
		StartTransfer( FileState::LoadHeader );
		TransitionTo( FileState::LoadHeader );
		break;

	case FileState::Loaded:
	case FileState::LoadHeader:
	case FileState::LoadSectionHeaders:
	case FileState::LoadFixupData:
	case FileState::LoadSections:
		/* nothing to do but wait */
		XASSERT_MSG( m_pStream == &rStream, "XResourceReader: Already loading from a different stream." );
		break;

	case FileState::Reload:
		XASSERT_MSG( m_pPendingStream == &rStream, "XResourceReader: Already loading from a different stream." );
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::Unload()
{
	switch( m_nState )
	{
	case FileState::Unload:
	case FileState::Unloaded:
		/* nothing to do */
		break;

	case FileState::Loaded:
		DeleteEverything();
		m_pStream = nullptr;
		TransitionTo( FileState::Unloaded );
		break;

	case FileState::LoadHeader:
	case FileState::LoadSectionHeaders:
	case FileState::LoadFixupData:
	case FileState::LoadSections:
		CancelTransfer();
		/// fall through...

	case FileState::Reload:
		m_pPendingStream = nullptr;
		TransitionTo( FileState::Unload );
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::LoadSection( unsigned int nSection )
{
	XASSERT( nSection < m_oHeader.m_nNumSections );
	SectionHeader * pSection = m_pSections + nSection;

	switch( m_nState )
	{
	case FileState::Unload:
	case FileState::Unloaded:
	case FileState::LoadHeader:
	case FileState::LoadSectionHeaders:
		XASSERT_MSG( 0, "XResourceReader: Can't load sections in this state." );
		break;

	case FileState::Loaded:
		if( pSection->m_nState != SectionState::Loaded )
		{
			XASSERT_MSG( pSection->m_nState == SectionState::Unloaded, "XResourceReader: File state is \"Loaded\" but section state is neither \"Loaded\" nor \"Unloaded\". How did this happen?" );
			StartSectionTransfer( pSection, true );
			TransitionSectionTo( pSection, SectionState::Load );
			TransitionTo( FileState::LoadSections );
		}
		break;

	case FileState::LoadFixupData:
		TransitionSectionTo( pSection, SectionState::Pending );
		break;

	case FileState::LoadSections:
		switch( pSection->m_nState )
		{
		case SectionState::Unload:
			TransitionSectionTo( pSection, SectionState::Reload );
			break;

		case SectionState::Unloaded:
			TransitionSectionTo( pSection, SectionState::Pending );
			break;

		case SectionState::Loaded:
		case SectionState::Pending:
		case SectionState::Load:
		case SectionState::Reload:
			/* nothing to do */
			break;

		default:
			XASSERT(0);
		}
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::UnloadSection( unsigned nIndex )
{
	XASSERT( nIndex < m_oHeader.m_nNumSections );
	SectionHeader * pSection = m_pSections + nIndex;

	switch( m_nState )
	{
	case FileState::Unload:
	case FileState::Unloaded:
	case FileState::LoadHeader:
	case FileState::LoadSectionHeaders:
		/* nothing to do */
		break;

	case FileState::Loaded:
		if( pSection->m_nState != SectionState::Unloaded )
		{
			XASSERT_MSG( pSection->m_nState == SectionState::Loaded, "XResourceReader: File state is \"Loaded\" but section state is neither \"Loaded\" nor \"Unloaded\". How did this happen?" );
			DeleteSection( pSection );
			TransitionSectionTo( pSection, SectionState::Unloaded );
			DoFixup();
		}
		break;

	case FileState::LoadFixupData:
		TransitionSectionTo( pSection, SectionState::Unloaded );
		break;

	case FileState::LoadSections:
		switch( pSection->m_nState )
		{
		case SectionState::Unload:
		case SectionState::Unloaded:
			/* nothing to do */
			break;

		case SectionState::Loaded:
			DeleteSection( pSection );
			/// fall through...

		case SectionState::Pending:
			TransitionSectionTo( pSection, SectionState::Unloaded );
			break;

		case SectionState::Load:
			CancelTransfer();
			/// Fall through...

		case SectionState::Reload:
			TransitionSectionTo( pSection, SectionState::Unload );
			break;

		default:
			XASSERT(0);
		}
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::ProcessInternal( bool bWait )
{
	switch( m_nState )
	{
	case FileState::Unload:
		if( FinishCanceledTransfer( bWait ) )
		{
			DeleteEverything();
			m_pStream = nullptr;
			TransitionTo( FileState::Unloaded );
		}
		break;

	case FileState::Unloaded:
	case FileState::Loaded:
		/* nothing to do */
		break;

	case FileState::Reload:
		if( FinishCanceledTransfer( bWait ) )
		{
			DeleteEverything();
			m_pStream = m_pPendingStream;
			m_pPendingStream = nullptr;
			StartTransfer( FileState::LoadHeader );
			TransitionTo( FileState::LoadHeader );
		}
		else
		{
			break;
		}

	case FileState::LoadHeader:
		if( FinishTransfer( FileState::LoadHeader, bWait ) )
		{
			XASSERT( m_oHeader.m_nMagic == RESOURCE_FILE_MAGIC );
			XASSERT( m_oHeader.m_nVersion == RESOURCE_FILE_VERSION );

			StartTransfer( FileState::LoadSectionHeaders );
			TransitionTo( FileState::LoadSectionHeaders );
		}
		else
		{
			break;
		}

	case FileState::LoadSectionHeaders:
		if( FinishTransfer( FileState::LoadSectionHeaders, bWait ) )
		{
			StartTransfer( FileState::LoadFixupData );
			TransitionTo( FileState::LoadFixupData );

			/// Transition auto-load sections to PENDING
			for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
			{
				SectionHeader * pSection = m_pSections + i;

				if( (pSection->m_nFlags & SectionFlags::AutoLoad)
#if defined( _DEBUG )
					|| (pSection->m_nFlags & SectionFlags::AutoLoadDebug)
#endif
					)
				{
					TransitionSectionTo( pSection, SectionState::Pending );
				}
			}
		}
		else
		{
			break;
		}

	case FileState::LoadFixupData:
		if( FinishTransfer( FileState::LoadFixupData, bWait ) )
		{
			if( StartPendingSectionTransfer() )
			{
				TransitionTo( FileState::LoadSections );
			}
			else
			{
				TransitionTo( FileState::Loaded );
				break;
			}
		}
		else
		{
			break;
		}

	case FileState::LoadSections:
		do
		{
			if( FinishSectionTransfer( bWait ) )
			{
				SectionHeader * pSection = m_pLoadingSection;

				bool bMorePendingSections = (pSection->m_nState == SectionState::Reload) || StartPendingSectionTransfer();

				switch( pSection->m_nState )
				{
				case SectionState::Unload:
					DeleteSection( pSection );
					TransitionSectionTo( pSection, SectionState::Unloaded );
					break;

				case SectionState::Load:
					/* TODO: Decompress section */
					TransitionSectionTo( pSection, SectionState::Loaded );
					break;

				case SectionState::Reload:
					StartSectionTransfer( pSection, false );
					TransitionSectionTo( pSection, SectionState::Load );
					break;

				case SectionState::Unloaded:
				case SectionState::Loaded:
				case SectionState::Pending:
					XASSERT_MSG( 0, "XResourceReader: Invalid state for the currently loading section. How did this happen?" );
					break;

				default:
					XASSERT(0);
				}

				if( !bMorePendingSections )
				{
					m_pLoadingSection = nullptr;
					DoFixup();
					TransitionTo( FileState::Loaded );
					break;
				}
			}
			else
			{
				break;
			}
		} while( bWait );
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::LoadAllSections()
{
	switch( m_nState )
	{
	case FileState::Unload:
	case FileState::Unloaded:
	case FileState::LoadHeader:
	case FileState::LoadSectionHeaders:
	case FileState::Reload:
		XASSERT_MSG( 0, "XResourceReader: Can't load sections in this state." );
		break;

	case FileState::Loaded:
		for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;
			if( pSection->m_nState != SectionState::Loaded )
			{
				XASSERT_MSG( pSection->m_nState == SectionState::Unloaded, "XResourceReader: File state is \"Loaded\" but section state is neither \"Loaded\" nor \"Unloaded\". How did this happen?" );
				TransitionSectionTo( pSection, SectionState::Pending );
			}
		}
		if( StartPendingSectionTransfer() )
		{
			TransitionTo( FileState::LoadSections );
		}
		break;

	case FileState::LoadFixupData:
		for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;
			XASSERT_MSG( (pSection->m_nState == SectionState::Unloaded) || (pSection->m_nState == SectionState::Pending), "XResourceReader: File state is \"LoadFixupData\" but section state is neither \"Unloaded\" nor \"Pending\". How did this happen?" );
			TransitionSectionTo( pSection, SectionState::Pending );
		}
		break;

	case FileState::LoadSections:
		for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;

			switch( pSection->m_nState )
			{
			case SectionState::Unload:
				TransitionSectionTo( pSection, SectionState::Reload );
				break;

			case SectionState::Unloaded:
				TransitionSectionTo( pSection, SectionState::Pending );
				break;

			case SectionState::Loaded:
			case SectionState::Pending:
			case SectionState::Load:
			case SectionState::Reload:
				break;

			default:
				XASSERT(0);
			}
		}
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

void XEngine::XResourceReader::UnloadSections( unsigned nFlags )
{
	switch( m_nState )
	{
	case FileState::Unload:
	case FileState::Unloaded:
		break;

	case FileState::LoadHeader:
	case FileState::LoadSectionHeaders:
	case FileState::Reload:
		XASSERT_MSG( 0, "XResourceReader: Can't unload sections in this state." );
		break;

	case FileState::Loaded:
		for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;
			if( (nFlags != 0) && (~pSection->m_nFlags & nFlags) )
			{
				continue;
			}
			if( pSection->m_nState != SectionState::Unloaded )
			{
				XASSERT_MSG( pSection->m_nState == SectionState::Loaded, "XResourceReader: File state is \"Loaded\" but section state is neither \"Loaded\" nor \"Unloaded\". How did this happen?" );
				DeleteSection( pSection );
				TransitionSectionTo( pSection, SectionState::Unloaded );
			}
		}
		DoFixup();
		break;

	case FileState::LoadFixupData:
		for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;
			XASSERT_MSG( (pSection->m_nState == SectionState::Unloaded) || (pSection->m_nState == SectionState::Pending), "XResourceReader: File state is \"LoadFixupData\" but section state is neither \"Unloaded\" nor \"Pending\". How did this happen?" );
			if( (nFlags != 0) && (~pSection->m_nFlags & nFlags) )
			{
				continue;
			}
			TransitionSectionTo( pSection, SectionState::Unloaded );
		}
		break;

	case FileState::LoadSections:
		for( unsigned i = 0; i < m_oHeader.m_nNumSections; ++i )
		{
			SectionHeader * pSection = m_pSections + i;

			if( (nFlags != 0) && (~pSection->m_nFlags & nFlags) )
			{
				continue;
			}

			switch( pSection->m_nState )
			{
			case SectionState::Unload:
			case SectionState::Unloaded:
				break;

			case SectionState::Loaded:
				DeleteSection( pSection );
				TransitionSectionTo( pSection, SectionState::Unloaded );
				break;

			case SectionState::Pending:
				TransitionSectionTo( pSection, SectionState::Unloaded );
				break;

			case SectionState::Load:
				CancelTransfer();
				/// Fall through...

			case SectionState::Reload:
				TransitionSectionTo( pSection, SectionState::Unload );
				break;

			default:
				XASSERT(0);
			}
		}
		break;

	default:
		XASSERT(0);
	}
}
