#include "pch.h"
#include "..\Header\Potion.h"
#include "Export_System.h"
#include "Export_Utility.h"

CPotion::CPotion(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	m_eItemNum = ITEM_POTION_HP;

	m_tStat.iMaxHp = 10;

}

CPotion::~CPotion()
{
}

HRESULT CPotion::Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eItemNum = _eItemNum;
	
	switch (m_eItemNum)
	{
	case ITEM_POTION_HP:
		m_iTextureNumber = 0;
		m_tStat.iMaxHp = 10;
		m_wItemName = L"치유 물약";
		m_wItemExplain[0] = L"체력 +200";
		m_wItemExplain[1] = L"몸에 좋은 물약으로, 마시면 즉시 치유됩니다.";
		m_wItemExplain[2] = L"물론 맛도 좋습니다.";
		break;
	case ITEM_POTION_ATT:
		m_iTextureNumber = 1;
		m_tStat.iAttack = 10;
		m_wItemName = L"격노 물약";
		m_wItemExplain[0] = L"물리 피해, 마법 피해 증가";
		m_wItemExplain[1] = L"근육을 강화하고 화를 돋우는 강력한 물약입니다.";
		break;
	case ITEM_POTION_DEF:
		m_iTextureNumber = 2;
		m_tStat.iDefense = 10;
		m_wItemName = L"암석 피부 물약";
		m_wItemExplain[0] = L"방어력 증가";
		m_wItemExplain[1] = L"피부를 단단하게 만들고 회복력을 높이는 짭짤한 물약입니다.";
		break;
	}

	m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	return S_OK;
}

_int CPotion::Update_GameObject(const _float& fTimeDelta)
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
		m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
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

void CPotion::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPotion::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);

	m_pBufferCom->Set_Index(0);

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

HRESULT CPotion::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_NormalAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PotionTexture"));
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

CPotion* CPotion::Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eItemNum, _vec3 vPos)
{
	CPotion* pPotion = new CPotion(pGraphicDev);

	if (FAILED(pPotion->Ready_GameObject(_eItemNum, vPos)))
	{
		Safe_Release(pPotion);
		MSG_BOX("pPotion Create Failed");
		return nullptr;
	}

	return pPotion;
}

void CPotion::Free()
{
	Engine::CGameObject::Free();
}
