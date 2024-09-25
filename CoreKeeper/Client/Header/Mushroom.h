#pragma once
#include "Monster.h"
class CMushroom : public CMonster
{
	explicit CMushroom(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMushroom();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	STATE			State_Change();

public:
	static CMushroom* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

