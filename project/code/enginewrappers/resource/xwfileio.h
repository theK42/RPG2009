#pragma once
#pragma unmanaged
#include "engine/core/xcore.h"
#include "engine/resource/xfileio.h"
#pragma managed 

using namespace XEngine;

namespace XWEngine
{
	///This class belongs in core but I haven't made that project yet.
	public ref class XWHandle
	{
	public:
		XWHandle(handle hHandle);
		handle Get();
	protected:
		handle m_hInternalHandle;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------
	
	public enum class XWFileFlags
	{
		Read			= XFileFlags::Read,
		Write			= XFileFlags::Write,
		ReadWrite		= XFileFlags::ReadWrite,
		ReadWriteMask	= XFileFlags::ReadWriteMask,
		Truncate		= XFileFlags::Truncate,
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------
	
	public enum class XWFileAccessMode
	{
		Read			= XFileAccessMode::Read,
		Write			= XFileAccessMode::Write,
		Execute			= XFileAccessMode::Execute,

		OwnerShift		= XFileAccessMode::OwnerShift,
		GroupShift		= XFileAccessMode::GroupShift,
		AnonShift		= XFileAccessMode::AnonShift,

		OwnerRead		= XFileAccessMode::OwnerRead,
		OwnerWrite		= XFileAccessMode::OwnerWrite,
		OwnerExecute	= XFileAccessMode::OwnerExecute,

		GroupRead		= XFileAccessMode::GroupRead,
		GroupWrite		= XFileAccessMode::GroupWrite,
		GroupExecute	= XFileAccessMode::GroupExecute,

		AnonRead		= XFileAccessMode::AnonRead,
		AnonWrite		= XFileAccessMode::AnonWrite,
		AnonExecute		= XFileAccessMode::AnonExecute,

		AllRead			= XFileAccessMode::AllRead,
		AllWrite		= XFileAccessMode::AllWrite,
		AllExecute		= XFileAccessMode::AllExecute,

		Default			= XFileAccessMode::Default,
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	public ref class XWAsyncFile
	{
	public:
		XWAsyncFile();
		~XWAsyncFile();
		void	Open( System::String^ sFilename, XWFileFlags nFlags, XWFileAccessMode nMode /* = 0644 */);
		void	Open( System::String^ sFilename, XWFileFlags nFlags );		
		void	Close();

		XWHandle ^	BeginAsyncRead( u64 nOffset, uaddr nSize, array<u8>^ pBuffer );
		XWHandle ^	BeginAsyncWrite( u64 nOffset, uaddr nSize, array<const u8>^ pBuffer );

		void	CancelAsyncTransfer( XWHandle^ hAsyncTransfer );
		bool	IsAsyncTransferComplete( XWHandle^ hAsyncTransfer );
		uaddr	FinishAsyncTransfer( XWHandle^ hAsyncTransfer );

		static void	InitModule();
		static void DeinitModule();
	private:
		XAsyncFile * m_pWrappedFile;
	};


	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	public ref class XWSyncFile
	{
	public:
		XWSyncFile();
		~XWSyncFile();
		void	Open( System::String^ sFilename, XWFileFlags nFlags, XWFileAccessMode nMode /* = 0644 */);
		void	Open( System::String^ sFilename, XWFileFlags nFlags );		
		void	Close();

		uaddr	Read( uaddr nSize, array<u8>^ pBuffer );
		uaddr	Write( uaddr nSize, array<const u8>^ pBuffer );

		XSyncFile * GetWrappedSyncFile();

	private:
		XSyncFile * m_pWrappedFile;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine
