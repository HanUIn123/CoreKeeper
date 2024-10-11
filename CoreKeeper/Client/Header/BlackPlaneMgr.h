#pragma once
#include "Base.h"
#include "Engine_Define.h"


BEGIN(Engine)

class CGameObject;


END

class CBlackPlaneMgr : public CBase
{
    DECLARE_SINGLETON(CBlackPlaneMgr)

private:
    explicit								CBlackPlaneMgr();
    virtual									~CBlackPlaneMgr();

public:
	HRESULT									Ready_BlackPlane(LPDIRECT3DDEVICE9 pGraphicDev);
	void									Update_BlackPlane(const _float& fTimeDelta);

	void									StartFadeIn();   
	void									StartFadeOut();  

	_bool									Create_BlackPlane();
public:
	bool									Is_FadeOutComplete() ; 

private:
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	CGameObject*							m_pBlackPlane;  
	_float									m_fAlpha;       
	_bool									m_bIsFadingIn;  
	_bool									m_bIsFadingOut; 
	_bool m_bFadeOver;

private:
	virtual void							Free();
};

