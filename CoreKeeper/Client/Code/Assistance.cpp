#include "pch.h"
#include "..\Header\Assistance.h"
#include "Export_System.h"
#include "Export_Utility.h"

CAssistance::CAssistance(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev), m_eAssistance(ASSISTANCE_END)
{
	m_eItemNum = ITEM_ASSISTANCE;
}

CAssistance::~CAssistance()
{
}

HRESULT CAssistance::Ready_GameObject(ASSISTANCE _eAssistance, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eAssistance = _eAssistance;
	m_iTextureNumber = m_eAssistance;

	switch (m_eAssistance)
	{
	case ASSISTANCE_WOOD_SHIELD:
		m_wItemName = L"나무 방패";
		m_wItemExplain[0] = L"적의 공격을 막아 주는 유용한 보조장비 아이템입니다.";
		break;
	case ASSISTANCE_FEATHER:
		m_wItemName = L"신속 깃털";
		m_wItemExplain[0] = L"위험에서 벗어나게 하는 수수께끼의 깃털입니다.";
		break;
	case ASSISTANCE_IRON_SHIELD:
		m_wItemName = L"철 방패";
		m_wItemExplain[0] = L"문장이 그려진 방패입니다.";
		m_wItemExplain[1] = L"공격으로부터 상당한 보호를 제공합니다.";
		break;
	case ASSISTANCE_AZEOS_FEATHER:
		m_wItemName = L"아제오스의 돌진 깃털";
		m_wItemExplain[0] = L"하늘 거인 아제오스의 다채로운 발톱 깃털입니다.";
		m_wItemExplain[1] = L"쥐고 있으면 고속으로 돌진할 수 있습니다.";
		break;
	case ASSISTANCE_BOOK:
		m_wItemName = L"파랑 가죽 책";
		m_wItemExplain[0] = L"파란색 커버가 봉제된 고대의 책입니다.";
		m_wItemExplain[1] = L"설마 피부로 표지를 만든 건가요?";
		break;
	}

	//m_tStat.iDefense = 10 * (m_eMaterial + 1);
	//m_tStat.iMaxHp = 20 * (m_eMaterial + 1);

	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 3);

	return S_OK;
}

_int CAssistance::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		Swing(0, 5, 2);

		m_bActive = true;
		m_bDrop = false;
		m_pTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);
	}

	if (m_bDrop)
	{
		// 아이템 움직임
		CItem::Wave(fTimeDelta);

		Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
			(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));

		// 플레이어와 충돌
		if (m_pColliderCom->Check_Collision(pPlayerCollider))
		{
			In_Inventory();
		}
	}	
	if (m_bFollow)
		Follow_Player();
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAssistance::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CAssistance::Render_GameObject()
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

HRESULT CAssistance::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_NormalAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AssistanceTexture"));
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

CAssistance* CAssistance::Create(LPDIRECT3DDEVICE9 pGraphicDev, ASSISTANCE _eAssistance, _vec3 vPos)
{
	CAssistance* pAssistance = new CAssistance(pGraphicDev);

	if (FAILED(pAssistance->Ready_GameObject(_eAssistance, vPos)))
	{
		Safe_Release(pAssistance);
		MSG_BOX("pAssistance Create Failed");
		return nullptr;
	}

	return pAssistance;
}

void CAssistance::Free()
{
	Engine::CGameObject::Free();
}
