#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
//class CCalculator;
//class CAnimator;

END

class CUICraft : public Engine::CGameObject
{
private:
	explicit CUICraft(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUICraft();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, _bool _bLRCheck);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(TABLETYPE _eType = TABLE_PLAYER, MATERIAL _eMaterial = MATERIAL_WOOD);

private:
    _int            Check_Index();
private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;
	_bool m_bWindow;

	_bool m_bLRCheck;

	MATERIAL eTableMaterial;
	TABLETYPE m_eTableType;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRcTex* m_pTitleBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pTitleTextureCom;

public:
	static CUICraft* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _bool _bLRCheck);

private:
	virtual void		Free();
};
