#include "pch.h"
#include "..\Header\UICraft.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\UICraftButton.h"

CUICraft::CUICraft(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_eTableType(TABLE_PLAYER)

{
}

CUICraft::~CUICraft()
{
}

HRESULT CUICraft::Ready_GameObject(_vec2 vPos, _vec2 vSize, _bool _bLRCheck)
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

	m_bLRCheck = _bLRCheck;

	return S_OK;
}

_int CUICraft::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CUICraft::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUICraft::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pTextureCom->Set_Texture(1);

	if (m_eTableType == TABLE_CRAFT || m_eTableType == TABLE_ANVIL)
	{
		m_pTextureCom->Set_Texture(1);

		matWorld._11 += 30.f;
	    matWorld._41 += 20.f;
	}
	else
		m_pTextureCom->Set_Texture();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBufferCom->Render_Buffer();

	matWorld._11 = 60.f;
	matWorld._22 = 30.f;
	matWorld._42 += 120.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	_int iIndex = Check_Index();

	m_pTitleTextureCom->Set_Texture(iIndex);

	m_pTitleBufferCom->Render_Buffer();
}

void CUICraft::Set_Window(TABLETYPE _eType, MATERIAL _eMaterial)
{
	if (m_bWindow)
	{
		CUICraftButton* pButton = dynamic_cast<CUICraftButton*>(Engine::Get_GameObject(L"Layer_UI", L"UI_CraftButton"));

		pButton->Set_DisableWindow();

		m_bWindow = false;
	}
	else
	{
		m_bWindow = true;

		m_eTableType = _eType;

		eTableMaterial = _eMaterial;

		if (_eType == TABLE_CRAFT || _eType == TABLE_ANVIL)
		{
			CUICraftButton* pButton = dynamic_cast<CUICraftButton*>(Engine::Get_GameObject(L"Layer_UI", L"UI_CraftButton"));

			pButton->Set_Window(_eType, _eMaterial);
		}
	}
}

_int CUICraft::Check_Index()
{
	_int iIndex(0);

	if (m_bLRCheck)
	{
		switch (m_eTableType)
		{
		case TABLE_PLAYER:
			iIndex = 0;
			break;

		case TABLE_CRAFT:
			iIndex = 3;
			break;

		case TABLE_ANVIL:
			iIndex = 1;
			break;

		case TABLE_ALCHEMY:
			iIndex = 3;
			break;

		case TABLE_MUSIC:
			iIndex = 0;
		}
	}
	else if (!m_bLRCheck)
	{
		switch (m_eTableType)
		{
		case TABLE_CRAFT:
			iIndex = 0;
			break;

		case TABLE_ANVIL:
			iIndex = 2;
			break;
		}
	}

	return iIndex;
}

HRESULT CUICraft::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTitleBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_TitleBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UICraft"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTitleTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UICraftTitle"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_TitleTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUICraft* CUICraft::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _bool _bLRCheck)
{
	CUICraft* pUICraft = new CUICraft(pGraphicDev);

	if (FAILED(pUICraft->Ready_GameObject(vPos, vSize, _bLRCheck)))
	{
		Safe_Release(pUICraft);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUICraft;
}

void CUICraft::Free()
{
	Engine::CGameObject::Free();
}
