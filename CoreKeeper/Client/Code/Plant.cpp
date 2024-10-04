#include "pch.h"
#include "../Header/Plant.h"
#include "Export_System.h"
#include "Export_Utility.h"

CPlant::CPlant(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev)
{
}

CPlant::~CPlant()
{
}

HRESULT CPlant::Ready_GameObject(ITEMNUM _eItemNum, _int iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_float fX = (iIndex % (VTXCNTX - 1)) * VTXITV;
	_float fZ = (iIndex / (VTXCNTX - 1)) * VTXITV;

	m_pTransformCom->Set_Pos(fX, 0.2f, fZ);
	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
	m_eItemNum = _eItemNum;
	switch (_eItemNum)
	{
	case ITEM_BERRY_SEED:
		m_iTextureNum = 0;
		break;
	case ITEM_PEPPER_SEED:
		m_iTextureNum = 1;
		break;
	case ITEM_CARROT_SEED:
		m_iTextureNum = 2;
		break;
	case ITEM_FIBER_SEED:
		m_iTextureNum = 3;
		break;
	}

	return S_OK;
}

_int CPlant::Update_GameObject(const _float& fTimeDelta)
{
	Add_RenderGroup(RENDER_ALPHA, this);
	//m_pAnimatorCom->Update_Animation();
	_vec3 vUp;
	m_pTransformCom->Get_Info(INFO_UP, &vUp);

	if (m_pAnimatorCom->Get_MotionIndex() < 5)
	{
		m_pTransformCom->Move_Pos(&vUp, fTimeDelta, 0.025f);
	}
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPlant::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPlant::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(m_iTextureNum);
	m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pAnimBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlant::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlantTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_PlantAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CPlant* CPlant::Create(LPDIRECT3DDEVICE9 pGraphicDev,ITEMNUM _eItemNum, _int iIndex)
{
	CPlant* pPlant = new CPlant(pGraphicDev);

	if (FAILED(pPlant->Ready_GameObject(_eItemNum, iIndex)))
	{
		Safe_Release(pPlant);
		MSG_BOX("pPlant Create Failed");
		return nullptr;
	}

	return pPlant;
}

void CPlant::Free()
{
	Engine::CGameObject::Free();
}
