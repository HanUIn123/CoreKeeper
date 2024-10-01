#include "pch.h"
#include "..\Header\UICraftButton.h"
#include "..\Header\UICraft.h"
#include "..\Header\UICraftSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUICraftButton::CUICraftButton(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_eTableType(TABLE_PLAYER), m_bDownCollision(false), m_bUpCollision(false)

{
}

CUICraftButton::~CUICraftButton()
{
}

HRESULT CUICraftButton::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y , 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);
	m_UpRect   = { (_long)(vPos.x - vSize.x), (_long)(vPos.y - 50.f - vSize.y), (_long)(vPos.x + vSize.x), (_long)(vPos.y - 50.f + vSize.y) };
	m_DownRect = { (_long)(vPos.x - vSize.x), (_long)(vPos.y + 50.f - vSize.y), (_long)(vPos.x + vSize.x), (_long)(vPos.y + 50.f + vSize.y) };

	return S_OK;
}

_int CUICraftButton::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (UPRECT_Picked(pt))
		{
			m_bUpCollision = true;
			if (Engine::Button_Down(DIM_LB))
			{
				CUICraft* pCraft = dynamic_cast<CUICraft*>(Engine::Get_GameObject(L"Layer_UI", L"UILeftCraft"));

				if (eTableMaterial < pCraft->Get_Material())
				{
					_int i = eTableMaterial;
					i++;

					eTableMaterial = static_cast<MATERIAL>(i);

					Set_Slot();
				}
			}
		}
		else if (!UPRECT_Picked(pt))
		{
			m_bUpCollision = false;
		}

		if (DOWNRECT_Picked(pt))
		{
			m_bDownCollision = true;
			if (Engine::Button_Down(DIM_LB))
			{
				CUICraft* pCraft = dynamic_cast<CUICraft*>(Engine::Get_GameObject(L"Layer_UI", L"UILeftCraft"));

				if (eTableMaterial > 0)
				{
					_int i = eTableMaterial;
					i--;

					eTableMaterial = static_cast<MATERIAL>(i);

					Set_Slot();
				}
			}
		}
		else if (!DOWNRECT_Picked(pt))
		{
			m_bDownCollision = false;
		}
	}
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CUICraftButton::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUICraftButton::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(eTableMaterial);

	m_pBufferCom->Render_Buffer();

	matWorld._42 -= 50.f;

	matWorld._11 = 20.f;
	matWorld._22 = 20.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_bDownCollision)
	{
		m_pColTextureCom->Set_Texture(1);

		m_pDColBufferCom->Render_Buffer();
	}

	matWorld._11 = 15.f;
	matWorld._22 = 15.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pArrowTextureCom->Set_Texture(1);

	m_pDArrowBufferCom->Render_Buffer();

	matWorld._42 += 100.f;

	matWorld._11 = 20.f;
	matWorld._22 = 20.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_bUpCollision)
	{
		m_pColTextureCom->Set_Texture(1);

		m_pDColBufferCom->Render_Buffer();
	}

	matWorld._11 = 15.f;
	matWorld._22 = 15.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pArrowTextureCom->Set_Texture(4);

	m_pDArrowBufferCom->Render_Buffer();

}

HRESULT CUICraftButton::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pUArrowBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_UArrowBuffer", pComponent });

	pComponent = m_pDArrowBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_DArrowBuffer", pComponent });

	pComponent = m_pUColBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_UColBuffer", pComponent });

	pComponent = m_pDColBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_DColBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITable"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pArrowTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITrashCan"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ArrowTexture", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

void CUICraftButton::Set_Slot()
{

	for (int i = 0; i < 6; i++)
	{
		wstring string;

		string = L"UICraftLSlot_" + std::to_wstring(i);

		CUICraftSlot* pSlot = dynamic_cast<CUICraftSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pSlot->Set_DisableWindow();
		pSlot->Set_Window(m_eTableType, eTableMaterial, true);
	}
}

CUICraftButton* CUICraftButton::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUICraftButton* pUICraft = new CUICraftButton(pGraphicDev);

	if (FAILED(pUICraft->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUICraft);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUICraft;
}

void CUICraftButton::Free()
{
	Engine::CGameObject::Free();
}
