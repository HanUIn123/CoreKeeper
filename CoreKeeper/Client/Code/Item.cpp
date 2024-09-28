#include "pch.h"
#include "..\Header\Item.h"
#include "Export_System.h"
#include "Export_Utility.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iTextureNumber(0), m_fFirstY(0.f), m_fTimeAcc(0.f), m_fSpeed(0.5f), m_bActive(true), m_bDrop(false), m_bDropSelf(false), m_bUse(false), m_bSwing(false), m_iCount(1), m_bHasRotated(false), m_fAngle(0.f), m_bFollow(false), m_bMeterial(false), m_eMaterial(MATERIAL_END)
{
	ZeroMemory(&m_tStat, sizeof(STAT));

	m_fWalkYSpeed = 2.4f;
	m_fAngleX = 0.f;
	m_fAngleY = 0.f;
}

CItem::~CItem()
{
}

HRESULT CItem::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	return S_OK;
}

_int CItem::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();

	if (m_bUse)
	{
		Swing(0, 5, 2);

		m_bActive = true;
		m_bDrop = false;
		m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
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
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CItem::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CItem::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	// 여러 텍스쳐 가진 애들 쓸 때
	m_pTextureCom->Set_Texture(m_iTextureNumber);
	
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

	if (m_bActive)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CItem::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_SwordAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SwordTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ItemCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pShadowBufferCom = dynamic_cast<CShadowTex*>(Engine::Clone_Proto(L"Proto_ShadowTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowCom", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pShadowTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShadowTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowTexture", pComponent });

	pComponent = m_pShadowTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowTransform", pComponent });

	return S_OK;
}

void CItem::Wave(const _float& fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 1.0f)
	{
		m_fSpeed *= -1;
		m_fTimeAcc = 0.0f;
	}
	_vec3 vUp;
	m_pTransformCom->Get_Info(INFO_UP, &vUp);
	m_pTransformCom->Move_Pos(&vUp, fTimeDelta, m_fSpeed);
}

void CItem::Swing(int start, int end, int Count)
{
	if (m_bSwing)
	{
		if (!m_bHasRotated)
		{
			if (g_bIsTopCamera)
			{
				switch (m_eDir)
				{
				case FRONT:
					m_fAngle = -90.f;
					m_fAngleY = 90.f;
					m_pColliderCubeCom->Set_Offset(_vec3(0, 0, -1.f));
					break;
				case LEFT:
					m_fAngle = 180.f;
					m_fAngleX = 90.f;
					m_pColliderCubeCom->Set_Offset(_vec3(-1.f, 0, 0));
					break;
				case BACK:
					m_fAngle = 90.f;
					m_fAngleY = -90.f;
					m_pColliderCubeCom->Set_Offset(_vec3(0, 0, 1.f));
					break;
				default:
					m_fAngle = 0.0f;
					m_fAngleX = 90.f;
					m_pColliderCubeCom->Set_Offset(_vec3(1.f, 0, 0));
					break;
				}
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(m_fAngleY));
				m_pTransformCom->Rotation(ROT_X, D3DXToRadian(m_fAngleX));
				m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(m_fAngle));
			}
			else
			{
				_vec3 vUp, vPlayerLook;
				CTransform* pPlayerTransformCom = dynamic_cast<Engine::CTransform*>
					(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

				m_pTransformCom->Get_Info(INFO_UP, &vUp);
				m_pTransformCom->Set_RotArbit(&vUp, -45);

				pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
				m_pColliderCubeCom->Set_Offset(vPlayerLook * 3.f);
			}
			m_bHasRotated = true;
		}

		m_pAnimatorCom->Set_CurState(SWING, start, end, Count);

		if (m_pAnimatorCom->Get_MotionEnd())
		{
			if (m_bHasRotated)
			{
				if (g_bIsTopCamera)
				{
					m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(-m_fAngle));
					m_pTransformCom->Rotation(ROT_X, D3DXToRadian(-m_fAngleX));
					m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-m_fAngleY));
					m_fAngle = 0.f;
					m_fAngleX = 0.f;
					m_fAngleY = 0.f;
				}
			}

			m_bSwing = false;
			m_bHasRotated = false;
		}
	}
	else
	{
		m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 10);
		m_bHasRotated = false;
	}
}

void CItem::Walk_Equipped(const _float& fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * 11.f;

	if (m_fTimeAcc >= 1.0f)
	{
		m_fWalkYSpeed *= -1;
		m_fTimeAcc = 0.0f;
	}
	_vec3 vUp;
	m_pTransformCom->Get_Info(INFO_UP, &vUp);
	m_pTransformCom->Move_Pos(&vUp, fTimeDelta, m_fWalkYSpeed);
}

void CItem::Follow_Player()
{
	CTransform* playerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	if (playerTransform)
	{
		_vec3 vPlayerPos, vPlayerAngle, vPlayerScale;
		playerTransform->Get_Info(INFO_POS, &vPlayerPos);

		if (g_bIsTopCamera)
		{
			m_pTransformCom->Set_Angle(0, 0, 0);
			m_pTransformCom->Set_Pos(vPlayerPos.x, vPlayerPos.y - 0.04f, vPlayerPos.z - 0.001f);
		}
		else
		{
			_vec3 vPlayerLook;
			playerTransform->Get_Info(INFO_LOOK, &vPlayerLook);
			m_pTransformCom->Set_Pos(vPlayerPos.x - vPlayerLook.x * 0.02f, vPlayerPos.y - 0.04f, vPlayerPos.z - vPlayerLook.z * 0.02f);
			vPlayerAngle = *(playerTransform->Get_Angle());
			m_pTransformCom->Set_Angle(vPlayerAngle.x, vPlayerAngle.y, vPlayerAngle.z);
		}

		vPlayerScale = *(playerTransform->Get_Scale());
		m_pTransformCom->Set_Scale(vPlayerScale.x, vPlayerScale.y, vPlayerScale.z);

		CAnimator* playerAnimator = dynamic_cast<CAnimator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Animator"));
		int iCurIndex = playerAnimator->Get_MotionIndex();
		m_pAnimatorCom->Set_CurState(STATE_END, iCurIndex, iCurIndex, 1);
	}
}

void CItem::In_Inventory()
{
	Engine::CInventory* pPlayerInventory = dynamic_cast<Engine::CInventory*>
		(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	// 인벤토리에 들어갔다
	if (pPlayerInventory->Add_Item(this))
	{
		m_bActive = false;
		m_bDrop = false;
	}
}

//CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, MATERIAL _eMaterial, _vec3 vPos)
//{
//	CItem* pItem = new CItem(pGraphicDev);
//
//	if (FAILED(pItem->Ready_GameObject(_eMaterial, vPos)))
//	{
//		Safe_Release(pItem);
//		MSG_BOX("pItem Create Failed");
//		return nullptr;
//	}
//
//	return pItem;
//}

void CItem::Free()
{
	Engine::CGameObject::Free();
}
