#include "stdafx.h"
#include "enginewrappers/graphics/xwtexture.h"
#include "enginewrappers/graphics/xwgraphics.h"
#include "string.h"


///------------------------------------------------------------------------

XWEngine::XWTexture::XWTexture()
{
	m_pWrappedTexture = new XTexture;
	m_pTexels = nullptr;
	m_nTexelsSize = 0;
}

///------------------------------------------------------------------------

XWEngine::XWTexture::~XWTexture()
{
	XASSERT(m_pTexels = nullptr);
}

///------------------------------------------------------------------------

void XWEngine::XWTexture::Init( XWGraphics ^ rGraphics, XWTextureFlags eFlags, System::Drawing::Bitmap ^ rBitmap )
{
	int nWidth = rBitmap->Width;
	int nHeight = rBitmap->Height;

	XWTextureFormat eTextureFormat = XWTextureFormat::RGBA_8888;

	enum ERGBA
	{
		eRGBA_R,
		eRGBA_G,
		eRGBA_B,
		eRGBA_A,
		eRGBA_count
	};

	u8 *pTexels = new u8[nWidth * nHeight * eRGBA_count];
	m_pTexels = pTexels;
	m_nTexelsSize = sizeof(u8) * nWidth * nHeight * eRGBA_count;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			Color oTexelColor = rBitmap->GetPixel(j, i);
			pTexels[(i * nWidth * eRGBA_count) + (j * eRGBA_count) + eRGBA_R] = oTexelColor.R;
			pTexels[(i * nWidth * eRGBA_count) + (j * eRGBA_count) + eRGBA_G] = oTexelColor.G;
			pTexels[(i * nWidth * eRGBA_count) + (j * eRGBA_count) + eRGBA_B] = oTexelColor.B;
			pTexels[(i * nWidth * eRGBA_count) + (j * eRGBA_count) + eRGBA_A] = oTexelColor.A;
		}
	}
	m_pWrappedTexture->Init(rGraphics->GetWrappedGraphics(), (int)eTextureFormat, nWidth, nHeight, (int)eFlags, pTexels);
}

///------------------------------------------------------------------------

void XWEngine::XWTexture::Deinit( XWGraphics ^ rGraphics )
{
	m_pWrappedTexture->Deinit(rGraphics->GetWrappedGraphics());
}

///------------------------------------------------------------------------

void XWEngine::XWTexture::Select( XWGraphics ^ rGraphics, int nTextureIndex )
{
	m_pWrappedTexture->Select(rGraphics->GetWrappedGraphics(), nTextureIndex);
}

#pragma unmanaged
struct SMetaData
{
	u16	nFormat;
	u16	nFlags;
	u16	nWidth;
	u16 nHeight;
};
#pragma managed

///------------------------------------------------------------------------

array<u8> ^ XWEngine::XWTexture::Serialize()
{

	SMetaData oMetaData;
	oMetaData.nFormat = m_pWrappedTexture->GetFormat();
	oMetaData.nFlags = m_pWrappedTexture->GetFlags();
	oMetaData.nWidth = m_pWrappedTexture->GetWidth();
	oMetaData.nHeight = m_pWrappedTexture->GetHeight();

	size_t nTotalSize = m_nTexelsSize + sizeof(SMetaData);

	array<u8> ^ rReturnArray = gcnew array<u8>(nTotalSize);
	pin_ptr<u8> pPinnedArray = &rReturnArray[0];
	memcpy(pPinnedArray, &oMetaData, sizeof(SMetaData));
	memcpy(&pPinnedArray[sizeof(SMetaData)], m_pTexels, m_nTexelsSize);
	return rReturnArray;
}

///------------------------------------------------------------------------
