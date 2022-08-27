
#include <math.h>

#include <gsl/gsl_poly.h>
#include <gsl/gsl_integration.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

#include "engine/core/xcore.h"

using std::uint32_t;
using XEngine::nullptr;

struct GlyphResult
{
	double		m_fLeft;
	double		m_fBottom;
	double		m_fWidth;
	double		m_fHeight;
	int			m_nGridWidth;
	int			m_nGridHeight;
	double *	m_pSignedDistances;
	double		(* m_pClosestEdgePoints)[2];
};

struct GlyphProcessingContext
{
	double							m_pPos[2];
	double							m_fLeft;
	double							m_fBottom;
	double							m_fWidth;
	double							m_fHeight;
	int								m_nGridWidth;
	int								m_nGridHeight;
	double *						m_pSquareDistances;
	double *						m_pClassifications;
	double							(* m_pClosestEdgePoints)[2];
	gsl_poly_complex_workspace *	m_pPolynomialWorkspace;
	gsl_integration_workspace *		m_pIntegrationWorkspace;
	bool							m_bReverseFill;
};

static int ReducePoly( double const * pX, int nX, double Epsilon )
{
	for (int i = nX; i > 1; --i)
	{
		if (fabs( pX[ i - 1 ] ) > Epsilon)
		{
			return i;
		}
	}
	return 1;
}

static int AddPoly( double * pResult, int nResult, double const * pX, int nX, double const * pY, int nY )
{
	int nOut = (nY > nX) ? nY : nX;
	XASSERT( nResult >= nOut );

	int nMin = (nY < nX) ? nY : nX;
	for (int i = 0; i < nMin; ++i)
	{

		pResult[ i ] = pX[ i ] + pY[ i ];
	}

	for (int i = nMin; i < nX; ++i)
	{
		pResult[ i ] = pX[ i ];
	}

	for (int i = nMin; i < nY; ++i)
	{
		pResult[ i ] = pY[ i ];
	}

	return nOut;
}

static int MulPoly( double * pResult, int nResult, double const * pX, int nX, double const * pY, int nY )
{
	int nOut = (nX == 0 || nY == 0) ? 0 : 1 + (nX - 1) + (nY - 1);
	XASSERT( nResult >= nOut );

	memset( pResult, 0, nOut * sizeof(double) );

	for (int i = 0; i < nX; ++i)
	{
		for (int j = 0; j < nY; ++j)
		{
			pResult[ i + j ] += pX[ i ] * pY[ j ];
		}
	}

	return nOut;
}

static int DifPoly( double * pResult, int nResult, double const * pX, int nX )
{
	int nOut = nX - 1;
	XASSERT( nResult >= nOut );

	for (int i = 0; i < nOut; ++i)
	{
		pResult[ i ] = (i + 1) * pX[ i + 1 ];
	}

	return nOut;
}

static double FindMinimum( double const * p, int n, gsl_poly_complex_workspace * w, double sLeft, double sRight, double * pSMin, double Epsilon )
{
	double sMin = sLeft;
	double fMin = gsl_poly_eval( p, n, sMin );

	double s = sRight;
	double f = gsl_poly_eval( p, n, s );
	sMin = (f < fMin) ? s : sMin;
	fMin = (f < fMin) ? f : fMin;

	double dp[ 6 ];
	int ndp = DifPoly( dp, 6, p, n );

	double pExtrema[10];
	int nExtrema;

	switch (ndp)
	{
	case 2:
		pExtrema[0] = -dp[0] / dp[1];
		pExtrema[1] = 0.0;
		nExtrema = 1;
		break;

	case 3:
		nExtrema = gsl_poly_solve_quadratic( dp[2], dp[1], dp[0], &pExtrema[0], &pExtrema[2] );
		pExtrema[1] = 0.0;
		pExtrema[3] = 0.0;
		break;

	case 4:
		nExtrema = gsl_poly_solve_cubic( dp[2] / dp[3], dp[1] / dp[3], dp[0] / dp[3], &pExtrema[0], &pExtrema[2], &pExtrema[4] );
		pExtrema[1] = 0.0;
		pExtrema[3] = 0.0;
		pExtrema[5] = 0.0;
		break;

	default:
		w->nc = ndp - 1;
		//w = gsl_poly_complex_workspace_alloc( ndp );
		gsl_poly_complex_solve( dp, ndp, w, pExtrema );
		//gsl_poly_complex_workspace_free( w );
		nExtrema = ndp - 1;
	}

	for (int i = 0; i < nExtrema; ++i)
	{
		if (fabs(pExtrema[ (i << 1) | 1 ]) > Epsilon)
		{
			/// Complex root
			continue;
		}

		s = pExtrema[ i << 1 ];
		if ((s < sLeft) || (s > sRight))
		{
			/// Outside interval
			continue;
		}

		f = gsl_poly_eval( p, n, s );
		sMin = (f < fMin) ? s : sMin;
		fMin = (f < fMin) ? f : fMin;
	}

	if (pSMin != nullptr)
	{
		*pSMin = sMin;
	}

	return fMin;
}

int gsl_poly_eval_derivs( double const c[], size_t lenc, double const x, double res[], size_t lenres )
{
	for (size_t i = 0; i < lenres && i < lenc; ++i)
	{
		res[i] = 0.0;
		for (size_t n = lenc; n-- != i; /*nothing*/)
		{
			double w = 1.0;
			for (size_t j = 0; j < i; ++j)
			{
				w *= n - j;
			}
			res[i] = w * c[n] + x * res[i];
		}
	}
	for (size_t i = lenc; i < lenres; ++i)
	{
		res[i] = 0.0;
	}
	return 0;
}

struct IntegrandParam
{
	double m_fX;
	double m_fY;
	double const * m_pX;
	double const * m_pY;
	int m_nX;
	int m_nY;
};

double __cdecl Integrand( double s, void * pParam )
{
	IntegrandParam * p = reinterpret_cast<IntegrandParam *>( pParam );

	double X[2], Y[2];
	gsl_poly_eval_derivs( p->m_pX, p->m_nX, s, X, 2 );
	gsl_poly_eval_derivs( p->m_pY, p->m_nY, s, Y, 2 );

	double RX = X[0] - p->m_fX;
	double RY = Y[0] - p->m_fY;

	double num = X[1] * RY - Y[1] * RX;
	double den = sqrt( RX * RX + RY * RY );

	return num / (den * den * den);
}

static double const Epsilon = 1.E-8;

static void Update( GlyphProcessingContext * pContext, double const * pX, int nX, double const * pY, int nY )
{
	double pRX[4], pRY[4];
	double pXX[7], pYY[7];

	double fCellWidth = pContext->m_fWidth / pContext->m_nGridWidth;
	double fCellHeight = pContext->m_fHeight / pContext->m_nGridHeight;

	for (int iRow = 0; iRow < pContext->m_nGridHeight; ++iRow)
	{
		int nRowIndex = iRow * pContext->m_nGridWidth;

		double y = pContext->m_fBottom + fCellHeight * (iRow + 0.5);
		pRY[0] = -y;
		int nRY = AddPoly( pRY, 4, pRY, 1, pY, nY );
		int nYY = MulPoly( pYY, 7, pRY, nRY, pRY, nRY );

		for (int iCol = 0; iCol < pContext->m_nGridWidth; ++iCol)
		{
			int nIndex = nRowIndex + iCol;

			double x = pContext->m_fLeft + fCellWidth * (iCol + 0.5);
			pRX[0] = -x;
			int nRX = AddPoly( pRX, 4, pRX, 1, pX, nX );
			int nXX = MulPoly( pXX, 7, pRX, nRX, pRX, nRX );
			nXX = AddPoly( pXX, 7, pXX, nXX, pYY, nYY );

			double sMin;
			double fMin = FindMinimum( pXX, nXX, pContext->m_pPolynomialWorkspace, 0.0, 1.0, &sMin, Epsilon );

			if (fMin < fabs(pContext->m_pSquareDistances[ nIndex ]))
			{
				pContext->m_pSquareDistances[ nIndex ] = fMin;
				pContext->m_pClosestEdgePoints[ nIndex ][ 0 ] = gsl_poly_eval( pX, nX, sMin );
				pContext->m_pClosestEdgePoints[ nIndex ][ 1 ] = gsl_poly_eval( pY, nY, sMin );
			}

			IntegrandParam oParam = { x, y, pX, pY, nX, nY };
			gsl_function f = { &Integrand, &oParam };
			double fClassification, fError;
			gsl_integration_qags( &f, 0.0, 1.0, Epsilon, 0.0, 1024, pContext->m_pIntegrationWorkspace, &fClassification, &fError );
			pContext->m_pClassifications[ nIndex ] += fClassification;
		}
	}
}

static double const ONE_64TH = 0.015625;

static int __cdecl MoveFunc( FT_Vector const * pEnd, void * pParam )
{
	GlyphProcessingContext * pContext = reinterpret_cast<GlyphProcessingContext *>( pParam );
	pContext->m_pPos[ 0 ] = pEnd->x * ONE_64TH;
	pContext->m_pPos[ 1 ] = pEnd->y * ONE_64TH;
	return 0;
}

static int __cdecl LineFunc( FT_Vector const * pEnd, void * pParam )
{
	GlyphProcessingContext * pContext = reinterpret_cast<GlyphProcessingContext *>( pParam );

	double const (& P0)[2] = pContext->m_pPos;
	double const P1[2] = { pEnd->x * ONE_64TH, pEnd->y * ONE_64TH };

	double const QX[2] = { P0[0], P1[0] - P0[0] };
	double const QY[2] = { P0[1], P1[1] - P0[1] };

	Update( pContext, QX, ReducePoly( QX, 2, Epsilon ), QY, ReducePoly( QY, 2, Epsilon ) );

	pContext->m_pPos[ 0 ] = P1[ 0 ];
	pContext->m_pPos[ 1 ] = P1[ 1 ];

	return 0;
}

static int __cdecl ConicFunc( FT_Vector const * pCtrl, FT_Vector const * pEnd, void * pParam )
{
	GlyphProcessingContext * pContext = reinterpret_cast<GlyphProcessingContext *>( pParam );

	double const (& P0)[2] = pContext->m_pPos;
	double const P1[2] = { pCtrl->x * ONE_64TH, pCtrl->y * ONE_64TH };
	double const P2[2] = { pEnd->x * ONE_64TH, pEnd->y * ONE_64TH };

	double const QX[3] = { P0[0], 2.0 * (P1[0] - P0[0]), P2[0] + P0[0] - 2.0 * P1[0] };
	double const QY[3] = { P0[1], 2.0 * (P1[1] - P0[1]), P2[1] + P0[1] - 2.0 * P1[1] };

	Update( pContext, QX, ReducePoly( QX, 3, Epsilon ), QY, ReducePoly( QY, 3, Epsilon ) );

	pContext->m_pPos[ 0 ] = P2[ 0 ];
	pContext->m_pPos[ 1 ] = P2[ 1 ];

	return 0;
}

static int __cdecl CubicFunc( FT_Vector const * pCtrl1, FT_Vector const * pCtrl2, FT_Vector const * pEnd, void * pParam )
{
	GlyphProcessingContext * pContext = reinterpret_cast<GlyphProcessingContext *>( pParam );

	double const (& P0)[2] = pContext->m_pPos;
	double const P1[2] = { pCtrl1->x * ONE_64TH, pCtrl1->y * ONE_64TH };
	double const P2[2] = { pCtrl2->x * ONE_64TH, pCtrl2->y * ONE_64TH };
	double const P3[2] = { pEnd->x * ONE_64TH, pEnd->y * ONE_64TH };

	double const QX[4] = { P0[0], 3.0 * (P1[0] - P0[0]), 3.0 * (P2[0] + P0[0] - 2.0 * P1[0]), (P3[0] - P0[0]) - 3.0 * (P2[0] - P1[0]) };
	double const QY[4] = { P0[1], 3.0 * (P1[1] - P0[1]), 3.0 * (P2[1] + P0[1] - 2.0 * P1[1]), (P3[1] - P0[1]) - 3.0 * (P2[1] - P1[1]) };

	Update( pContext, QX, ReducePoly( QX, 4, Epsilon ), QY, ReducePoly( QY, 4, Epsilon ) );

	pContext->m_pPos[ 0 ] = P3[ 0 ];
	pContext->m_pPos[ 1 ] = P3[ 1 ];

	return 0;
}

static void InitGlyphProcessing( GlyphProcessingContext * pContext, int nGridWidth, int nGridHeight )
{
	int nSize = nGridWidth * nGridHeight;
	pContext->m_nGridWidth = nGridWidth;
	pContext->m_nGridHeight = nGridHeight;
	pContext->m_pSquareDistances = new double[ nSize ];
	pContext->m_pClassifications = new double[ nSize ];
	pContext->m_pClosestEdgePoints = new double[ nSize ][ 2 ];
	for (int i = 0; i < nSize; ++i)
	{
		pContext->m_pSquareDistances[ i ] = nSize;
		pContext->m_pClassifications[ i ] = 0.0;
	}
}

void ProcessGlyph( GlyphResult * pResult, FT_Face pFace, uint32_t nCharCode )
{
	int nResult;

	nResult = FT_Load_Char( pFace, nCharCode, FT_LOAD_NO_BITMAP );
	XASSERT( nResult == 0 );

	FT_GlyphSlot pGlyphSlot = pFace->glyph;
	FT_Glyph pGlyph;

	nResult = FT_Get_Glyph( pGlyphSlot, &pGlyph );
	XASSERT( nResult == 0 );

	FT_BBox oCBox;
	FT_Glyph_Get_CBox( pGlyph, FT_GLYPH_BBOX_PIXELS, &oCBox );

	FT_Done_Glyph( pGlyph );

	int nGridWidth = 2 + oCBox.xMax - oCBox.xMin;
	int nGridHeight = 2 + oCBox.yMax - oCBox.yMin;

	GlyphProcessingContext oContext;
	InitGlyphProcessing( &oContext, nGridWidth, nGridHeight );
	oContext.m_fLeft = oCBox.xMin - 1.0;
	oContext.m_fBottom = oCBox.yMin - 1.0;
	oContext.m_fWidth = nGridWidth;
	oContext.m_fHeight = nGridHeight;
	oContext.m_pPolynomialWorkspace = gsl_poly_complex_workspace_alloc( 7 );
	oContext.m_pIntegrationWorkspace = gsl_integration_workspace_alloc( 1024 );
	oContext.m_bReverseFill = (pGlyphSlot->outline.flags & FT_OUTLINE_REVERSE_FILL) != 0;

	FT_Outline_Funcs oFuncs =
	{
		&MoveFunc,
		&LineFunc,
		&ConicFunc,
		&CubicFunc
	};

	nResult = FT_Outline_Decompose( &pGlyphSlot->outline, &oFuncs, &oContext );
	XASSERT( nResult == 0 );

	gsl_poly_complex_workspace_free( oContext.m_pPolynomialWorkspace );
	gsl_integration_workspace_free( oContext.m_pIntegrationWorkspace );

	int nSize = oContext.m_nGridWidth * oContext.m_nGridHeight;

	XASSERT( pResult != nullptr );
	pResult->m_fLeft = oContext.m_fLeft;
	pResult->m_fBottom = oContext.m_fBottom;
	pResult->m_fWidth = oContext.m_fWidth;
	pResult->m_fHeight = oContext.m_fHeight;
	pResult->m_nGridWidth = oContext.m_nGridWidth;
	pResult->m_nGridHeight = oContext.m_nGridHeight;
	pResult->m_pSignedDistances = new double[ nSize ];
	pResult->m_pClosestEdgePoints = oContext.m_pClosestEdgePoints;

	for (int i = 0; i < nSize; ++i)
	{
		double fDistance = sqrt( oContext.m_pSquareDistances[ i ] );
		pResult->m_pSignedDistances[ i ] = oContext.m_pClassifications[ i ] < Epsilon ? -fDistance : fDistance;
	}

	delete [] oContext.m_pSquareDistances;
	delete [] oContext.m_pClassifications;
}

void DeinitGlyphResult( GlyphResult * pGlyphResult )
{
	delete [] pGlyphResult->m_pSignedDistances;
	delete [] pGlyphResult->m_pClosestEdgePoints;
}

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <gl/gl.h>

#include "windows/core/window.h"

using namespace Windows;

class MyWindow : public CWindow
{
public:
	MyWindow( GlyphResult const * pGlyph );
	~MyWindow();

protected:
		virtual void	OnCreate();
		virtual void	OnRender();
		virtual void	OnResize( CDimensions const & rDimensions );

private:
	float		(* m_pVerts)[2];
	uint32_t *	m_pColors;
	int			m_nVerts;
	float		m_fLeft, m_fRight;
	float		m_fBottom, m_fTop;
};

MyWindow::MyWindow( GlyphResult const * pGlyph )
{
	int nVerts = m_nVerts = 2 * pGlyph->m_nGridWidth * pGlyph->m_nGridHeight;
	float (* pVerts)[2] = m_pVerts = new float[ nVerts ][ 2 ];
	uint32_t * pColors = m_pColors = new uint32_t[ nVerts ];

	double fPixelWidth = pGlyph->m_fWidth / pGlyph->m_nGridWidth;
	double fPixelHeight = pGlyph->m_fHeight / pGlyph->m_nGridHeight;
	for (int iRow = 0; iRow < pGlyph->m_nGridHeight; ++iRow)
	{
		int nRowIndex = iRow * pGlyph->m_nGridWidth;
		double y = pGlyph->m_fBottom + fPixelHeight * (iRow + 0.5);
		for (int iCol = 0; iCol < pGlyph->m_nGridWidth; ++iCol)
		{
			int nIndex = nRowIndex + iCol;
			double x = pGlyph->m_fLeft + fPixelWidth * (iCol + 0.5);
			uint32_t nColor = (pGlyph->m_pSignedDistances[ nIndex ] < 0.0) ? 0xFF000000 : 0xFFFFFFFF;

			pVerts[0][0] = (float)x;
			pVerts[0][1] = (float)y;
			pVerts[1][0] = (float)pGlyph->m_pClosestEdgePoints[ nIndex ][ 0 ];
			pVerts[1][1] = (float)pGlyph->m_pClosestEdgePoints[ nIndex ][ 1 ];
			pVerts += 2;

			pColors[0] = nColor;
			pColors[1] = nColor;
			pColors += 2;
		}
	}

	m_fLeft = (float)pGlyph->m_fLeft;
	m_fRight = (float)(pGlyph->m_fLeft + pGlyph->m_fWidth);
	m_fBottom = (float)pGlyph->m_fBottom;
	m_fTop = (float)(pGlyph->m_fBottom + pGlyph->m_fHeight);
}

MyWindow::~MyWindow()
{
	delete [] m_pVerts;
	delete [] m_pColors;
}

void MyWindow::OnCreate()
{
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
}

void MyWindow::OnResize( CDimensions const & rDimensions )
{
	glViewport( 0, 0, rDimensions.GetWidth(), rDimensions.GetHeight() );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( m_fLeft, m_fRight, m_fBottom, m_fTop, -1.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
}

void MyWindow::OnRender()
{
	glClear( GL_COLOR_BUFFER_BIT );

	glVertexPointer(2, GL_FLOAT, 8, m_pVerts);
	glColorPointer(4, GL_UNSIGNED_BYTE, 4, m_pColors);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_LINES, 0, m_nVerts);
}

int main()
{
	int nResult;

	FT_Library pLibrary;
	nResult = FT_Init_FreeType( &pLibrary );
	XASSERT( nResult == 0 );

	FT_Face pFace;
	nResult = FT_New_Face( pLibrary, "C:\\Windows\\Fonts\\ARIAL.TTF", 0, &pFace );
	XASSERT( nResult == 0 );

	nResult = FT_Set_Pixel_Sizes( pFace, 32, 32 );
	XASSERT( nResult == 0 );

	GlyphResult oResult;
	ProcessGlyph( &oResult, pFace, 'B' );

	FT_Done_Face( pFace );
	FT_Done_FreeType( pLibrary );

	MyWindow oWindow( &oResult );
	oWindow.Create( CDimensions(24*oResult.m_nGridWidth, 24*oResult.m_nGridHeight) );
	DeinitGlyphResult( &oResult );

	nResult = oWindow.DoMessageLoop();
	oWindow.Destroy();

	return nResult;
}
