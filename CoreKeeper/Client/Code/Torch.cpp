#include "pch.h"
#include "..\Header\Torch.h"
#include "Export_System.h"
#include "Export_Utility.h"

CTorch::CTorch(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	 

	m_eItemNum = ITEM_TORCH;
	m_iLightNum = g_iLightNum++;
}

CTorch::~CTorch()
{
}

HRESULT CTorch::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 3);

	m_wItemName = L"횃불";
	m_wItemExplain[0] = L"주변에 불을 밝힙니다.땅에 설치하거나 ";
	m_wItemExplain[1] = L"벽에 부착할 수도 있습니다.";

	return S_OK;
}

_int CTorch::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bUse && !m_bDrop && !m_bActive)
		return 0;

	//SetUp_Light(); // 조명 설정

	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		m_bActive = true;
		m_bDrop = false;
		m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
	}

	if (m_bDrop)
	{
		// 아이템 움직임
		Wave(fTimeDelta);

		// 플레이어와 충돌
		Check_Collision();
	}
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CTorch::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CTorch::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());

	if (m_bActive)
	{
		m_pBufferCom->Render_Buffer();
	}

	// 콜라이더 렌더링 추가
	if (m_bActive)
	{
		m_pColliderCom->Render_Collider();
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShadowTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bDrop)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTorch::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ObjectAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TorchTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ItemCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pShadowBufferCom = dynamic_cast<CShadowTex*>(Engine::Clone_Proto(L"Proto_ShadowTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowCom", pComponent });

	pComponent = m_pShadowTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShadowTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowTexture", pComponent });

	pComponent = m_pShadowTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowTransform", pComponent });

	return S_OK;
}

void CTorch::SetUp_Light()
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
	//light.Range = 3.0f; // 조명의 범위
	//light.Falloff = 1.f; // 감쇠
	//light.Attenuation0 = 1.0f; // 감쇠 계수
	//light.Attenuation1 = 0.01f;
	//light.Attenuation2 = 0.0f;

	//m_pGraphicDev->SetLight(m_iLightNum, &light); // 조명 설정

	//if (m_bUse)
	//{
	//	m_pGraphicDev->LightEnable(m_iLightNum, TRUE); // 조명 활성화
	//}
	//else
	//{
	//	m_pGraphicDev->LightEnable(m_iLightNum, FALSE); // 조명 비활성화
	//}

	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	light.Position = { vPos.x, vPos.y + 10.f, vPos.z };

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

	if (m_bUse)
	{
		m_pGraphicDev->LightEnable(m_iLightNum, TRUE); // 조명 활성화
	}
	else
	{
		m_pGraphicDev->LightEnable(m_iLightNum, FALSE); // 조명 비활성화
	}
}

CTorch* CTorch::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CTorch* pTorch = new CTorch(pGraphicDev);

	if (FAILED(pTorch->Ready_GameObject(vPos)))
	{
		Safe_Release(pTorch);
		MSG_BOX("pTorch Create Failed");
		return nullptr;
	}

	return pTorch;
}

void CTorch::Free()
{
	Engine::CGameObject::Free();
}
