#include "pch.h"
#include "../Header/StatueBase.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Core.h"

CStatueBase::CStatueBase(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_bActive(false)
{
}

CStatueBase::~CStatueBase()
{
}

HRESULT CStatueBase::Ready_GameObject(_vec3 vPos, int _iNum)
{
	m_iTextureNum = _iNum * 2;

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CStatueBase::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive)
	{
		CCore* pCore = dynamic_cast<CCore*>(Engine::Get_GameObject(L"Layer_Environment", L"Core"));
		m_bActive = pCore->Get_ActiveCore(m_iTextureNum/2);
		if(m_bActive)
			m_iTextureNum++;
	}
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CStatueBase::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CStatueBase::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(m_iTextureNum);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CStatueBase::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_StatueBaseTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_StatueBaseTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	return S_OK;
}

CStatueBase* CStatueBase::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, int _iNum)
{
	CStatueBase* pCore = new CStatueBase(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos, _iNum)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CStatueBase::Free()
{
	Engine::CGameObject::Free();
}
