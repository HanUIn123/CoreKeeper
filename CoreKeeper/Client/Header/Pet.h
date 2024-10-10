#pragma once
#include	"Monster.h"
#include	"Item.h"

class CPet : public CMonster
{
public:
	enum	PETIDLETYPE
	{
		PET_IDLE, PET_STRETCH, PET_SITDOWN, PET_SITTING, PET_STAND, PET_GROOM,
		PET_MOVE,
		PET_HAPPY,
		PET_ATTACK,
		PET_END
	};
private:
	explicit CPet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPet();

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
	void			Pattern_Interact();
	void			Pattern_Dead() override;
	STATE			State_Change() override;

	void			Set_Direction(_vec3* vPlayerPos);
	void			Set_Teleport(_vec3 vPlayerPos);
	void			Check_Stop();

private:
	_bool					m_bJustRespawned;

	PETIDLETYPE				m_eIdleType;
	_bool					m_bIdleFirstFrame;
	/*
		0. Idle
		1. Stretch
		2. SitDown
		3. Sitting
		4. Stand
		5. Groom
	*/
	_int					m_iChaseType;
	_bool					m_bChaseFirstFrame;
	_int					m_iChaseStep;
	_vec3					m_vMoveDir;
	list<_int>				m_listTargetIndex;
	_float					m_fSaveTimer;
	_float					m_fTeleportTimer;
	/*
		0. 이동(Move)
		1. 순간 이동
		2. 그냥 이동
	*/
	_int					m_iSwingType;
	_bool					m_bSwingFirstFrame;
	/*
		0. 발사(RangedAttack)
	*/
	_bool					m_bIsInteracting;
	_bool					m_bIsTeleporting;
	_bool					m_bIsForcedMove;

	_bool					m_bCharging;
	_int					m_iFrameCount;
	_int					m_iAttackAnimProgress;

	vector<wstring>			m_vecFireName;
	_int					m_iCurNumber;

public:
	static CPet* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = {0, 0, 0});

private:
	virtual void		Free();
};

