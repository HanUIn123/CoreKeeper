#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CStatueCore : public CItem
{
private:
	explicit CStatueCore(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStatueCore();

public:
	virtual			HRESULT			Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CStatueCore* Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eItemNum, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();
};
