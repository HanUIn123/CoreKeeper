#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CState : public CComponent
{
private:
	explicit CState();
	explicit CState(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CState(const CState& rhs);
	virtual ~CState();

public:
	HRESULT				Ready_State();

public:
	// Get, Set ÇÔ¼ö
	void				Set_Stat(int iMaxHp, int iMaxMp, int iAtt, int iDef)
	{
		m_tStat.iMaxHp = iMaxHp; m_tStat.iHp = iMaxHp;
		m_tStat.iMaxMp = iMaxMp; m_tStat.iMp = iMaxMp;
		m_tStat.iAttack = iAtt; m_tStat.iDefense = iDef;
	}
	void				Set_BasicStat(STAT* pStat) 
	{ 
		m_tStat.iAttack = pStat->iAttack;
		m_tStat.iDefense = pStat->iDefense;
		m_tStat.iMaxHp = pStat->iMaxHp;
		m_tStat.iMaxMp = pStat->iMaxMp;
	}
	void				Set_EquippedStat(STAT* pStat) 
	{ 
		m_tStat.iAttack += pStat->iAttack;
		m_tStat.iDefense += pStat->iDefense;
		m_tStat.iMaxHp += pStat->iMaxHp;
		m_tStat.iMaxMp += pStat->iMaxMp;
	}
	void				Set_BuffStat(STAT* pStat)
	{
		m_tStat.iAttack += pStat->iAttack;
		m_tStat.iDefense += pStat->iDefense;
		m_tStat.iMaxHp += pStat->iMaxHp;
		m_tStat.iMaxMp += pStat->iMaxMp;
		if (m_tStat.iMaxHp < m_tStat.iHp)
			m_tStat.iHp = m_tStat.iMaxHp;
	}

	void				Set_MaxHP(int iMaxHp) { m_tStat.iMaxHp = iMaxHp; }
	void				Set_Damaged(int iDamage) { m_tStat.iHp -= iDamage; }
	void				Set_UseMP(int iMP) { m_tStat.iMp -= iMP; }
	void				Set_MP() { m_tStat.iMp = m_tStat.iMaxMp; }
	void				Set_Recover(int iHeal) { m_tStat.iHp += iHeal; if (m_tStat.iHp > m_tStat.iMaxHp) m_tStat.iHp = m_tStat.iMaxHp; }
	void				Set_Dead() { m_bDead = true; }
	void				Set_Revive() { m_tStat.iHp = m_tStat.iMaxHp; m_bDead = false; }

	void				Set_AttPlus(_int iAtt) { m_tStat.iAttack += iAtt; }
	void				Set_MaxHpPlus(_int iMaxHp) { m_tStat.iMaxHp += iMaxHp; m_tStat.iHp += iMaxHp; }
	void				Set_DefPlus(_int iDef) { m_tStat.iDefense += iDef; }

	void				Set_MaxHunger(_int iMax) { m_iMaxHunger = iMax; m_iHunger = m_iMaxHunger; }
	_int				Get_Hunger() { return m_iHunger; }
	void				Set_Hunger(_int iHunger) { m_iHunger = iHunger; }
	_int				Get_MaxHunger() { return m_iMaxHunger; }
	void				Set_HungerMinus(_int iMinus) { m_iHunger -= iMinus; if (m_iHunger < 0) m_iHunger = 0; }
	void				Set_HungerPlus(_int iPlus) { m_iHunger += iPlus; if (m_iHunger > m_iMaxHunger) m_iHunger = m_iMaxHunger; }

	const STAT*			Get_Stat() { return &m_tStat; }
	const bool			Get_Dead() { if (m_tStat.iHp <= 0) m_bDead = true; return m_bDead; }

public:
	static	CState*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	bool				m_bDead;
	STAT				m_tStat;
	_int				m_iHunger;
	_int				m_iMaxHunger;

private:
	virtual void		Free();
};

END