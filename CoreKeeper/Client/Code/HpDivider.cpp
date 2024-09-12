#include "pch.h"
#include "..\Header\HpDivider.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\UIHealth.h"

CHpDivider::CHpDivider(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev),  m_bActive(false)
{
}

CHpDivider::~CHpDivider()
{
}

HRESULT CHpDivider::Ready_GameObject(_vec2 vPos, _vec2 vSize,  _int iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	
	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->m_vScale = { vSize.x, vSize.y , 1.f }; 

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, 0);
	m_vPos = { x, y };

	m_iIndex = iIndex;

	return S_OK;
}

_int CHpDivider::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CHpDivider::LateUpdate_GameObject()
{
	if (m_bActive)
	{
		/*
		CUIHealth* pUI = dynamic_cast<CUIHealth*>
			(Engine::Get_GameObject(L"Layer_UI", L"UI_Health"));
		NULL_CHECK_RETURN(pUI, -1);

		m_iMaxHp = pUI->Get_MaxHp();
		m_iCurHp = pUI->Get_CurHp();
		*/

		Add_RenderGroup(RENDER_UI, this);
	}

	Engine::CGameObject::LateUpdate_GameObject();
}

void CHpDivider::Render_GameObject()
{
	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pTextureCom->Set_Texture(m_iIndex);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBufferCom->Render_Buffer();
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

void CHpDivider::Calculate_Pos(_float _fCurLength, _int _iCurHp, _int _iMaxHp)
{
	/*
	_int iCount;
	if (_iCurHp % 25 == 0)
	{
		iCount = _iMaxHp / 25 ;
	}
	else
		 iCount = _iMaxHp / 25 + 1;

	_float fCurLength = m_fLength / iCount;
	fCurLength *= iIndex;
	*/

	m_pTransformCom->Set_Pos(m_vPos.x + _fCurLength , m_vPos.y , 0.f);

	m_bActive = true;
}

CHpDivider* CHpDivider::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _int iIndex)
{
	CHpDivider* pHpDivider = new CHpDivider(pGraphicDev);

	if (FAILED(pHpDivider->Ready_GameObject(vPos, vSize, iIndex)))
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
