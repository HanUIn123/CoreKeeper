#pragma once
#include "Monster.h"

class CBUllet;
class CHunter : public CMonster
{
private:
	explicit CHunter(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHunter();

public:
	HRESULT			Ready_GameObject(_vec3 vPos) override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject() override;
	void			Render_GameObject() override;

private:
	HRESULT			Add_Component();

	void			Pattern_Idle(const _float& fTimeDelta) override;
	void			Pattern_Chase(const _float& fTimeDelta) override;
	void			Pattern_Attack(const _float& fTimeDelta) override;
	void			Pattern_Dead() override;
	STATE			State_Change() override;

	void			Set_Direction(_vec3* vPlayerPos);

private:
	_float					m_fRange;

	_bool					m_bCharging;
	_int					m_iFrameCount;
	_int					m_iAttackAnimProgress;
	
	vector<wstring>			m_vecBulletName;
	_int					m_iCurNumber;

public:
	static CHunter* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};

