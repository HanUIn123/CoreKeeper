#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShader();

public:
	HRESULT					Ready_Shader(const _tchar* pShaderFilePath);

public:
	HRESULT					Begin(_uint iPassIndex);
	HRESULT					End();

	HRESULT					Set_RawValue(D3DXHANDLE hParameter, const void* pData, _uint iLength);
	HRESULT					Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);

	LPD3DXEFFECT			Get_Effect() { return m_pEffect; }
public:
	static	CShader*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pShaderFilePath);
	virtual CComponent*		Clone();
	
private:
	virtual void			Free();

private:
	LPD3DXEFFECT			m_pEffect;
};

END