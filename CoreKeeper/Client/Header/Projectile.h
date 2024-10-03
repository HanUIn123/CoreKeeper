#pragma once
#include "Monster.h"
class CProjectile : public CMonster
{
	explicit CProjectile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CProjectile();

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

	void			Set_Light();

public:
	void			Set_Chase(_vec3 vPlayerPos) { m_bCharging = false; m_vAttackPoint = vPlayerPos; }




	// 말루가즈용
	void			Set_ChargeActive(bool _bChargeActive) { m_bChargeActive = _bChargeActive; }
	void			Set_ChargingTime(int _iChargingTime) { m_iChargingTime = _iChargingTime; }

private:
	_bool			m_bLightEnable;
	_int			m_iLightNum;

	_bool			m_bCharging;
	_bool			m_bCollideWithPlayer;

	_float			m_fAttackTimeLimit;

	bool			m_bChargeActive;
	int				m_iChargingTime;
public:
	static CProjectile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};

