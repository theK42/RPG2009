
#include "engine/core/xcore.h"
#include "engine/resource/xresdb.h"

int XEngine::XResourceDatabase::FindFirstObjectIndexByType( u32 nType ) const
{
	if( XUNLIKELY( m_nNumObjects == 0 ) )
	{
		return -1;
	}

	ResourceObject const * pLo = m_pObjects;
	ResourceObject const * pHi = m_pObjects + m_nNumObjects;

	while( pLo < pHi )
	{
		ResourceObject const * pMid = pLo + ((pHi - pLo) >> 1);

		if( pMid->m_nType >= nType )
		{
			pHi = pMid;
		}
		else
		{
			pLo = pMid + 1;
		}
	}

	return XLIKELY( pLo->m_nType == nType ) ? (int)(pLo - m_pObjects) : -1;
}

int XEngine::XResourceDatabase::FindObjectIndexByNameAndType( u32 nNameCRC, u32 nType ) const
{
	ResourceObject const * pLo = m_pObjects;
	ResourceObject const * pHi = m_pObjects + m_nNumObjects;

	while( pLo < pHi )
	{
		ResourceObject const * pMid = pLo + ((pHi - pLo) >> 1);

		if( (pMid->m_nType > nType) || ((pMid->m_nType == nType) && (pMid->m_nNameCRC > nNameCRC)) )
		{
			pHi = pMid;
		}
		else if( (pMid->m_nType < nType) || ((pMid->m_nType == nType) && (pMid->m_nNameCRC < nNameCRC)) )
		{
			pLo = pMid + 1;
		}
		else
		{
			return (int)(pMid - m_pObjects);
		}
	}

	return -1;
}
