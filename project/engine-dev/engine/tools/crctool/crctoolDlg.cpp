// crctoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crctool.h"
#include "crctoolDlg.h"

#include "engine/core/xcore.h"
#include "engine/algorithms/xchecksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CcrctoolDlg dialog




CcrctoolDlg::CcrctoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CcrctoolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcrctoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcrctoolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CcrctoolDlg message handlers

BOOL CcrctoolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CcrctoolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CcrctoolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void StripCarriageReturns( CString & szText )
{
	CString szOut = "";

	char c;
	for( int i = 0; (c = szText[i]) != '\0'; ++i )
	{
		if( (c == '\r') && (szText[i+1] == '\n') )
		{
			++i;
			szOut += '\n';
		}
		else
		{
			szOut += c;
		}
	}

	szOut += '\0';
	szText = szOut;
}

void StripSlashes( CString & szText )
{
	CString szOut = "";

	char c;
	for( int i = 0; (c = szText[i]) != '\0'; ++i )
	{
		if( c == '\\' )
		{
			switch( c = szText[++i] )
			{
			case 'a':
				szOut += '\a';
				break;
			case 'b':
				szOut += '\b';
				break;
			case 'f':
				szOut += '\f';
				break;
			case 'n':
				szOut += '\n';
				break;
			case 'r':
				szOut += '\r';
				break;
			case 't':
				szOut += '\t';
				break;
			case 'v':
				szOut += '\v';
				break;
			case '\'':
				szOut += '\'';
				break;
			case '\"':
				szOut += '\"';
				break;
			case '\\':
				szOut += '\\';
				break;
			case '\?':
				szOut += '\?';
				break;
			case '\n':
				szOut += '\n';
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				/// octal
				{
					char n = c;
					for(int j = 0; j < 3; ++j )
					{
						switch( c = szText[++i] )
						{
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							n = (n * 8) + (c - '0');
							break;

						default:
							--i;
							goto OCTAL_END;
						}
					}
OCTAL_END:
					szOut += n;
				}
				break;
			case 'x':
				/// hex
				{
					char n = 0;
					for( int j = 0; j < 2; ++j )
					{
						switch( c = szText[++i] )
						{
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							n = (n * 16) + (c - '0');
							break;

						case 'A':
						case 'B':
						case 'C':
						case 'D':
						case 'E':
						case 'F':
							n = (n * 16) + (c - 'A');
							break;

						case 'a':
						case 'b':
						case 'c':
						case 'd':
						case 'e':
						case 'f':
							n = (n * 16) + (c - 'a');
							break;

						default:
							--i;
							goto HEX_END;
						}
HEX_END:
						szOut += n;
					}
				}
				break;
			default:
				szOut += c;
			}
		}
		else
		{
			szOut += c;
		}
	}

	szOut += '\0';
	szText = szOut;
}

void AddSlashes( CString & szText )
{
	CString szOut = "";

	char c;
	for( int i = 0; (c = szText[i]) != '\0'; ++i )
	{
		switch( c )
		{
		case '\a':
			szOut += "\\a";
			break;
		case '\b':
			szOut += "\\b";
			break;
		case '\f':
			szOut += "\\f";
			break;
		case '\n':
			szOut += "\\n";
			break;
		case '\r':
			szOut += "\\r";
			break;
		case '\t':
			szOut += "\\t";
			break;
		case '\v':
			szOut += "\\v";
			break;
		case '\'':
			szOut += "\\\'";
			break;
		case '\"':
			szOut += "\\\"";
			break;
		case '\\':
			szOut += "\\\\";
			break;
		case '\?':
			szOut += "\\\?";
		default:
			if( isprint( (unsigned char) c ) )
			{
				szOut += c;
			}
			else
			{
				szOut += '\\';
				szOut += (char)('0' + ((c / 64) % 8));
				szOut += (char)('0' + ((c / 8) % 8));
				szOut += (char)('0' + (c % 8));
			}
		}
	}

	szOut += '\0';
	szText = szOut;
}

void CcrctoolDlg::OnOK()
{
	using namespace XEngine;

	CString szText;
	GetDlgItem( IDC_EDIT1 )->GetWindowText( szText );
	StripCarriageReturns( szText );
	StripSlashes( szText );
	XChecksum nChecksum = XChecksum_CalculateFromString( szText.GetBuffer() );
	AddSlashes( szText );

	CString szClipboard;
	szClipboard.Format( "CRC( 0x%08X, \"%s\" )", nChecksum, szText.GetBuffer() );

	size_t nClipboardSize = 1 + szClipboard.GetLength();
	HGLOBAL hClipboardMem = GlobalAlloc( GMEM_MOVEABLE, szClipboard.GetLength() + 1 );
	LPVOID pLockedMem = GlobalLock( hClipboardMem );
	memcpy( pLockedMem, szClipboard.GetBuffer(), nClipboardSize );
	GlobalUnlock( hClipboardMem );

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData( CF_TEXT, hClipboardMem );
	CloseClipboard();
}
