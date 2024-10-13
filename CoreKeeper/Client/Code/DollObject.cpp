#include "pch.h"
#include "../Header/DollObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"

CDollObject::CDollObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_bCheck(false), m_pPlayer(nullptr), m_bIsPlaying(false), m_bPlayOnce(false)
{
}

CDollObject::~CDollObject()
{
}

HRESULT CDollObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CDollObject::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_pPlayer)
		m_pPlayer = dynamic_cast<CPlayer*>(Get_GameObject(L"Layer_GameLogic", L"Player"));

	Update_Texture();

	if (Check_Interaction())
	{
		Interaction();
	}
	else if (!Check_Interaction())
	{
		if (m_bCollision)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

			if (pPlayer->Get_ChestInv() && pPlayer->Get_Inventory())
			{
				pPlayer->Set_ChestInventory();

				//pPlayer->Set_Inventory();
			}

			m_bCollision = false;
		}
	}

	Play_Instrument();
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CDollObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CDollObject::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(m_iTextureNum);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDollObject::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

		pPlayer->Set_ChestInventory(m_pInventoryCom);

		//pPlayer->Set_Inventory();

		m_bCollision = true;
	}
}

HRESULT CDollObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_DollTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DollTexture"));
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

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_OneSlotInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	return S_OK;
}

void CDollObject::Update_Texture()
{
	if (m_pInventoryCom->Check_Empty(0))
		m_iTextureNum = 0;
	else
	{
		ITEMNUM eItemNum = m_pInventoryCom->Get_Item(0)->Get_ItemNum();

		m_iTextureNum = eItemNum - ITEM_INSTRUMENT_HARP + 1;
	}
}

void CDollObject::Play_Instrument()
{
	// 인벤토리 비어있으면 정지
	if (m_pInventoryCom->Check_Empty(0))
	{
		m_bIsPlaying = false;
		m_bPlayOnce = false;
	}
	else
	{
		// 인벤토리에 악기가 들어와있고 플레이어가 연주 중이면 연주 시작
		if (m_pPlayer->Get_InstrumentToggle())
			m_bIsPlaying = true;
		else
		{
			m_bIsPlaying = false;
			m_bPlayOnce = false;
		}
	}

	if (m_bIsPlaying)
	{
		if (!m_bPlayOnce)
		{
			m_bPlayOnce = true;
			ITEMNUM eItemNum = m_pInventoryCom->Get_Item(0)->Get_ItemNum();
			switch (eItemNum)
			{
			case ITEM_INSTRUMENT_HARP:
				Engine::CSoundMgr::GetInstance()->PlayOnce(L"harpItsABigWorldOutside.wav", SOUND_INSTRUMENTS_HARP, 0.3f);
				break;
			case ITEM_INSTRUMENT_CELLO:
				Engine::CSoundMgr::GetInstance()->PlayOnce(L"celloItsABigWorldOutside.wav", SOUND_INSTRUMENTS_CELLO, 0.6f);
				break;
			case ITEM_INSTRUMENT_FLUTE:
				Engine::CSoundMgr::GetInstance()->PlayOnce(L"fluteItsABigWorldOutside.wav", SOUND_INSTRUMENTS_FLUTE, 0.4f);
				break;
			case ITEM_INSTRUMENT_OCARINA:
				Engine::CSoundMgr::GetInstance()->PlayOnce(L"ocarinaItsABigWorldOutside.wav", SOUND_INSTRUMENTS_OCARINA, 0.3f);
				break;
			case ITEM_INSTRUMENT_DRUM:
				Engine::CSoundMgr::GetInstance()->PlayOnce(L"drumsItsABigWorldOutside.wav", SOUND_INSTRUMENTS_DRUM, 0.8f);
				break;
			}
		}
	}
}

CDollObject* CDollObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CDollObject* pCore = new CDollObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CDollObject::Free()
{
	Engine::CGameObject::Free();
}
