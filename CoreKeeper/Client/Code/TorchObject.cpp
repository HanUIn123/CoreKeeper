#include "pch.h"
#include "../Header/TorchObject.h"
#include "Export_System.h"
#include "Export_Utility.h"

CTorchObject::CTorchObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev)
{
	m_iLightNum = g_iLightNum++;
}

CTorchObject::~CTorchObject()
{
}

HRESULT CTorchObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);	

	m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 3);

	return S_OK;
}

_int CTorchObject::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pCalculCom->In_Frustum(m_pTransformCom))
	{
		SetUp_Light(); // 조명 설정
	}

	m_pAnimatorCom->Update_Animation();
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CTorchObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CTorchObject::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();
	m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pAnimBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTorchObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TorchTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ObjectAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	return S_OK;
}

void CTorchObject::SetUp_Light()
{
	//D3DLIGHT9 light;
	//ZeroMemory(&light, sizeof(D3DLIGHT9));

	//light.Type = D3DLIGHT_POINT; // 포인트 조명
	//light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	//light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	//light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	//_vec3 vPos;
	//m_pTransformCom->Get_Info(INFO_POS, &vPos);

	//light.Position = vPos; // 횃불의 위치
	//light.Range = 5.0f; // 조명의 범위
	//light.Falloff = 1.f; // 감쇠
	//light.Attenuation0 = 1.0f; // 감쇠 계수
	//light.Attenuation1 = 0.01f;
	//light.Attenuation2 = 0.0f;

	//m_pGraphicDev->SetLight(m_iLightNum, &light); // 조명 설정
	//m_pGraphicDev->LightEnable(m_iLightNum, TRUE); // 조명 활성화

	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	light.Position = { vPos.x, vPos.y+10.f, vPos.z };

	// 아래쪽을 향하는 스포트라이트
	_vec3 vDir = { 0.0f, -1.0f, 0.0f };
	light.Direction = vDir;

	light.Range = 5.f * 10.f; // 조명의 범위
	light.Falloff = 1.0f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	// 스포트라이트의 내부 및 외부 각도 설정
	light.Theta = D3DXToRadian(20.0f); // 내부 각도 (작은 값일수록 집중된 조명)
	light.Phi = D3DXToRadian(40.0f); // 외부 각도 (큰 값일수록 퍼지는 조명)

	m_pGraphicDev->SetLight(m_iLightNum, &light);
	m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
}

CTorchObject* CTorchObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CTorchObject* pTorchObject = new CTorchObject(pGraphicDev);

	if (FAILED(pTorchObject->Ready_GameObject(vPos)))
	{
		Safe_Release(pTorchObject);
		MSG_BOX("pTorchObject Create Failed");
		return nullptr;
	}

	return pTorchObject;
}

void CTorchObject::Free()
{
	Engine::CGameObject::Free();
}
