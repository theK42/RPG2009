#pragma once

#include "game/basegame/meshrenderer.h"

class CHexGrid {
public:
	CHexGrid();
	~CHexGrid();

	Init(int nRadius, float fRadius, CMeshRenderer * pRenderer);
	Deinit();

protected:
	CMeshGraphic oMesh;

	static const int				msc_nVertexWidth = 3;
	int								m_nVertexCount;
	float							m_pVertices[];
	int								m_nIndexCount;
	int								m_pIndices[];

};