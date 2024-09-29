#include "Export_Utility.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}


CShader::~CShader()
{
	Safe_Release(m_pEffect);
}

HRESULT CShader::Ready_Shader(const _tchar* pShaderFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, pShaderFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->Begin(nullptr, 0);
	m_pEffect->BeginPass(iPassIndex);

	return S_OK;
}

HRESULT CShader::End()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->EndPass();
	m_pEffect->End();

	return S_OK;
}

HRESULT CShader::Set_RawValue(D3DXHANDLE hParameter, const void* pData, _uint iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetRawValue(hParameter, pData, 0, iLength);
}

HRESULT CShader::Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(hParameter, pTexture);
}

CShader* CShader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pShaderFilePath)
{
	CShader* pShader = new CShader(pGraphicDev);

	if (FAILED(pShader->Ready_Shader(pShaderFilePath)))
	{
		Safe_Release(pShader);
		return nullptr;
	}

	return pShader;
}

CComponent* CShader::Clone()
{
	return new CShader(*this);
}

void CShader::Free()
{
	CComponent::Free();
}
