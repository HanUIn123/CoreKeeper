#include "pch.h"
#include "..\Header\HpDivider.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\UIHealth.h"

CHpDivider::CHpDivider(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iCurHp(0), m_iMaxHp(0), m_bFirst(true)
{
}

CHpDivider::~CHpDivider()
{
}

HRESULT CHpDivider::Ready_GameObject( _vec2 vPos, _vec2 vSize, _float _fLength)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->m_vScale = { vSize.x, vSize.y , 1.f };
	m_pTransformCom->Set_Pos(x, y, 0);

	m_fLength = _fLength;

	return S_OK;
}

_int CHpDivider::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	CUIHealth* pUI = dynamic_cast<CUIHealth*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Health"));
	NULL_CHECK_RETURN(pUI, -1);

	m_iMaxHp = pUI->Get_MaxHp();
	m_iCurHp = pUI->Get_CurHp();

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CHpDivider::LateUpdate_GameObject()
{
	/*
	Engine::CTransform* pUIHealth = dynamic_cast<Engine::CTransform*>
			(Engine::Get_Component(ID_DYNAMIC, L"Layer_UI", L"UI_Health_bar", L"Com_UITransform"));
		NULL_CHECK_RETURN(pUIHealth);

		_vec3 vPos;
		pUIHealth->Get_Info(INFO_POS, &vPos);

		m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

		m_bFirst = false;*/

	Engine::CGameObject::LateUpdate_GameObject();
}

void CHpDivider::Render_GameObject()
{
	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);


	m_pTextureCom->Set_Texture(2);

	_int iNum = m_iMaxHp / m_iCurHp;

	_float fInterval = (float)m_fLength / iNum;

	for (int i = 0; i < iNum; i++)
	{
	    matWorld._41 += fInterval;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pBufferCom->Render_Buffer();
	}
}

HRESULT CHpDivider::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIHealthTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	/*
	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);*/
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });
	
	return S_OK;
}

CHpDivider* CHpDivider::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _float fLength)
{
	CHpDivider* pHpDivider = new CHpDivider(pGraphicDev);

	if (FAILED(pHpDivider->Ready_GameObject(vPos, vSize, fLength)))
	{
		Safe_Release(pHpDivider);
		MSG_BOX("HpDivider Create Failed");
		return nullptr;
	}

	return pHpDivider;
}

void CHpDivider::Free()
{
	Engine::CGameObject::Free();
}
