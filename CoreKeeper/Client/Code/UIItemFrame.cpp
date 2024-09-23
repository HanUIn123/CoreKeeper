#include "pch.h"
#include "..\Header\UIItemFrame.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIItemFrame::CUIItemFrame(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bCollapse(false)

{
	m_vPos = { 0, 0 };
}

CUIItemFrame::~CUIItemFrame()
{
}

HRESULT CUIItemFrame::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);

	return S_OK;
}

_int CUIItemFrame::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{

		if (Engine::Get_DIMouseMove(DIMS_Z) < 0)
		{
			m_vPos.y += 8.f;
		}
		else if (Engine::Get_DIMouseMove(DIMS_Z) > 0)
		{
			m_vPos.y -= 8.f;
		}
	}
	return iExit;
}

void CUIItemFrame::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Add_RenderGroup(RENDER_UI, this);
	}
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIItemFrame::Render_GameObject()
{
	if (m_pItem)
	{
		m_pGraphicDev->SetScissorRect(&m_BRect);

		_matrix matWorld;

		m_pTransformCom->Get_WorldMatrix(&matWorld);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(150, 255, 255, 255));
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pSelTextureCom->Set_Texture();

		m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

		wstring sFont = L"³ª¹«";

		const _tchar* tFont = sFont.c_str();

		Engine::Render_Font(L"Font_Status", tFont, &m_vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}
}

void CUIItemFrame::Set_Window(CItem* _pItem)
{
	if (m_bWindow)
	{
		m_bWindow = false;

		m_pItem = nullptr;
	}
	else
	{
		m_bWindow = true;
	}
	m_pItem = _pItem;

	CTransform* pCursor = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_UI", L"UI_Cursor", L"Com_UITransform"));

	_vec3 vPos;
	pCursor->Get_Info(INFO_POS, &vPos);

	m_pTransformCom->Set_Pos(vPos.x + 10.f, vPos.y - 50.f, 0.f);

	m_vPos = { vPos.x + WINCX / 2.f - 80.f, WINCY / 2.f - vPos.y - 80.f};

	m_BRect.left = 0;
	m_BRect.right = WINCX;
	m_BRect.top = (WINCY / 2.f - vPos.y + 59.f) - (m_pTransformCom->Get_Scale()->y);
	m_BRect.bottom = (WINCY / 2.f - vPos.y + 42.5f) + (m_pTransformCom->Get_Scale()->y);
}

HRESULT CUIItemFrame::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pSelTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIItemFrame"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_StatusTexture", pComponent });
	

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIItemFrame* CUIItemFrame::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIItemFrame* pUIItemFrame = new CUIItemFrame(pGraphicDev);

	if (FAILED(pUIItemFrame->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIItemFrame);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIItemFrame;
}

void CUIItemFrame::Free()
{
	Engine::CGameObject::Free();
}
