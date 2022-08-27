#pragma once
#include "game/shared/practicallyuselessallocator.h"
#include "game/game/map.h"
#include "engine/graphics/xgraphics.h"
///Temp
#include "engine/graphics/xtexture.h"


namespace XEngine
{
	class XMatrix44; 
}

namespace erpg
{
	class CMap;

	class CGame
	{
	public:
		void Init();
		void Deinit();
		void Update(float fTime);
		void Render();

		///XLIKELY to be removed, give too much control to the GameWindow for my taste.
		void SetViewport(int nX0, int nY0, int nX1, int nY1);
		void SetViewMatrix(XEngine::XMatrix44 & mView);
		void SetLightPosition(float fX, float fY);
	private:
		CMap							m_oMap;
		CPracticallyUselessAllocator	m_oAllocator;
		XEngine::XGraphics				m_oGraphics;
		XEngine::XMatrix44				m_oViewMatrix;
		float							m_fLightX;
		float							m_fLightY;
	};
}