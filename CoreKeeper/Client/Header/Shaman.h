#pragma once
#include "Monster.h"

class CProjectile;
class CShaman : public CMonster
{
	explicit CShaman(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShaman();

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

	void			Set_Direction(_vec3* vPlayerPos);
	void			Set_Light();

private:
	_bool			m_bLightEnable;
	_int			m_iLightNum;

	_bool			m_bCharging;
	_float			m_fRange;
	_int			m_iFrameCount;
	_int			m_iAttackAnimProgress;
	vector<wstring>			m_vecProjectileName;
	_int			m_iCurNumber;


public:
	static CShaman* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};

