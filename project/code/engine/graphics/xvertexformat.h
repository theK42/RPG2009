
#pragma once

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XVertexAttributeId
	{
		enum Type
		{
			Position,
			Normal,
			Color,
			TexCoord0,
			TexCoord1,
			Weights,
			MatrixIndices,

			Max
		};
	};

	///------------------------------------------------------------------------

	struct XVertexAttributeMask
	{
		enum Type
		{
			Position		= (1 << XVertexAttributeId::Position),
			Normal			= (1 << XVertexAttributeId::Normal),
			Color			= (1 << XVertexAttributeId::Color),
			TexCoord0		= (1 << XVertexAttributeId::TexCoord0),
			TexCoord1		= (1 << XVertexAttributeId::TexCoord1),
			Weights			= (1 << XVertexAttributeId::Weights),
			MatrixIndices	= (1 << XVertexAttributeId::MatrixIndices),
		};
	};

	///------------------------------------------------------------------------

	struct XVertexAttributeType
	{
		enum Type
		{
			S8,
			U8,
			S16,
			U16,
			S32,
			U32,
			F32,

			Max
		};
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XVertexFormat
	{
	public:
		XVertexFormat();

		unsigned	GetNumAttributes() const;
		unsigned	GetVertexStride() const;

		int			GetAttributeId( unsigned nAttribute ) const;
		int			GetAttributeType( unsigned nAttribute ) const;
		unsigned	GetNumComponents( unsigned nAttribute ) const;
		unsigned	GetOffset( unsigned nAttribute ) const;

		void		Add( int nAttributeId, int nType, unsigned nNumComponents, unsigned nOffset = 0 );

	private:
		struct Attribute
		{
			u8		m_nAttributeId;
			u8		m_nType;
			u8		m_nNumComponents;
			u8		m_nOffset;
		};

		u8			m_nNumAttributes;
		u8			m_nVertexAlignment;
		u16			m_nVertexStride;
		Attribute	m_pAttributes[ XVertexAttributeId::Max ];
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::XVertexFormat::XVertexFormat()
{
	m_nNumAttributes	= 0;
	m_nVertexAlignment	= 1;
	m_nVertexStride		= 0;
}

///----------------------------------------------------------------------------

inline unsigned XEngine::XVertexFormat::GetNumAttributes() const
{
	return m_nNumAttributes;
}

///----------------------------------------------------------------------------

inline unsigned XEngine::XVertexFormat::GetVertexStride() const
{
	return m_nVertexStride;
}

///----------------------------------------------------------------------------

inline int XEngine::XVertexFormat::GetAttributeId( unsigned nAttribute ) const
{
	return m_pAttributes[nAttribute].m_nAttributeId;
}

///----------------------------------------------------------------------------

inline int XEngine::XVertexFormat::GetAttributeType( unsigned nAttribute ) const
{
	return m_pAttributes[nAttribute].m_nType;
}

///----------------------------------------------------------------------------

inline unsigned XEngine::XVertexFormat::GetNumComponents( unsigned nAttribute ) const
{
	return m_pAttributes[nAttribute].m_nNumComponents;
}

///----------------------------------------------------------------------------

inline unsigned XEngine::XVertexFormat::GetOffset( unsigned nAttribute ) const
{
	return m_pAttributes[nAttribute].m_nOffset;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
