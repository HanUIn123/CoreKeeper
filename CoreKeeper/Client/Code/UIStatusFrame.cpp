#include "pch.h"
#include "..\Header\UIStatusFrame.h"

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
			if (Engine::Get_DIMouseMove(DIMS_Z) < 0 && m_vPos.y <= 200.f)
			{
				//if(m_vPos.y >= 80.f)
					m_vPos.y += 8.f;
			}
			else if (Engine::Get_DIMouseMove(DIMS_Z) > 0 && m_vPos.y >= 70.f)
			{
				//if (m_vPos.y >= 60.f)
					m_vPos.y -= 8.f;
			}
			m_bCollapse = true;
		}
		else
			m_bCollapse = false;
	}
	return iExit;
}

void CUIStatusFrame::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Add_RenderGroup(RENDER_UI, this);
	}
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

	matWorld._11 += 20.f;
	matWorld._22 += 20.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

	_vec2 vTPos[3] = { m_vPos, m_vPos, m_vPos };
	_vec2 vSPos[3] = { m_vPos, m_vPos, m_vPos };

	wstring sHTitleFont = L"체력 ";
	//vTPos[0].y = vTPos[0].y + 40.f;
	wstring sDTitleFont = L"방어";
	vTPos[1].y = vTPos[1].y + 80.f;
	wstring sATitleFont = L"공격";
	vTPos[2].y = vTPos[2].y + 160.f;


	wstring sHStatFont = L"최대체력 : " + to_wstring(m_pStat->iMaxHp);
	vSPos[0].y = vSPos[0].y + 40.f;

	wstring sDStatFont = L"방어력 : " + to_wstring(m_pStat->iDefense);
	vSPos[1].y = vSPos[1].y + 120.f;

	wstring sAStatFont = L"공격력 : " + to_wstring(m_pStat->iAttack);
	vSPos[2].y = vSPos[2].y + 200.f;

	const _tchar* tHTitleFont = sHTitleFont.c_str();
	const _tchar* tDTitleFont = sDTitleFont.c_str();
	const _tchar* tATitleFont = sATitleFont.c_str();

	const _tchar* tHStatFont = sHStatFont.c_str();
	const _tchar* tDStatFont = sDStatFont.c_str();
	const _tchar* tAStatFont = sAStatFont.c_str();

	Engine::Render_Font(L"Font_Status", tHTitleFont, &vTPos[0], D3DCOLOR_ARGB(255, 56, 49, 28));
	Engine::Render_Font(L"Font_Status", tDTitleFont, &vTPos[1], D3DCOLOR_ARGB(255, 56, 49, 28));
	Engine::Render_Font(L"Font_Status", tATitleFont, &vTPos[2], D3DCOLOR_ARGB(255, 56, 49, 28));

	Engine::Render_Font(L"Font_Status", tHStatFont, &vSPos[0], D3DCOLOR_ARGB(255, 256, 256, 256));
	Engine::Render_Font(L"Font_Status", tDStatFont, &vSPos[1], D3DCOLOR_ARGB(255, 256, 256, 256));
	Engine::Render_Font(L"Font_Status", tAStatFont, &vSPos[2], D3DCOLOR_ARGB(255, 256, 256, 256));

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
