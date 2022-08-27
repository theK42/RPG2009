
#pragma once

namespace XEngine
{
	class XAsyncIO;
	class XAllocator;

	class XResourceReader
	{
	public:
		void	Init( XAllocator * pDefaultAllocator, XAllocator * pDiscardableAllocator = nullptr );
		void	Deinit();

		void	Load( XAsyncIO & rStream );
		void	Unload();

		void	LoadSection( unsigned nIndex );
		void	UnloadSection( unsigned nIndex );

		void	LoadAllSections();
		void	UnloadDiscardableSections();
		void	UnloadAllSections();

		bool	IsLoadComplete() const;	/// or unload
		void	WaitForLoad();			/// or unload

		void *	GetRootObject() const;

		void	Process();

	private:
		struct Reference
		{
			u32		m_nSection;
			u32		m_nOffset;
		};

		struct Header
		{
			u32			m_nMagic;
			u32			m_nVersion;
			u32			m_nFixupDataOffset;
			u32			m_nFixupDataSize;
			u32			m_nNumSections;
			Reference	m_oRootObjectReference;
		};

		struct SectionHeader
		{
			u32		m_nOffset;
			u32		m_nCompressedSize;
			u32		m_nSize;
			u32		m_nAlignment;
			u16		m_nFlags;
			s16		m_nState;
			union
			{
				void *	m_pData;
				uaddr	m_nDataAddress;
			};
		};

		struct FixupItem
		{
			u32			m_nOffset;
			Reference	m_oReferencedObject;
		};

	private:
		void	StartTransfer( int nStage );
		bool	FinishTransfer( int nStage, bool bWait );
		void	CancelTransfer();
		bool	FinishCanceledTransfer( bool bWait );
		void	TransitionTo( int nState );

		void	StartSectionTransfer( SectionHeader * pSection, bool bAllocate );
		bool	FinishSectionTransfer( bool bWait );
		bool	StartPendingSectionTransfer();
		void	DeleteSection( SectionHeader * pSection );
		void	TransitionSectionTo( SectionHeader * pSection, int nState );

		void	DeleteEverything();
		void	DoFixup();

		void	UnloadSections( unsigned nFlags );
		void	ProcessInternal( bool bWait );

		void *	ResolveReference( Reference const & rReference ) const;

	private:
		struct FileState
		{
			enum Type
			{
				Unload				= -2,
				Unloaded			= -1,
				Loaded				= 0,
				LoadHeader,
				LoadSectionHeaders,
				LoadFixupData,
				LoadSections,
				Reload,
			};
		};

		struct SectionState
		{
			enum Type
			{
				Unload		= -2,
				Unloaded	= -1,
				Loaded		= 0,
				Pending,
				Load,
				Reload,
			};
		};

		struct SectionFlags
		{
			enum Type
			{
				Discardable		= 1,
				Compressed		= 2,
				AutoLoad		= 4,
				AutoLoadDebug	= 8,
			};
		};

	protected:
		XAllocator *	m_pDefaultAllocator;
		XAllocator *	m_pDiscardableAllocator;
		XAsyncIO *		m_pStream;
		XAsyncIO *		m_pPendingStream;
		Header			m_oHeader;
		SectionHeader *	m_pSections;
		void *			m_pFixupData;
		uint			m_nState;
		handle			m_hTransfer;
		SectionHeader *	m_pLoadingSection;
	};

} /// namespace XEngine

inline void XEngine::XResourceReader::UnloadAllSections()
{
	UnloadSections( 0 );
}

inline void XEngine::XResourceReader::UnloadDiscardableSections()
{
	UnloadSections( SectionFlags::Discardable );
}

inline bool XEngine::XResourceReader::IsLoadComplete() const
{
	return (m_nState == FileState::Loaded) || (m_nState == FileState::Unloaded);
}

inline void XEngine::XResourceReader::WaitForLoad()
{
	ProcessInternal( true );
}

inline void XEngine::XResourceReader::Process()
{
	ProcessInternal( false );
}

inline void * XEngine::XResourceReader::ResolveReference( Reference const & rReference ) const
{
	SectionHeader * pSection = m_pSections + rReference.m_nSection;
	if( pSection->m_nState != SectionState::Loaded )
	{
		return nullptr;
	}
	return (void*)(pSection->m_nDataAddress + rReference.m_nOffset);
}

inline void * XEngine::XResourceReader::GetRootObject() const
{
	return (m_nState == FileState::Loaded) ? ResolveReference( m_oHeader.m_oRootObjectReference ) : nullptr;
}
