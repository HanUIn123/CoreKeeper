#include "pch.h"
#include "..\Header\UIHealth.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIHealth::CUIHealth(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bExit(false)

{
}

CUIHealth::~CUIHealth()
{
}

HRESULT CUIHealth::Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&ButtonWorld);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = Viewport.Height;
	float width = Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->m_vScale = { vSize.x, vSize.y , 1.f };
	m_pTransformCom->Set_Pos(x, y, 0);

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_iIndex = iIndex;

	return S_OK;
}

_int CUIHealth::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);


	if (Map_Picked(pt))
	{
		if (Engine::Get_DIMouseState(DIM_LB))
		{
			/*
			_vec3	vRight;
			m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), 1.f, 10.f);
			*/

			// 충돌 확인용 코드

		
			//인덱스에 따라 출력되는 창 변경
		}

		m_bCollapse = true;
	}
	else
		m_bCollapse = false;

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIHealth::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIHealth::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	//인벤토리 오픈했을때 출력
	/*
	if (m_bExit)
		m_pBufferCom->Set_Index(3);
	else
		m_pBufferCom->Set_Index(m_iIndex);
		*/
	//현재 마우스와 충돌중일때 출력
	
	//if (!m_bCollapse)
		m_pTextureCom->Set_Texture(m_iIndex);
	/*else
	
	m_pColTextureCom->Set_Texture(m_iIndex);
	*/
	m_pBufferCom->Render_Buffer();
}

HRESULT CUIHealth::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_UIScreenIconTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIHealthTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CUIHealth* CUIHealth::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	CUIHealth* pUIHealth = new CUIHealth(pGraphicDev);

	if (FAILED(pUIHealth->Ready_GameObject(vPos, vSize, iIndex)))
	{
		Safe_Release(pUIHealth);
		MSG_BOX("UIHealth Create Failed");
		return nullptr;
	}

	return pUIHealth;
}

void CUIHealth::Free()
{
	Engine::CGameObject::Free();
}
