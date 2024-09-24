#include "pch.h"
#include "..\Header\UIStatusFrame.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIStatusFrame::CUIStatusFrame(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bCollapse(false)

{
}

CUIStatusFrame::~CUIStatusFrame()
{
}

HRESULT CUIStatusFrame::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);

	m_BRect.left = vPos.x - (vSize.x - 5.f);
	m_BRect.right = vPos.x + (vSize.x - 5.f);
	m_BRect.top = vPos.y - (vSize.y - 5.f);
	m_BRect.bottom = vPos.y + (vSize.y - 5.f);

	m_vPos.x = vPos.x -= 110.f;
	m_vPos.y = vPos.y -= 130.f;

	return S_OK;
}

_int CUIStatusFrame::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (Map_Picked(pt))
		{
			if (Engine::Get_DIMouseMove(DIMS_Z) < 0)
			{
				//if(m_vPos.y >= 80.f)
					m_vPos.y += 8.f;
			}
			else if (Engine::Get_DIMouseMove(DIMS_Z) > 0)
			{
				//if (m_vPos.y >= 60.f)
					m_vPos.y -= 8.f;
			}
			m_bCollapse = true;
		}
		else
			m_bCollapse = false;

		Add_RenderGroup(RENDER_UI, this);
	}
	return iExit;
}

void CUIStatusFrame::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIStatusFrame::Render_GameObject()
{
	m_pGraphicDev->SetScissorRect(&m_BRect);

	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pSelTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

	wstring sFont = L"총 아이템 레벨 : ";

	const _tchar* tFont = sFont.c_str();

	Engine::Render_Font(L"Font_Status", tFont, &m_vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

HRESULT CUIStatusFrame::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pSelTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIStatusFrame"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_StatusTexture", pComponent });
	

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIStatusFrame* CUIStatusFrame::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIStatusFrame* pUIStatusFrame = new CUIStatusFrame(pGraphicDev);

	if (FAILED(pUIStatusFrame->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIStatusFrame);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIStatusFrame;
}

void CUIStatusFrame::Free()
{
	Engine::CGameObject::Free();
}
