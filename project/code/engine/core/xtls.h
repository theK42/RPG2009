
#pragma once

#if defined(TARGET_MSWIN)

namespace XEngine
{

	class XThreadLocalVariable
	{
	public:
		inline XThreadLocalVariable()
			: m_nTlsIndex( ~0u )
		{
		}

		void	Alloc();
		void	Free();

		void *	Get() const;
		void	Set( void * pValue );

	private:
		u32		m_nTlsIndex;
	};

	template< typename T >
	class XThreadLocalVariableWrapper : public XThreadLocalVariable
	{
	public:
		inline XThreadLocalVariableWrapper()
		{
		}

		inline operator T() const
		{
			void * pValue = this->Get();
			return reinterpret_cast<T>( pValue );
		}

		inline XThreadLocalVariableWrapper<T> & operator =( T const & rOther )
		{
			void const * pValue = reinterpret_cast<void const *>( rOther );
			this->Set( const_cast<void *>( pValue ) );
			return (*this);
		}

		inline XThreadLocalVariableWrapper<T> & operator =( XThreadLocalVariableWrapper<T> const & rOther )
		{
			this->Set( rOther.Get() );
			return (*this);
		}

	private:
		XThreadLocalVariableWrapper( XThreadLocalVariableWrapper<T> const & );
	};

}

#	define XTLS_INIT(var)			((var).Alloc())
#	define XTLS_DEINIT(var)			((var).Free())
#	define XTLS_DECLARE(type)		XEngine::XThreadLocalVariableWrapper< type >

#else

#	define XTLS_INIT(var)			((void)sizeof(var))
#	define XTLS_DEINIT(var)			((void)sizeof(var))
#	define XTLS_DECLARE(type)		XTHREAD_LOCAL type

#endif
