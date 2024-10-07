#pragma once
#include "Base.h"
#include "Engine_Define.h"

class CPlayer;
class CUIBuff;
class CBuffMgr : public CBase
{
	DECLARE_SINGLETON(CBuffMgr)

private:
	explicit CBuffMgr();
	virtual ~CBuffMgr();

public:
	HRESULT		Ready_Buff(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Update_Buff(const _float& fTimeDelta);

public:
	void		Set_BuffStart(BUFFTYPE _eType, _float fTime);
	void		Set_BuffEnd(BUFFTYPE _eType) { m_arrBuffState[_eType].x = 0.f; }
	_vec2		Get_BuffInfo(BUFFTYPE _eType) { return m_arrBuffState[_eType]; }
	_bool		Get_BuffEnd(BUFFTYPE _eType) { return m_arrBuffState[_eType].x <= 0.f; }
	void		Set_UIBuff(BUFFTYPE _eType, CUIBuff* pObject) { m_vecUIBuff.push_back(pObject); }

private:
	void		BuffTimer(const _float& fTimeDelta);
	void		Arrange_UIBuff(_int iIndex);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_vec2					m_arrBuffState[BUFFTYPE_END]; // x : 현재 시간, y : 설정된 시간
	vector<CUIBuff*>		m_vecUIBuff;
	CPlayer*				m_pPlayer;
	vector<BUFFTYPE>		m_vecCurOrder;

private:
	virtual void		Free();
};

