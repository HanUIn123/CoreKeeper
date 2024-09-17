#pragma once
#include "GameObject.h"
#include "../Client/Header/ImguiMgr.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CMapToolTex;
class CTransform;
class CCalculator;

END

class CMapToolTerrain : public Engine::CGameObject
{
private:
	explicit											CMapToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual												~CMapToolTerrain();

public:
	virtual			HRESULT								Ready_GameObject();
	virtual			_int								Update_GameObject(const _float& fTimeDelta);
	virtual			void								LateUpdate_GameObject();
	virtual			void								Render_GameObject();

	_vec3												Get_PikingPos() { return Picking_OnTerrain(); }
	_int												Get_TerrainNumber() { return m_iTerrainImageNum; }
	void												Set_TerrainNumber(_int _iTerrainNum) { m_iTerrainImageNum = _iTerrainNum; }


private:
	HRESULT												Add_Component();
	HRESULT												Setup_Material();
	_vec3												Picking_OnTerrain();

private:
	Engine::CMapToolTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CMapToolTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void										Free();

	_vec3												vPickPos;
	_int												m_iTerrainImageNum;
};

