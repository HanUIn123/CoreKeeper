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
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTile();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 _tilePos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT							Add_Component();
	_vec3							Picking_OnTerrain();
private:
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;
	Engine::CTileTex* m_pTileTexCom;
	Engine::CMapToolTex* m_pMapToolBufferCom;


public:
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vTilePos);

private:
	virtual void		Free();



	_vec3				m_vTilePosition;
};


