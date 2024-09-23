#include "pch.h"
#include "..\Header\Item.h"
#include "Export_System.h"
#include "Export_Utility.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iTextureNumber(0), m_fFirstY(0.f), m_fTimeAcc(0.f), m_fSpeed(0.5f), m_bActive(true), m_bDrop(false), m_bDropSelf(false), m_bUse(false), m_bSwing(false), m_iCount(1), m_bHasRotated(false), m_fAngle(0.f)
{
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
			Engine::CInventory* pPlayerInventory = dynamic_cast<Engine::CInventory*>
				(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

			// 인벤토리에 들어갔다
			if (pPlayerInventory->Add_Item(this))
			{
				m_bActive = false;
				m_bDrop = false;
			}
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
	// 카메라를 바라보게 하면서 스케일 유지
	//Apply_Billboard();  

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

void CItem::Apply_Billboard()
{
	D3DXMATRIX matWorld, matView, matBill, matScale;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// 스케일 행렬을 따로 계산
	D3DXMatrixScaling(&matScale, m_pTransformCom->Get_Scale()->x, m_pTransformCom->Get_Scale()->y, m_pTransformCom->Get_Scale()->z);
	
	D3DXMATRIX matInverseScale;
	D3DXMatrixInverse(&matInverseScale, 0, &matScale);

	// 최종 월드 행렬: 스케일 적용 후 빌보드 회전 적용
	D3DXMATRIX matFinal = matScale * matBill * matInverseScale * matWorld;

	m_pTransformCom->Set_WorldMatrix(&matFinal);
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
				m_pTransformCom->Rotation(ROT_X, D3DXToRadian(m_fAngleX));
				m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(m_fAngle));
				m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(m_fAngleY));
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

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CItem* pItem = new CItem(pGraphicDev);

	if (FAILED(pItem->Ready_GameObject(vPos)))
	{
		Safe_Release(pItem);
		MSG_BOX("pItem Create Failed");
		return nullptr;
	}

	return pItem;
}

void CItem::Free()
{
	Engine::CGameObject::Free();
}
