#include "pch.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/CraftMgr.h"

CUIBuffFrame::CUIBuffFrame(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bCollapse(false), m_bCraft(false)

{
	m_vPos = { 0, 0 };
}

CUIBuffFrame::~CUIBuffFrame()
{
}

HRESULT CUIBuffFrame::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	Set_BuffExplain();

	//m_vPos = { vPos.x - 240.f, vPos.y - 150.f };

	return S_OK;
}

_int CUIBuffFrame::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

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

void CUIBuffFrame::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIBuffFrame::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pSelTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	auto iter = m_mapType.find(m_eType);

	const _tchar* tChar = (iter->second).c_str();

	_vec3 vPos3 = {};

	m_pTransformCom->Get_Info(INFO_POS, &vPos3);
	_vec2 vPos2 = { vPos3.x + WINCX/ 2.f - 120.f, WINCY / 2.f - vPos3.y - 20.f };

	Engine::Render_Font(L"Font_Item", tChar, &vPos2, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CUIBuffFrame::Set_Window(CUIBuff::BUFFICONTYPE _BuffType, POINT _pt, _bool _bCraft)
{
	m_bWindow = true;

	m_bCraft = _bCraft;

	m_eType = _BuffType;

	m_pTransformCom->Set_Pos((_float)_pt.x + 130.f - WINCX / 2.f, WINCY / 2.f - (_float)_pt.y - 10.f, 0.f);
}

void CUIBuffFrame::Set_BuffExplain()
{
	wstring string = L"최대 체력 \n+5.0% 증가";
	m_mapType.insert({ 0, string });

	string = L"피해 \n+5.0% 증가";
	m_mapType.insert({ 1, string });

	string = L"체굴 피해 \n+50 증가";
	m_mapType.insert({ 6, string });

	string = L"방어력 \n+5.0% 증가";
	m_mapType.insert({ 15, string });

	string = L"허기로 인해 \n피해 및 최대 체력 -5.0% 감소";
	m_mapType.insert({ 17, string });

	string = L"+10의 화상 피해";
	m_mapType.insert({ 24, string });

	string = L"이동속도 증가";
	m_mapType.insert({ 29, string });

	string = L"슬라임으로 인해 \n이동속도 -40.0%";
	m_mapType.insert({ 30, string });

	string = L"포만감으로 인해 \n피해 및 최대 체력 +5.0% 증가";
	m_mapType.insert({ 31, string });

	string = L"잠시동안 이동 불가";
	m_mapType.insert({ 38, string });

	string = L"신세계의 \n신이 되는 거다!";
	m_mapType.insert({ 40, string });
}

HRESULT CUIBuffFrame::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pItemBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ItemBuffer", pComponent });

	pComponent = m_pSelTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIBuffFrame"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_StatusTexture", pComponent });
	

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIBuffFrame* CUIBuffFrame::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIBuffFrame* pUIItemFrame = new CUIBuffFrame(pGraphicDev);

	if (FAILED(pUIItemFrame->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIItemFrame);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIItemFrame;
}

void CUIBuffFrame::Free()
{
	Engine::CGameObject::Free();
}
