#include "pch.h"
#include "../Header/Statue.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"
#include "..\Header\Core.h"

CStatue::CStatue(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_bActive(false)
{
	m_iLightNum = g_iLightNum++;
}

CStatue::~CStatue()
{
}

HRESULT CStatue::Ready_GameObject(_vec3 vPos, int iImgNum)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_iTextureNum = iImgNum * 2;

	return S_OK;
}

_int CStatue::Update_GameObject(const _float& fTimeDelta)
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

			if (pPlayer->Get_StatueUI())
			{
				pPlayer->Set_Statue();

				pPlayer->Set_Inventory();

				m_bCollision = false;
			}
		}
	}
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CStatue::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CStatue::Render_GameObject()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (!m_bActive)
	{
		m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
		FAILED_CHECK_RETURN(Setup_Material(), );
	}

	m_pTextureCom->Set_Texture(m_iTextureNum);

	m_pBufferCom->Render_Buffer();

	m_pColliderCom->Render_Collider();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CStatue::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		// 아이템 들어가면 코어의 bool을 true로 바꿔주는거
		/*CCore* pCore = dynamic_cast<CCore*>(Engine::Get_GameObject(L"Layer_Environment", L"Core"));
		pCore->Set_ActiveCore(m_iTextureNum / 2);
		m_iTextureNum++;*/

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

		if (m_bActive)
		{
			pPlayer->Set_Statue(m_iTextureNum, true);
		}
		else
			pPlayer->Set_Statue(m_iTextureNum);

		pPlayer->Set_Inventory();
	
		m_bCollision = true;
	}
}

HRESULT CStatue::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_StatueTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_StatueTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
	
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_NormalCubeCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

CStatue* CStatue::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, int iImgNum)
{
	CStatue* pCore = new CStatue(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos, iImgNum)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CStatue::Free()
{
	Engine::CGameObject::Free();
}
