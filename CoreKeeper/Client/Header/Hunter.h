#pragma once
#include "Monster.h"

class CHunterEye;
class CBUllet;
class CHunter : public CMonster
{
public:
	enum	HUNTERIDLETYPE
	{
		HUNTER_BUSH, HUNTER_PEAK, HUNTER_GOTOBUSH, HUNTER_LEAVEBUSH, HUNTER_IDLE, HUNTER_MOVE,		
		HUNTER_END
	};
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
	HUNTERIDLETYPE			m_eIdleType;
	_bool					m_bIdleFirstFrame;
	/*
	 	0. 은신(Bush)
		0 - 1. 빼꼼(Peak)
		0 - 2. 은신 진입(GoToBush)
		0 - 3. 은신 해제(LeaveBush)
		1. 정지(Idle)
		2. 이동(Move)
	*/
	_int					m_iChaseType;
	_bool					m_bChaseFirstFrame;
	/*
		0. 은신 해제(LeaveBush)
		1. 빠른 이동(Move)
	*/
	_int					m_iSwingType;
	_bool					m_bLeaveBeforeSwing;
	_bool					m_bIsReloaded;
	_bool					m_bSwingFirstFrame;
	/*
		0. 장전(Reload)
		1. 발사(RangedAttack)
	*/

	_float					m_fRange;

	_bool					m_bCharging;
	_int					m_iFrameCount;
	_int					m_iAttackAnimProgress;
	
	vector<wstring>			m_vecBulletName;
	_int					m_iCurNumber;
	
	CHunterEye*				m_pHunterEye;

public:
	static CHunter* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};

