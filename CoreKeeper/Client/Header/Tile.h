#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Export_System.h"


BEGIN(Engine)

class CTransform;
class CTexture;
class CCalculator;
class CTileTex;
class CMapToolTex;

END

class CTile : public Engine::CGameObject
{
private:
	explicit															CTile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual																~CTile();

public:
	virtual			HRESULT												Ready_GameObject(_float _fTileX, _float _fTileZ);
	virtual			_int												Update_GameObject(const _float& fTimeDelta);
	virtual			void												LateUpdate_GameObject();
	virtual			void												Render_GameObject();

	_int																Get_TileNumber() { return m_iTileImageNum; }
	void																Set_TileNumber(_int _iTileNum) { m_iTileImageNum = _iTileNum; }

	_vec3																Get_TilePos() { return m_vTilePosition; }
	void																Set_TilePos(_vec3 _iTilePos) { m_vTilePosition = _iTilePos; }

	_vec3																Get_PikingPos() {return Picking_OnTile();}

private:
	HRESULT																Add_Component();
	_vec3																Picking_OnTile();
private:
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;
	Engine::CTileTex* m_pTileTexCom;
	Engine::CMapToolTex* m_pMapToolBufferCom;


public:
	static CTile*														Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fTileX, _float _fTileZ);

private:
	virtual void														Free();

	_vec3																m_vTilePosition;
	_int																m_iTileImageNum;

	


};


