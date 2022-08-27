
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <GL/gl.h>
#include "window.h"

void Windows::CWindow::Create( CDimensions const & rDimensions )
{
	HINSTANCE hCurrentProcess = (HINSTANCE)GetCurrentProcess();

	WNDCLASS wc =
	{
		CS_OWNDC | CS_DBLCLKS,
		&WindowProc,
		0, 0,
		hCurrentProcess,
		LoadIcon( NULL, IDI_APPLICATION ),
		LoadCursor( NULL, IDC_ARROW ),
		NULL,
		NULL,
		"XGLWINDOW"
	};

	ATOM nClass = RegisterClass( &wc );

	DWORD dwWindowStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);

	RECT rcWindow = { 0, 0, rDimensions.GetWidth(), rDimensions.GetHeight() };
	AdjustWindowRect( &rcWindow, dwWindowStyle, FALSE );

	int nWidth = rcWindow.right - rcWindow.left;
	int nHeight = rcWindow.bottom - rcWindow.top;

	HWND hWnd = CreateWindow(
		(LPCSTR)nClass,
		"",
		dwWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nWidth,
		nHeight,
		HWND_DESKTOP,
		NULL,
		hCurrentProcess,
		this );

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
	OnCreate();
	wglMakeCurrent( NULL, NULL );

	ShowWindow( hWnd, SW_SHOW );
}

void Windows::CWindow::Destroy()
{
	wglDeleteContext( m_hGLRC );
	ReleaseDC( m_hWnd, m_hDC );
	DestroyWindow( m_hWnd );
}

void Windows::CWindow::CaptureMouse()
{
	SetCapture( m_hWnd );
}

void Windows::CWindow::ReleaseMouse()
{
	ReleaseCapture();
}

int Windows::CWindow::DoMessageLoop()
{
	LARGE_INTEGER oTimerFrequency, oLastFrame, oThisFrame;
	QueryPerformanceFrequency( &oTimerFrequency );
	QueryPerformanceCounter( &oLastFrame );

	float fSecsPerTick = 1.0f / oTimerFrequency.QuadPart;

	while( true )
	{
		MSG msg;
		for( int nSanity = 100; nSanity && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ); --nSanity )
		{
			if( msg.message == WM_QUIT )
			{
				return (int)msg.wParam;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		QueryPerformanceCounter( &oThisFrame );
		wglMakeCurrent( m_hDC, m_hGLRC );
		OnUpdate( fSecsPerTick * (oThisFrame.QuadPart - oLastFrame.QuadPart) );
		OnRender();
		wglMakeCurrent( NULL, NULL );
		SwapBuffers( m_hDC );
		oLastFrame = oThisFrame;
	}
}

LRESULT CALLBACK Windows::CWindow::WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CWindow * pthis = (CWindow*)(LONG_PTR) GetWindowLongPtr( hWnd, GWLP_USERDATA );

	switch( message )
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT pCreate = (LPCREATESTRUCT)lParam;
			pthis = (CWindow*)pCreate->lpCreateParams;
			SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pthis );
		}
		break;

	case WM_SIZE:
		{
			CDimensions oDims( LOWORD( lParam ), HIWORD( lParam ) );
			wglMakeCurrent( pthis->m_hDC, pthis->m_hGLRC );
			pthis->OnResize( oDims );
			wglMakeCurrent( NULL, NULL );
		}
		break;

	case WM_PAINT:
		ValidateRect( hWnd, NULL );
		break;

	case WM_MOUSEMOVE:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnMouseMove( oPoint );
		}
		break;

	case WM_LBUTTONDOWN:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnMouseDown( oPoint, 0 );
		}
		break;

	case WM_RBUTTONDOWN:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnMouseDown( oPoint, 1 );
		}
		break;

	case WM_LBUTTONUP:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnMouseUp( oPoint, 0 );
		}
		break;

	case WM_RBUTTONUP:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnMouseUp( oPoint, 1 );
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnDoubleClick( oPoint, 0 );
		}
		break;

	case WM_RBUTTONDBLCLK:
		{
			CPoint oPoint( LOWORD( lParam ), HIWORD( lParam ) );
			pthis->OnDoubleClick( oPoint, 1 );
		}
		break;


	case WM_CLOSE:
		wglMakeCurrent( pthis->m_hDC, pthis->m_hGLRC );
		pthis->OnDestroy();
		wglMakeCurrent( NULL, NULL );
		PostQuitMessage( 0 );
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}
