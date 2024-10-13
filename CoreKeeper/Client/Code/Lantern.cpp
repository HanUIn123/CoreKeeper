#include "pch.h"
#include "..\Header\Lantern.h"
#include "Export_System.h"
#include "Export_Utility.h"

CLantern::CLantern(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	 

	m_eItemNum = ITEM_LANTERN;
}

CLantern::~CLantern()
{
}

HRESULT CLantern::Ready_GameObject(MATERIAL _eMaterial, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eMaterial = _eMaterial;
	m_iTextureNumber = m_eMaterial;

	switch (m_eMaterial)
	{
	case MATERIAL_WOOD:
		m_wItemName = L"소형 등불";
		m_wItemExplain[0] = L"불빛 +3";
		m_wItemExplain[1] = L"벨트에 장착할 수 있는 전등입니다.";
		m_wItemExplain[2] = L"약한 빛을 냅니다.";
		break;

	case MATERIAL_COPPER:
		m_wItemName = L"등불";
		m_wItemExplain[0] = L"불빛 +4";
		m_wItemExplain[1] = L"손 슬롯을 차지하지 않는 작고 유용한 등입니다.";
		break;

	case MATERIAL_IRON:
		m_wItemName = L"구체 등불";
		m_wItemExplain[0] = L"불빛 +5";
		m_wItemExplain[1] = L"벨트에 장착할 수 있는 개선된 전등입니다.";
		m_wItemExplain[2] = L"더 밝은 빛을 냅니다.";
		break;
	}

	m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 3);

	//m_wItemExplain[0] = L"횃불";
	//m_wItemExplain[1] = L"주변에 불을 밝힙니다.땅에 설치하거나 ";
	//m_wItemExplain[2] = L"벽에 부착할 수도 있습니다.";
	//
	//m_wItemMetrial = L"소재 : 나무 1";

	//m_bMeterial = true;

	return S_OK;
}

_int CLantern::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bUse && !m_bDrop && !m_bActive)
		return 0;

	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		m_bActive = true;
		m_bDrop = false;
		m_pTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);
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

void CLantern::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CLantern::Render_GameObject()
{
	// 카메라를 바라보게 하면서 스케일 유지
	//CItem::Apply_Billboard();  

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

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

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bDrop)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CLantern::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_NormalAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_LanternTexture"));
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

CLantern* CLantern::Create(LPDIRECT3DDEVICE9 pGraphicDev, MATERIAL _eMaterial, _vec3 vPos)
{
	CLantern* pLantern = new CLantern(pGraphicDev);

	if (FAILED(pLantern->Ready_GameObject(_eMaterial, vPos)))
	{
		Safe_Release(pLantern);
		MSG_BOX("pLantern Create Failed");
		return nullptr;
	}

	return pLantern;
}

void CLantern::Free()
{
	Engine::CGameObject::Free();
}
