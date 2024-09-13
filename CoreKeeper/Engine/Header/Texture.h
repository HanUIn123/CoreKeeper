#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture :	public CComponent
{
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	HRESULT		Ready_Texture(const _tchar* pPath, TEXTUREID eType, const int& iCnt);
	void		Set_Texture(const _uint& iIndex = 0);

	IDirect3DBaseTexture9* Get_Texture(_uint _iIndex = 0) { return m_vecTexture[_iIndex]; }

private:
	vector<IDirect3DBaseTexture9*>			m_vecTexture;

public:
	static CTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath, TEXTUREID eType, const _uint& iCnt = 1);
	virtual CComponent*		Clone();

private:
	virtual void	Free();
};

END