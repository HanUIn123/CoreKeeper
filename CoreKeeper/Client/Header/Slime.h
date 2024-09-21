#pragma once
#include "Monster.h"

class CSlime : public CMonster
{
	explicit CSlime(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSlime();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	STATE			State_Change();

public:
	static CSlime* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

