#pragma once
#include "Component.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;

class ENGINE_DLL CCalculator :	public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCalculator();

public:
	HRESULT			Ready_Calculator();

public:
	static CCalculator*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CCalculator*	Clone();

private:
	virtual void Free();
};

END