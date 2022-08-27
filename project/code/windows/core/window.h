
#pragma once

namespace Windows
{

	class CPoint
	{
	public:
		CPoint();
		CPoint( int x, int y );

		int	GetX() const;
		int GetY() const;

	private:
		int	m_nX;
		int	m_nY;
	};

	class CDimensions
	{
	public:
		CDimensions();
		CDimensions( int nWidth, int nHeight );

		int	GetWidth() const;
		int	GetHeight() const;

	private:
		int	m_nWidth;
		int	m_nHeight;
	};

	class CWindow
	{
	public:
		void	Create( CDimensions const & rDimensions );
		void	Destroy();
		int		DoMessageLoop();

	protected:
		virtual void	OnCreate();
		virtual void	OnDestroy();
		virtual void	OnUpdate( float fDT );
		virtual void	OnRender();
		virtual void	OnResize( CDimensions const & rDimensions );
		virtual void	OnMouseMove( CPoint const & rPoint );
		virtual void	OnMouseDown( CPoint const & rPoint, int nButton );
		virtual void	OnMouseUp( CPoint const & rPoint, int nButton );
		virtual void	OnDoubleClick( CPoint const & rPoint, int nButton );

	protected:
		void	CaptureMouse();
		void	ReleaseMouse();

	private:
		static LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	private:
		HWND	m_hWnd;
		HDC		m_hDC;
		HGLRC	m_hGLRC;
	};

} /// namespace Windows

inline Windows::CPoint::CPoint()
{
}

inline Windows::CPoint::CPoint( int x, int y )
: m_nX( x ), m_nY( y )
{
}

inline int Windows::CPoint::GetX() const
{
	return m_nX;
}

inline int Windows::CPoint::GetY() const
{
	return m_nY;
}

inline Windows::CDimensions::CDimensions()
{
}

inline Windows::CDimensions::CDimensions( int nWidth, int nHeight )
: m_nWidth( nWidth ), m_nHeight( nHeight )
{
}

inline int Windows::CDimensions::GetWidth() const
{
	return m_nWidth;
}

inline int Windows::CDimensions::GetHeight() const
{
	return m_nHeight;
}

inline void Windows::CWindow::OnCreate()
{
}

inline void Windows::CWindow::OnDestroy()
{
}

inline void Windows::CWindow::OnUpdate( float fDT )
{
}

inline void Windows::CWindow::OnRender()
{
}

inline void Windows::CWindow::OnResize( CDimensions const & rDimensions )
{
}

inline void Windows::CWindow::OnMouseMove( CPoint const & rPoint )
{
}

inline void Windows::CWindow::OnMouseDown( CPoint const & rPoint, int nButton )
{
}

inline void Windows::CWindow::OnMouseUp( CPoint const & rPoint, int nButton )
{
}

inline void Windows::CWindow::OnDoubleClick( CPoint const & rPoint, int nButton )
{
}