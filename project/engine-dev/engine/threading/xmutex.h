
#pragma once

#if defined( TARGET_MSWIN )
#	include "engine/core/xmswin.h"
#endif

#if defined( TARGET_POSIX )
#	include "engine/core/xposix.h"
#	include <pthread.h>
#endif

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XMutex
	{
	public:
		void Init();
		void Deinit();

		void Lock();
		void Unlock();

	private:

#if defined( TARGET_MSWIN )
		CRITICAL_SECTION	m_oCriticalSection;
#endif

#if defined( TARGET_POSIX )
		pthread_mutex_t		m_oMutex;
#endif

	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine
