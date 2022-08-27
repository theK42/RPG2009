#include "stdafx.h"
#include "enginewrappers/resource/xwfileio.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWHandle::XWHandle(XEngine::handle hHandle)
{
	m_hInternalHandle = hHandle;
}

handle XWEngine::XWHandle::Get()
{
	return m_hInternalHandle;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWAsyncFile::XWAsyncFile()
{
	m_pWrappedFile = new XAsyncFile;
}

///----------------------------------------------------------------------------

XWEngine::XWAsyncFile::~XWAsyncFile()
{
	delete m_pWrappedFile;
	m_pWrappedFile = nullptr;
}

///----------------------------------------------------------------------------

void XWEngine::XWAsyncFile::Close()
{
	m_pWrappedFile->Close();
}

///----------------------------------------------------------------------------

XWEngine::XWHandle ^ XWEngine::XWAsyncFile::BeginAsyncRead( u64 nOffset, uaddr nSize, array<u8>^ pBuffer )
{
	pin_ptr<u8> pPinnedBuffer = &pBuffer[0];
	return gcnew XWHandle(m_pWrappedFile->BeginAsyncRead(nOffset, nSize, pPinnedBuffer));
}

///----------------------------------------------------------------------------

XWEngine::XWHandle ^ XWEngine::XWAsyncFile::BeginAsyncWrite( u64 nOffset, uaddr nSize, array<const u8>^ pBuffer )
{
	pin_ptr<u8 const> pPinnedBuffer = &pBuffer[0];
	return gcnew XWHandle(m_pWrappedFile->BeginAsyncWrite(nOffset, nSize, pPinnedBuffer));
}

///----------------------------------------------------------------------------

void XWEngine::XWAsyncFile::CancelAsyncTransfer( XWEngine::XWHandle^ hAsyncTransfer )
{
	m_pWrappedFile->CancelAsyncTransfer(hAsyncTransfer->Get());
}

///----------------------------------------------------------------------------

bool XWEngine::XWAsyncFile::IsAsyncTransferComplete( XWEngine::XWHandle^ hAsyncTransfer )
{
	return m_pWrappedFile->IsAsyncTransferComplete(hAsyncTransfer->Get());
}

///----------------------------------------------------------------------------

uaddr XWEngine::XWAsyncFile::FinishAsyncTransfer( XWEngine::XWHandle^ hAsyncTransfer )
{
	return m_pWrappedFile->FinishAsyncTransfer(hAsyncTransfer->Get());
}

///----------------------------------------------------------------------------

void XWEngine::XWAsyncFile::InitModule()
{
	XEngine::XAsyncFileDescriptorIO::InitModule();
}

///----------------------------------------------------------------------------

void XWEngine::XWAsyncFile::DeinitModule()
{
	XEngine::XAsyncFileDescriptorIO::DeinitModule();
}

///----------------------------------------------------------------------------

void XWEngine::XWAsyncFile::Open( System::String^ sFilename, XWFileFlags nFlags, XWFileAccessMode nMode)
{
	char * pFileName = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(sFilename).ToPointer();
	m_pWrappedFile->Open(pFileName, (int)nFlags, (int)nMode);
	System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)pFileName);
}

///----------------------------------------------------------------------------

void XWEngine::XWAsyncFile::Open( System::String^ sFilename, XWFileFlags nFlags)
{
	Open(sFilename, nFlags, XWFileAccessMode::Default);
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWSyncFile::XWSyncFile()
{
	m_pWrappedFile = new XSyncFile;
}

///----------------------------------------------------------------------------

XWEngine::XWSyncFile::~XWSyncFile()
{
	delete m_pWrappedFile;
	m_pWrappedFile = nullptr;
}

///----------------------------------------------------------------------------

void XWEngine::XWSyncFile::Close()
{
	m_pWrappedFile->Close();
}

///----------------------------------------------------------------------------

uaddr XWEngine::XWSyncFile::Read( uaddr nSize, array<u8>^ pBuffer )
{
	pin_ptr<u8> pPinnedBuffer = &pBuffer[0];
	return m_pWrappedFile->Read(nSize, pPinnedBuffer);
}

///----------------------------------------------------------------------------

uaddr XWEngine::XWSyncFile::Write(uaddr nSize, array<const u8>^ pBuffer )
{
	pin_ptr<u8 const> pPinnedBuffer = &pBuffer[0];
	return m_pWrappedFile->Write(nSize, pPinnedBuffer);
}

///----------------------------------------------------------------------------

void XWEngine::XWSyncFile::Open( System::String^ sFilename, XWFileFlags nFlags, XWFileAccessMode nMode)
{
	char * pFileName = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(sFilename).ToPointer();
	m_pWrappedFile->Open(pFileName, (int)nFlags, (int)nMode);
	System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)pFileName);
}

///----------------------------------------------------------------------------

void XWEngine::XWSyncFile::Open( System::String^ sFilename, XWFileFlags nFlags)
{
	Open(sFilename, nFlags, XWFileAccessMode::Default);
}

///----------------------------------------------------------------------------

XSyncFile * XWEngine::XWSyncFile::GetWrappedSyncFile()
{
	return m_pWrappedFile;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------