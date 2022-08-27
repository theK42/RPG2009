
#pragma once

#include "engine/tools/resource/xmemio.h"
#include "engine/tools/stl/xstlvector.h"
#include "engine/tools/stl/xstlhashmap.h"

namespace XEngine
{

	class XAllocator;

	namespace XTools
	{

		struct XResourceSectionFlags
		{
			enum Type
			{
				Discardable		= 1,
				Compressed		= 2,
				AutoLoad		= 4,
				AutoLoadDebug	= 8,
			};
		};

		/// XResourceFileWriter

		/// Resource files have sections. Most XResourceFileWriter functions
		/// operate on the section that is currently open. Initially section
		/// 0 is open; other sections can become the open section by calling
		/// OpenSection() or OpenNextSection().

		/// How linking works:

		/// WriteLink(h) makes note of a link from the current reference
		/// position to a target that is "named" by h. It then writes a null
		/// pointer to the output. The link target may or may not have been
		/// resolved at this time.

		/// CAUTION: There's no way to "delete" a link notation, so if you back
		/// up and write over a link with other data (perhaps by using Seek),
		/// the location where the link was previously written will be treated
		/// as a pointer and will be fixed up at runtime. Don't do this! It's
		/// OK to write over a link with another link, though, as long as
		/// they're written at the exact same location (which they should be
		/// if they overlap; otherwise one of them wasn't aligned properly).

		/// ResolveLink(h) resolves the target named by h to the current
		/// reference position.

		/// When the resource file is finalized, fixup data is generated to
		/// point links at their referenced objects. All link targets must be
		/// resolved by this time. Links to targets that have not been resolved
		/// will not have fixup data generated for them and will be null at run
		/// time.

		/// Example:
		/// // Section 0
		/// // Write a link that's "named" by the address of the data
		/// oResourceFileWriter.WriteLink( pTexture->m_pTexels );
		/// ...
		/// // Section 1 (Discardable)
		/// // Resolve the link we wrote earlier and write the data
		/// oResourceFileWriter.ResolveLink( pTexture->m_pTexels );
		/// oResourceFileWriter.WriteBytes( pTexture->m_nTexelSize, pTexture->m_pTexels );
		/// ...
		/// // Fixup data is generated here
		/// oResourceFileWriter.Finalize( oSerialWriter );

		class XResourceFileWriter
		{
		public:
			XResourceFileWriter();

			void	SetAllSectionFlags( unsigned nFlags, unsigned nMask );

			void	OpenSection( unsigned nSection );
			void	OpenNextSection();

			void	Finalize( XSeekIO & rStream );

			/// The rest of the public interface operates on the currently open section

			void	SetSectionFlags( unsigned nFlags );
			void	SetSectionAlignment( uaddr nAlignment );
			void	SetSectionMinAlignment( uaddr nAlignment );

			/// Sets the file's root object reference to the current reference location
			void	SetRootObjectReference();

			void	WriteBytes( uaddr nNumBytes, void const * pBuffer );

			void	WriteInt8( s8 n );
			void	WriteInt16( s16 n );
			void	WriteInt32( s32 n );
			void	WriteInt64( s64 n );

			void	WriteUInt8( u8 n );
			void	WriteUInt16( u16 n );
			void	WriteUInt32( u32 n );
			void	WriteUInt64( u64 n );

			void	WriteFloat( float f );
			void	WriteDouble( double d );

			void	WriteLink( handle h );
			void	ResolveLink( handle h );

			void	Align( uaddr nAlignment );

			void	Seek( uaddr nOffset );
			uaddr	Tell() const;

		private:
			void	CreateSectionIfNecessary();

		private:
			struct Reference
			{
				u32		m_nSection;
				u32		m_nOffset;
			};

			struct Header
			{
				u32			m_nMagic;
				u32			m_nVersion;
				u32			m_nFixupDataOffset;
				u32			m_nFixupDataSize;
				u32			m_nNumSections;
				Reference	m_oRootObjectReference;
			};

			struct SectionHeader
			{
				u32		m_nOffset;
				u32		m_nCompressedSize;
				u32		m_nSize;
				u32		m_nAlignment;
				u16		m_nFlags;
				s16		m_nState;
				union
				{
					void *	m_pData;
					uaddr	m_nDataAddress;
				};
			};

			struct FixupItem
			{
				u32			m_nOffset;
				Reference	m_oReferencedObject;
			};

			struct SectionState
			{
				enum Type
				{
					Unload		= -2,
					Unloaded	= -1,
					Loaded		= 0,
					Pending,
					Load,
					Reload,
				};
			};

		private:
			typedef XSTLHashMap< u32, handle >			SectionLinkMap;
			typedef XSTLHashMap< handle, Reference >	LinkTargetMap;
			typedef XSTLVector< SectionHeader >			SectionHeaderArray;
			typedef XSTLVector< XMemIO >				SectionDataArray;
			typedef XSTLVector< SectionLinkMap >		SectionLinkArray;

		private:
			LinkTargetMap		m_oLinkTargets;
			SectionHeaderArray	m_oSectionHeaders;
			SectionDataArray	m_oSectionData;
			SectionLinkArray	m_oSectionLinks;
			Reference			m_oRootObjectReference;
			unsigned			m_nDefaultSectionFlags;
			unsigned			m_nNumSections;
			unsigned			m_nOpenSection;
		};

	} /// namepsace XTools

} /// namespace XTools