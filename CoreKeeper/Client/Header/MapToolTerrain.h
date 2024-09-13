#pragma once
#include "GameObject.h"
#include "../Client/Header/ImguiMgr.h"

BEGIN(Engine)

class CMapToolTex;
//class CTerrainTex;
class CTransform;
class CTexture;
class CCalculator;

END

class CMapToolTerrain : public Engine::CGameObject
{
private:
	explicit													CMapToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual														~CMapToolTerrain();

public:
	virtual			HRESULT										Ready_GameObject();
	virtual			_int										Update_GameObject(const _float& fTimeDelta);
	virtual			void										LateUpdate_GameObject();
	virtual			void										Render_GameObject();

	_int														Get_TileNumber() { return m_iTileImageNum; }
	void														Set_TileNumber(_int _iTileNum) { m_iTileImageNum = _iTileNum; }

private:
	HRESULT														Add_Component();
	HRESULT														Setup_Material();
	_vec3														Picking_OnTerrain();

private:
	Engine::CMapToolTex* m_pBufferCom;
	//Engine::CTerrainTex*										m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CMapToolTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	//void														Switch_Terrain();

private:
	virtual void												Free();
	//bool														bSwitch;

	_vec3* m_pPos;
	_vec3														vPickPos;
	_int														m_iTileImageNum;
};

