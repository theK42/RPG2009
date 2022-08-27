#include "stdafx.h"
#include "enginewrappers/graphics/xwcolor.h"
#pragma unmanaged
#include "engine/graphics/xcolor.h"
#pragma managed 

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWColor::XWColor()
{
	m_pWrappedColor = new XEngine::XColor;
}

///----------------------------------------------------------------------------

XWEngine::XWColor::XWColor( XEngine::u32 nPacked )
{
	m_pWrappedColor = new XEngine::XColor(nPacked);
}

///----------------------------------------------------------------------------

XWEngine::XWColor::XWColor( float r, float g, float b, float a )
{
	m_pWrappedColor = new XEngine::XColor(r, g, b, a);
}

///----------------------------------------------------------------------------

XWEngine::XWColor::~XWColor()
{
	delete m_pWrappedColor;
	m_pWrappedColor = nullptr;
}

///----------------------------------------------------------------------------

void XWEngine::XWColor::Set( float r, float g, float b, float a )
{
	m_pWrappedColor->Set(r, g, b, a);
}

///----------------------------------------------------------------------------

float XWEngine::XWColor::Red::get()
{
	return m_pWrappedColor->GetRed();
}

///----------------------------------------------------------------------------

float XWEngine::XWColor::Green::get()
{
	return m_pWrappedColor->GetGreen();
}

///----------------------------------------------------------------------------

float XWEngine::XWColor::Blue::get()
{
	return m_pWrappedColor->GetBlue();
}

///----------------------------------------------------------------------------

float XWEngine::XWColor::Alpha::get()
{
	return m_pWrappedColor->GetAlpha();
}
///----------------------------------------------------------------------------

XEngine::u32 XWEngine::XWColor::Packed::get()
{
	return m_pWrappedColor->GetPacked();
}

///----------------------------------------------------------------------------

void XWEngine::XWColor::Packed::set(XEngine::u32 nPacked)
{
	return m_pWrappedColor->SetPacked(nPacked);
}