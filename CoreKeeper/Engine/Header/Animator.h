#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CAnimator :	public CComponent
{
private:
	explicit CAnimator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAnimator();

public:
	HRESULT			Ready_Animator();
	void			Update_Animation();

	void			Set_CurState(STATE _eCurState, int _iStart, int _iEnd, int _iCount);
	int				Get_MotionIndex() { return m_tAnim.iMotion; }
	bool			Get_MotionEnd() { return m_bMotionEnd; }

public:
	static CAnimator*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CAnimator*	Clone();

private:
	virtual void Free();

private:
	ANIM	m_tAnim;
	bool	m_bMotionEnd;
};

END