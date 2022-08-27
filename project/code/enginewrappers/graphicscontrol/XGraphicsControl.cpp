// XGraphicsControl.cpp : main project file.

#include "stdafx.h"
#include "XGraphicsControl.h"




void XWEngine::XGraphicsControl::OnHandleCreated( EventArgs^ e )
{
	HWND hWnd = (HWND)Handle.ToPointer();

	HDC hDC = GetDC( hWnd );

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize		= sizeof( pfd );
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA;

	int iPixelFormat = ChoosePixelFormat( hDC, &pfd );
	SetPixelFormat( hDC, iPixelFormat, &pfd );

	HGLRC hGLRC = wglCreateContext( hDC );

	m_hWnd	= hWnd;
	m_hDC	= hDC;
	m_hGLRC	= hGLRC;

	wglMakeCurrent( m_hDC, m_hGLRC );
	Control::OnHandleCreated(e);
	wglMakeCurrent( NULL, NULL );
}

void XWEngine::XGraphicsControl::OnHandleDestroyed( EventArgs^ e )
{
	wglMakeCurrent( m_hDC, m_hGLRC );
	Control::OnHandleDestroyed(e);
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( m_hGLRC );
	ReleaseDC( m_hWnd, m_hDC );	
}

void XWEngine::XGraphicsControl::OnPaint( PaintEventArgs^ e )
{
	wglMakeCurrent( m_hDC, m_hGLRC );
	Control::OnPaint(e);
	wglMakeCurrent( NULL, NULL );
	SwapBuffers( m_hDC );
}

void XWEngine::XGraphicsControl::OnResize( EventArgs^ e )
{
	wglMakeCurrent( m_hDC, m_hGLRC );
	Control::OnResize(e);
	wglMakeCurrent( NULL, NULL );
}