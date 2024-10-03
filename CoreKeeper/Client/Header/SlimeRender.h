#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;

END

class CSlimeRender : public CGameObject
{
	explicit CSlimeRender(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSlimeRender();

public:
	virtual			HRESULT			Ready_GameObject(_int iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_MonsterType() { return MON_SLIME; }

private:
	HRESULT			Add_Component();

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static	CSlimeRender*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);

private:
	virtual void	Free();
};

