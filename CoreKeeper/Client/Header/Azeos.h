#pragma once
#include "Monster.h"

class CAzeos : public CMonster
{
private:

private:
	explicit CAzeos(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAzeos();

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

	void            Pattern_Teleport();
	void            Pattern_GenerateCrystal();
	void            Pattern_GenerateThunder();

	void            Generate_Line();
	void            Generate_Random();
	void            Generate_Circle();

	void		    Check_CrystalCollide();

public:
	_vec3           Get_Pos() { return m_vPos; }
	_bool           Get_Crystal() { return m_bCrystal; }
	void            Set_Crystal() { m_bCrystal = false; }

private:
	_bool			m_bLightEnable;
	_int			m_iLightNum;

	_bool			m_bCharging;
	_float			m_fRange;
	_int			m_iFrameCount;
	_int			m_iAttackAnimProgress;
	vector<wstring>			m_vecProjectileName;
	_int			m_iCurNumber;

	int				m_iPhase;
	int				m_iIdleCount;
	int				m_iTextureNum;
	
	_vec3           m_vFirstPos;

	_bool           m_bCrystal;

	_int            m_iRandomPattern;
	_int            m_iRandom;

	_int            m_iEndCount;

	_bool           m_bAttack;

	_vec3           m_vPos;

	_int            m_iCrystalNumber;

	_bool           m_bDead;

public:
	static	CAzeos*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void	Free();
};

