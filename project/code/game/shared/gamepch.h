#pragma once
#include "engine/core/xcore.h"
#include "engine/core/xtypes.h"
#include "engine/core/xassert.h"
#include "engine/core/xallocator.h"
#include "engine/algorithms/xchecksum.h"
#include "engine/math/xconstants.h"
#include "engine/math/xvector.h"
#include "engine/math/xmatrix.h"
#include "engine/algorithms/xutil.h"

using XEngine::s8;
using XEngine::s16;
using XEngine::s32;
using XEngine::s64;
using XEngine::saddr;

using XEngine::u8;
using XEngine::u16;
using XEngine::u32;
using XEngine::u64;
using XEngine::uaddr;

using XEngine::uint;
using XEngine::handle;

#ifndef _MANAGED
using XEngine::nullptr;
#endif

using XEngine::XChecksum;

using XEngine::PI;

using XEngine::IDENTITY;

using XEngine::ZERO;
using XEngine::UNIT_X;
using XEngine::UNIT_Y;
using XEngine::UNIT_Z;
using XEngine::UNIT_W;



using XEngine::Min;
using XEngine::Max;
using XEngine::Clamp;
using XEngine::IsBounded;

using XEngine::XVector4;
using XEngine::XMatrix44;