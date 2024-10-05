#pragma once
#include "Monster.h"

BEGIN(Engine)

class CSmog;
class CFirework;

END

class CBullet : public CMonster
{
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBullet();

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

private:
	_bool				m_bFirstFrame;
	_bool				m_bCollideWithPlayer;

	_float				m_fAttackTimeLimit;

	Engine::CSmog*		m_pSmogParticleCom;
	Engine::CFirework*	m_pFlameParticleCom;

	_bool				m_bSmog;
	_matrix				m_SmogMatrix;

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};

