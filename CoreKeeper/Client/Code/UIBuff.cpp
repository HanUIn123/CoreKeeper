#include "pch.h"
#include "..\Header\UIBuff.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIBuff::CUIBuff(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_eBuffIndex(BUFF), m_eBuffType(BUFF_HEAL), m_fCurTime(0.f), m_fBuffTime(50.f), m_fTime(0.f)

{
}

CUIBuff::~CUIBuff()
{
}

HRESULT CUIBuff::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	m_BRect.left = vPos.x - vSize.x;
	m_BRect.right = vPos.x + vSize.x;
	m_BRect.top = vPos.y - vSize.y;
	m_BRect.bottom = vPos.y + vSize.y;

	m_Rect = m_BRect;

	m_pRangeBufferCom->Set_Height(100);
	m_pRangeBufferCom->Set_Width(200);

	return S_OK;
}

_int CUIBuff::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (Map_Picked(pt))
		{
			m_bCollapse = true;
		}
		else
			m_bCollapse = false;

		if (m_fCurTime >= 0)
		{
			m_fCurTime -= fTimeDelta * 5.f;

			m_Rect.top = m_fTime + ((_float)(m_BRect.bottom - m_BRect.top) -(_float(m_BRect.bottom - m_BRect.top) * m_fCurTime / m_fBuffTime));

			//m_BRect.top = m_fTime;
		}
		else
		{
			m_bWindow = false;
		}
		Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CUIBuff::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIBuff::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(m_eBuffIndex);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphicDev->SetScissorRect(&m_Rect);

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	//m_pRangeBufferCom->Set_Height(_int(m_fCurTime));

	//m_pRangeBufferCom->Render_Buffer();

	m_pAnimBufferCom->Set_Index(m_eBuffType);
	 
	m_pBuffTextureCom->Set_Texture(0);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pAnimBufferCom->Render_Buffer();
}

void CUIBuff::Set_Window(BUFFTYPE _eType, BUFFSLOTTYPE _eSlotType, _float _fTime) 
{
	if (m_bWindow)
	{
		m_bWindow = false;
	}
	else
	{
		m_bWindow = true;

		m_eBuffType = _eType;
		m_eBuffIndex = _eSlotType;

		m_fBuffTime = _fTime;

		m_fTime = (_float)m_BRect.top;

		m_fCurTime = m_fBuffTime;
	}
}

HRESULT CUIBuff::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pRangeBufferCom = dynamic_cast<CRangeTex*>(Engine::Clone_Proto(L"Proto_RangeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_RangeBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIBuffSlot"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pBuffTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_Buff"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_BuffTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_BuffIcons"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Buff", pComponent });
	return S_OK;
}

CUIBuff* CUIBuff::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIBuff* pUIBuff = new CUIBuff(pGraphicDev);

	if (FAILED(pUIBuff->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIBuff);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIBuff;
}

void CUIBuff::Free()
{
	Engine::CGameObject::Free();
}
