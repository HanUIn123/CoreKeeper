#pragma once
#include "Monster.h"

BEGIN(Engine)

class CSmoke;
class CHit;

END

class CShroomMan : public CMonster
{
	explicit CShroomMan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShroomMan();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

	void			Pattern_Idle(const _float& fTimeDelta) override;
	void			Pattern_Chase(const _float& fTimeDelta) override;
	void			Pattern_Attack(const _float& fTimeDelta) override;
	void			Pattern_Dead() override;
	STATE			State_Change() override;

public:
	static CShroomMan* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();

private:
	Engine::CSmoke* m_pSmokeParticleCom;

	_vec3 vPrePos;
};

