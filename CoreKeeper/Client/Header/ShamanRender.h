#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;

END

class CShamanRender : public CGameObject
{
	explicit CShamanRender(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShamanRender();

public:
	virtual			HRESULT			Ready_GameObject(_int iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_MonsterType() { return MON_SHAMAN; }

private:
	HRESULT			Add_Component();

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static	CShamanRender* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);

private:
	virtual void	Free();
};

