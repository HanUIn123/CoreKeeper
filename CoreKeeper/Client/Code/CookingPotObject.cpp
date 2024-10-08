#include "pch.h"
#include "../Header/CookingPotObject.h"
#include "../Header/Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/CraftMgr.h"

CCookingPotObject::CCookingPotObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_fTime(80.f)
{
}

CCookingPotObject::~CCookingPotObject()
{
}

HRESULT CCookingPotObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 100);

	return S_OK;
}

_int CCookingPotObject::Update_GameObject(const _float& fTimeDelta)
{
	if (Check_Interaction())
	{
		Interaction();
	}
	else if (!Check_Interaction())
	{
		if (m_bCollision)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

			if (pPlayer->Get_CookingUI())
			{
				pPlayer->Set_CookingPot();

				m_bCollision = false;
			}
		}
	}
	if (!m_pInventoryCom->Check_Empty(0) && !m_pInventoryCom->Check_Empty(1))
	{
		m_pAnimatorCom->Set_CurState(WALK, 1, 4, 10);
	}
	else if ((m_pInventoryCom->Check_Empty(0) && m_pInventoryCom->Check_Empty(1) && !m_pSecondInventoryCom->Check_Empty(0))
		|| (!m_pInventoryCom->Check_Empty(0) && m_pInventoryCom->Check_Empty(1) && !m_pSecondInventoryCom->Check_Empty(0))
		|| (m_pInventoryCom->Check_Empty(0) && !m_pInventoryCom->Check_Empty(1) && !m_pSecondInventoryCom->Check_Empty(0))
		)
		m_pAnimatorCom->Set_CurState(SWING, 5, 5, 100);
	else
		m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 100);
	m_pAnimatorCom->Update_Animation();


	if (!m_pInventoryCom->Check_Empty(0) && !m_pInventoryCom->Check_Empty(1))
	{
		m_fTime -= fTimeDelta * 5.f;

		//m_Rect.top = (_long)(m_rFirstRect.top + ((_float)(m_rFirstRect.bottom - m_rFirstRect.top) - ((_float)(m_rFirstRect.bottom - m_rFirstRect.top) * (m_fTime / 80.f))));

		if (m_fTime <= 0)
		{
			m_fTime = 80.f;

			ITEMNUM eUNum = m_pInventoryCom->Get_Item(0)->Get_ItemNum();

			ITEMNUM eDNum = m_pInventoryCom->Get_Item(1)->Get_ItemNum();

			CItem* pCraftItem = CCraftMgr::GetInstance()->Cook(eUNum, eDNum);

			m_pSecondInventoryCom->Add_Item(pCraftItem);

			m_pInventoryCom->Minus_Item(m_pInventoryCom->Get_Item(0)->Get_ItemNum(), 1);

			m_pInventoryCom->Minus_Item(m_pInventoryCom->Get_Item(1)->Get_ItemNum(), 1);
		}
	}
	else if (m_pInventoryCom->Check_Empty(1) && !m_pInventoryCom->Check_Empty(0) || m_pInventoryCom->Check_Empty(0) && !m_pInventoryCom->Check_Empty(1))
	{
		m_fTime = 80.f;
	}

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CCookingPotObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CCookingPotObject::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture();
	m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pAnimBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CCookingPotObject::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

		pPlayer->Set_CookingPot(m_pInventoryCom, m_pSecondInventoryCom, &m_fTime);

		m_bCollision = true;
	}
}

HRESULT CCookingPotObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ObjectAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CookingPotTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_NormalCubeCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_CookingPotInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	pComponent = m_pSecondInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_OneSlotInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SecondInventory", pComponent });

	return S_OK;
}

CCookingPotObject* CCookingPotObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CCookingPotObject* pCore = new CCookingPotObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CCookingPotObject::Free()
{
	Engine::CGameObject::Free();
}
