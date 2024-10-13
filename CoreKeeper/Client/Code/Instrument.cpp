#include "pch.h"
#include "..\Header\Instrument.h"
#include "Export_System.h"
#include "Export_Utility.h"

CInstrument::CInstrument(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CInstrument::~CInstrument()
{
}

HRESULT CInstrument::Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eItemNum = _eItemNum;
	m_iTextureNumber = m_eItemNum - ITEM_INSTRUMENT_HARP;
	m_iFrameCount = 0;

	switch (m_eItemNum)
	{
	case ITEM_INSTRUMENT_HARP:
		m_wItemName = L"하프";
		m_wItemExplain[0] = L"섬세한 현을 손끝으로 스치기만 해도";
		m_wItemExplain[1] = L"공기 중에 조화를 이루는 소리가 진동합니다.";
		break;
	case ITEM_INSTRUMENT_CELLO:
		m_wItemName = L"첼로";
		m_wItemExplain[0] = L"나무로 만들어진 현악기입니다.";
		m_wItemExplain[1] = L"세계 일류 작품은 아니지만";
		m_wItemExplain[2] = L"조용한 동굴을 하모니로 채우기에는 충분하지요.";
		break;
	case ITEM_INSTRUMENT_FLUTE:
		m_wItemName = L"플루트";
		m_wItemExplain[0] = L"휘파람 같은 소리를 내는 섬세한 관악기입니다.";
		m_wItemExplain[1] = L"숙달하기에는 어려울 수 있습니다.";
		break;
	case ITEM_INSTRUMENT_OCARINA:
		m_wItemName = L"오카리나";
		m_wItemExplain[0] = L"세월이 흘러도 변치 않는 악기입니다.";
		break;
	case ITEM_INSTRUMENT_DRUM:
		m_wItemName = L"드럼 키트";
		m_wItemExplain[0] = L"이웃을 짜증나게 하지만 올바르게 연주하면";
		m_wItemExplain[1] = L"매혹적인 리듬을 만들어 내는 악기입니다.";
		break;
	case ITEM_INSTRUMENT_PIANO:
		m_wItemName = L"휴대용 피아노";
		m_wItemExplain[0] = L"이 휴대용 건반 악기는 홀로 연주할 수도 있고,";
		m_wItemExplain[1] = L"오케스트라와 함께 연주해도 좋습니다.";
		break;
	}

	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);


	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	return S_OK;
}

_int CInstrument::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		m_bActive = true;
		m_bDrop = false;
		// m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
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

void CInstrument::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CInstrument::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);

	m_pBufferCom->Set_Index(m_iFrameCount);

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

HRESULT CInstrument::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_InstrumentAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_InstrumentTexture"));
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

CInstrument* CInstrument::Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eItemNum, _vec3 vPos)
{
	CInstrument* pInstrument = new CInstrument(pGraphicDev);

	if (FAILED(pInstrument->Ready_GameObject(_eItemNum, vPos)))
	{
		Safe_Release(pInstrument);
		MSG_BOX("pInstrument Create Failed");
		return nullptr;
	}

	return pInstrument;
}

void CInstrument::Free()
{
	Engine::CGameObject::Free();
}
