#include "pch.h"
#include "../Header/Crystal.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/Azeos.h"
#include "../Header/Terrain.h"

CCrystal::CCrystal(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_pAzeos(nullptr), m_pAzeosTransform(nullptr), m_pPlayer(nullptr), m_pPlayerTransform(nullptr), m_pTerrain(nullptr), m_bDeadFirstFrame(true), m_iHp(80)
{
}

CCrystal::~CCrystal()
{
}

HRESULT CCrystal::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);

	return S_OK;
}

_int CCrystal::Update_GameObject(const _float& fTimeDelta)
{
	Set_Cast();
	if (m_iHp <= 0)
	{
		if (m_bDeadFirstFrame)
		{
			_vec3 vCrystalPos;
			m_pTransformCom->Get_Info(INFO_POS, &vCrystalPos);
			_int iIndex = _int(vCrystalPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCrystalPos.x + 0.5f * VTXITV);
			m_pTerrain->Set_Unreachable(iIndex, false);
			m_pTerrain->Set_Unreachable(iIndex - 1, false);
			m_pTerrain->Set_Unreachable(iIndex + 1, false);
		}
		return 0;
	}
	
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	if (m_pAzeos->Get_Crystal())
	{

		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);

		m_pAzeosTransform->Set_Pos(vPos.x, 3.f, vPos.z);

		m_pAzeos->Set_Crystal();
	}

	_vec3 vAzeosPos, vCrystalPos, vTotal;
	m_pAzeosTransform->Get_Info(INFO_POS, &vAzeosPos);
	m_pTransformCom->Get_Info(INFO_POS, &vCrystalPos);

	vTotal = vAzeosPos - vCrystalPos;

	if (D3DXVec3Length(&vTotal) < 4)
	{

	}

	_vec3 vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	CItem* pPlayerHandedItem = m_pPlayer->Get_HandedItem();

	if (m_pPlayer->Get_CurState() == SWING)
	{
		if (pPlayerHandedItem->Get_ItemNum() == ITEM_PICKAXE)
		{
			CColliderCube* pHandedItemCollider = dynamic_cast<CColliderCube*>(pPlayerHandedItem->Get_Component(ID_DYNAMIC, L"Com_ColliderCube"));
			m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

			if (m_pSphereColliderCom->Check_Cube_Collision(pHandedItemCollider))
			{
				m_iHp -= 10.f;
			}
		}
	}

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CCrystal::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CCrystal::Render_GameObject()
{
	if (m_iHp <= 0)
		return;
	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pSphereColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CCrystal::Set_Cast()
{
	if(!m_pAzeos)
		m_pAzeos = dynamic_cast<CAzeos*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Azeos"));
	if(!m_pAzeosTransform)
		m_pAzeosTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Azeos", L"Com_Transform"));
	if(!m_pPlayer)
		m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));
	if(!m_pPlayerTransform)
		m_pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	if(!m_pTerrain)
		m_pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
}

HRESULT CCrystal::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_ObjectTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CrystalTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pSphereColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ShroomManCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

CCrystal* CCrystal::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CCrystal* pCore = new CCrystal(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CCrystal::Free()
{
	Engine::CGameObject::Free();
}
