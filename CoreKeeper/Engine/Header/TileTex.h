#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CTileTex : public CVIBuffer
{
private:
	explicit									CTileTex();
	explicit									CTileTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit									CTileTex(const CTileTex& rhs);
	virtual										~CTileTex();

public:
	const _vec3* Get_VtxPos() const { return m_pPos; }
public:
	virtual		HRESULT							Ready_Buffer();
	virtual void								Render_Buffer();

public:
	static		CTileTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		CComponent* Clone();

private:
	_vec3* m_pPos;

private:
	virtual void								Free();

};

END