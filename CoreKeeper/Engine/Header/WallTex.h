#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CWallTex : public CVIBuffer
{
private:
	explicit									CWallTex();
	explicit									CWallTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit									CWallTex(const CWallTex& rhs);
	virtual										~CWallTex();

public:
	virtual		HRESULT							Ready_Buffer();
	virtual		void							Render_Buffer();

public:
	static		CWallTex*						Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		CComponent*						Clone();

private:
	virtual void								Free();

};

END

