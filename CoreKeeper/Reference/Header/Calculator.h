#pragma once
#include "Component.h"

BEGIN(Engine)

class CMapToolTex;
class CTileTex;
class CTerrainTex;
class CTransform;

class ENGINE_DLL CCalculator :	public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCalculator();

public:
	HRESULT			Ready_Calculator();
	_vec3			Picking_OnTerrain(HWND _hWnd, CMapToolTex* _pMapToolBufferCom, CTransform* _pWireTerrainTransform);

public:
	static CCalculator*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CCalculator*	Clone();

private:
	virtual void Free();
};

END