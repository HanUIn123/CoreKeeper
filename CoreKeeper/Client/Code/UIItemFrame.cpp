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

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);

	m_BRect.left = _long(vPos.x - (vSize.x - 2.f));
	m_BRect.right = _long(vPos.x + (vSize.x - 2.f));
	m_BRect.top = _long(vPos.y - (vSize.y - 2.f));
	m_BRect.bottom = _long(vPos.y + (vSize.y - 2.f));

	m_vPos = { vPos.x - 240.f, vPos.y - 150.f };

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
		else if (Engine::Get_DIMouseMove(DIMS_Z) > 0 && m_vPos.y >= 350.f)
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

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(200, 255, 255, 255));
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

		_vec2 vFPos = m_vPos;
		wstring wsEmpty = L"";
		const wstring* sFont= m_pItem->Get_Explain();

		const _tchar* tFont[30];

		for (int i = 0; i < 30; i++)
		{
			tFont[i] = sFont[i].c_str();

			if (i == 0) {
				Engine::Render_Font(L"Font_Status", tFont[i], &vFPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

				vFPos.y += 20.f;
			}
			else
				Engine::Render_Font(L"Font_Status", tFont[i], &vFPos, D3DCOLOR_ARGB(255, 22, 22, 22));

			vFPos.y += 20.f;

			if (sFont[i] == wsEmpty)
			{
				break;
			}
		}

		if (m_pItem->Get_UseMet())
		{
			const wstring sMFont = m_pItem->Get_Meterial();

			const _tchar* tMFont = sMFont.c_str();

			Engine::Render_Font(L"Font_Status", tMFont, &vFPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}

		/*
		matWorld._41 = vFPos.x;
		matWorld._42 = vFPos.y;

		Engine::ITEMNUM eNum = m_pItem->Get_ItemNum();

		switch (eNum)
		{
		case ITEM_SEED:
			matWorld._11 = 10.f;
			matWorld._22 = 10.f;
			break;

		case ITEM_SWORD:
			matWorld._11 = 40.f;
			matWorld._22 = 40.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_WOOD:
			matWorld._11 = 12.f;
			matWorld._22 = 12.f;
			break;

		case ITEM_BOW:
			matWorld._11 = 45.f;
			matWorld._22 = 45.f;

			matWorld._42 += 2.f;
			break;

		case ITEM_HOE:
			matWorld._11 = 50.f;
			matWorld._22 = 50.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_PICKAXE:
			matWorld._11 = 50.f;
			matWorld._22 = 50.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_SHOVEL:
			matWorld._11 = 60.f;
			matWorld._22 = 60.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_STAFF:
			matWorld._11 = 35.f;
			matWorld._22 = 35.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_HELMET:
			matWorld._11 = 30.f;
			matWorld._22 = 30.f;

			matWorld._42 -= 13.f;
			break;

		case ITEM_CHEST:
			matWorld._11 = 30.f;
			matWorld._22 = 30.f;
			break;

		case ITEM_LEG:
			matWorld._11 = 40.f;
			matWorld._22 = 40.f;

			matWorld._42 += 15.f;
			break;

		dafault:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;
		}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pItem->Get_Texture()->Set_Texture();

		m_pItem->Get_Buffer()->Render_First();
		*/

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}
}

void CUIItemFrame::Set_Window(CItem* _pItem)
{
	m_bWindow = true;

	m_pItem = _pItem;
}

HRESULT CUIItemFrame::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pItemBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ItemBuffer", pComponent });

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
