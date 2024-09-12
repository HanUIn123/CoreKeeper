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
	void				Set_MaxHP(int iMaxHp) { m_tStat.iMaxHp = iMaxHp; }
	void				Set_Damaged(int iDamage) { m_tStat.iHp -= iDamage; }
	void				Set_Recovor(int iHeal) { m_tStat.iHp += iHeal; }
	void				Set_Dead() { m_bDead = true; }

	const STAT*			Get_Stat() { return &m_tStat; }
	const bool			Get_Dead() { return m_bDead; }

public:
	static	CState*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	bool				m_bDead;
	STAT				m_tStat;

private:
	virtual void		Free();
};

END