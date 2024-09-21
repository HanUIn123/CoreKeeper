#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CAnimTex : public CVIBuffer
{
private:
	explicit CAnimTex();
	explicit CAnimTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAnimTex(const CAnimTex& rhs);
	virtual ~CAnimTex();

public:
	virtual		HRESULT		Ready_Buffer(int _iTexWidth, int _iTexHeight);
	virtual		void		Render_Buffer();

	void					Render_First();
	void					Set_Index(int _iCurIndex) { m_iCurIndex = _iCurIndex; }
public:
	static		CAnimTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iTexWidth, int _iTexHeight);
	virtual		CComponent* Clone();

private:
	virtual void		Free();

private:
	int		m_iTexWidth;
	int		m_iTexHeight;
	int		m_iCurIndex;
};

END