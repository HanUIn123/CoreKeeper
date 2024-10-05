#include "pch.h"
#include "../Header/Core.h"
#include "Export_System.h"
#include "Export_Utility.h"

CCore::CCore(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev)
{
	m_iLightNum = g_iLightNum++;
}

CCore::~CCore()
{
}

HRESULT CCore::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CCore::Update_GameObject(const _float& fTimeDelta)
{
	SetUp_Light(); // 조명 설정

	Add_RenderGroup(RENDER_ALPHA, this);

	Add_RenderGroup(RENDER_MAP, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CCore::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CCore::Render_GameObject()
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CCore::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_CoreTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoreTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	return S_OK;
}

void CCore::SetUp_Light()
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_POINT; // 포인트 조명
	light.Diffuse = { 0.4f, 0.4f, 0.6f, 1.0f }; // 확산 색상 (연한 파란색)
	light.Specular = { 0.4f, 0.4f, 0.6f, 1.0f }; // 반사 색상
	light.Ambient = { 0.4f, 0.4f, 0.6f, 1.0f }; // 주변광 (부드럽고 연한 파란색)

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	light.Position = vPos; // 횃불의 위치
	light.Range = 5.0f; // 조명의 범위
	light.Falloff = 1.f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	m_pGraphicDev->SetLight(m_iLightNum, &light); // 조명 설정

	m_pGraphicDev->LightEnable(m_iLightNum, TRUE); // 조명 활성화
}

CCore* CCore::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CCore* pCore = new CCore(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CCore::Free()
{
	Engine::CGameObject::Free();
}
