
#pragma once

namespace XEngine
{
	class XResourceDatabase
	{
	public:
		int		FindFirstObjectIndexByType( u32 nType ) const;
		int		FindObjectIndexByNameAndType( u32 nNameCRC, u32 nType ) const;

		void *	GetObjectByIndex( unsigned nIndex ) const;
		u32		GetObjectTypeByIndex( unsigned nIndex ) const;
		u32		GetObjectNameCRCByIndex( unsigned nIndex ) const;

		void *	FindFirstObjectByType( u32 nType ) const;
		void *	FindObjectByNameAndType( u32 nNameCRC, u32 nType ) const;

	protected:
		struct ResourceObject
		{
			u32				m_nType;
			u32				m_nNameCRC;
			char const *	m_pName;
			void *			m_pData;
		};

		u32					m_nMagic;
		u32					m_nVersion;
		u32					m_nNumObjects;
		ResourceObject *	m_pObjects;
	};

} /// namespace XEngine

inline void * XEngine::XResourceDatabase::GetObjectByIndex( unsigned nIndex ) const
{
	XASSERT( nIndex < m_nNumObjects );
	return m_pObjects[ nIndex ].m_pData;
}

inline XEngine::u32 XEngine::XResourceDatabase::GetObjectNameCRCByIndex( unsigned nIndex ) const
{
	XASSERT( nIndex < m_nNumObjects );
	return m_pObjects[ nIndex ].m_nNameCRC;
}

inline XEngine::u32 XEngine::XResourceDatabase::GetObjectTypeByIndex( unsigned nIndex ) const
{
	XASSERT( nIndex < m_nNumObjects );
	return m_pObjects[ nIndex ].m_nType;
}

inline void * XEngine::XResourceDatabase::FindFirstObjectByType( u32 nType ) const
{
	int nIndex = FindFirstObjectIndexByType( nType );
	return XUNLIKELY( nIndex < 0 ) ? nullptr : GetObjectByIndex( nIndex );
}

inline void * XEngine::XResourceDatabase::FindObjectByNameAndType( u32 nNameCRC, u32 nType ) const
{
	int nIndex = FindObjectIndexByNameAndType( nNameCRC, nType );
	return XUNLIKELY( nIndex < 0 ) ? nullptr : GetObjectByIndex( nIndex );
}
