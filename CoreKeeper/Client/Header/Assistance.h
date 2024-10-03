#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CAssistance : public CItem
{
private:
	explicit CAssistance(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAssistance();

public:
	virtual			HRESULT			Ready_GameObject(ASSISTANCE _eAssistance, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();
	
private:
	HRESULT			Add_Component();

public:
	static CAssistance* Create(LPDIRECT3DDEVICE9 pGraphicDev, ASSISTANCE _eAssistance, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();

private:
	ASSISTANCE			m_eAssistance;
};
