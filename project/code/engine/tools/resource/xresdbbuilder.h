
#pragma once

#include "engine/tools/resource/xresfilewriter.h"
#include "engine/tools/stl/xstlvector.h"
#include "engine/tools/stl/xstlhashmap.h"
#include "engine/algorithms/xchecksum.h"

namespace XEngine
{
	namespace XTools
	{
		class XResourceDatabaseBuilder;

		/// The string that is referenced by the pointer returned from
		/// XResourceDatabaseBuilderObject must remain valid and unchanged
		/// from XResourceDatabaseBuilder's AddObject through Finalize().

		/// The XResourceDatabaseBuilderObject is responsible for setting the
		/// reference location for the packaged object in the output resource
		/// file. Usually implementations should do this with a line of code
		/// like this inside the Write() method:
		///		rWriter.ResolveLink( hLink );

		class XResourceDatabaseBuilderObject
		{
		public:
			virtual char const *	GetName() = 0;
			virtual XChecksum		GetNameCRC() = 0;
			virtual u32				GetType() = 0;

			virtual void			Write( XResourceDatabaseBuilder & rBuilder, XResourceFileWriter & rWriter, handle hLink ) = 0;
		};

		class XResourceDatabaseBuilderNamedObject : public XResourceDatabaseBuilderObject
		{
		public:
			void					SetName( char const * pName );
			virtual char const *	GetName();
			virtual XChecksum		GetNameCRC();

		private:
			static int const MAX_NAME_SIZE		= 256;
			static int const MAX_NAME_LENGTH	= MAX_NAME_SIZE - 1;

			XChecksum	m_nNameCRC;
			char		m_pName[ MAX_NAME_SIZE ];
		};

		class XResourceDatabaseBuilder
		{
		public:
			void	AddObject( XResourceDatabaseBuilderObject * pObject );
			void	Finalize( XSeekIO & rStream, bool bWriteStringTable );

		public:
			static int const MAIN_SECTION			= 0;
			static int const DISCARDABLE_SECTION	= 1;
			static int const DEBUG_SECTION			= 2;

			handle	MapString( char const * pString );

		private:
			typedef XSTLVector< XResourceDatabaseBuilderObject * >	ObjectList;
			typedef XSTLHashMap< char const *, handle >				StringTable;

		private:
			XResourceFileWriter		m_oWriter;
			ObjectList				m_oObjects;
			StringTable				m_oStringTable;
		};

	} /// namespace XTools

} /// namespace XEngine

inline void XEngine::XTools::XResourceDatabaseBuilder::AddObject( XResourceDatabaseBuilderObject * pObject )
{
	m_oObjects.push_back( pObject );
}

inline char const * XEngine::XTools::XResourceDatabaseBuilderNamedObject::GetName()
{
	return m_pName;
}

inline XEngine::XChecksum XEngine::XTools::XResourceDatabaseBuilderNamedObject::GetNameCRC()
{
	return XChecksum_CalculateFromString(m_pName);
}
