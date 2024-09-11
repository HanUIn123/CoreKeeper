#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRangeTex : public CVIBuffer
{
private:
	explicit CRangeTex();
	explicit CRangeTex(LPDIRECT3DDEVICE9 pGraphicDev, _int _iTexWidth);
	explicit CRangeTex(const CRangeTex& rhs);
	virtual ~CRangeTex();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();

	void					Set_Index(int _iCurIndex) { m_iCurIndex = _iCurIndex; }
	void					Set_Width(int _iCurWidth) { m_iTexWidth = _iCurWidth; } // 현재 넓이 가져오기 (백분율로  ex : 80%일 때 _iCurWidth = 80)
public:
	static		CRangeTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int _iTexWidth);
	virtual		CComponent* Clone();

private:
	virtual void		Free();

private:
	int		m_iTexWidth;
	int     m_iCurWidth;
	//int		m_iTexHeight;
	int		m_iCurIndex;
};

END