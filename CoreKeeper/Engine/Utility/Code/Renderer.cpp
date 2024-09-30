#include "..\..\Header\Renderer.h"
#include "..\..\Header\Management.h"
#include "..\..\Header\Camera.h"
#include "../../Client/Header/DynamicCamera.h"
#include "../../Header/Export_System.h"
#include "../../Header/Export_Utility.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
	m_MainViewport.X = 0;
	m_MainViewport.Y = 0;
	m_MainViewport.Width = WINCX;
	m_MainViewport.Height = WINCY;
	m_MainViewport.MinZ = 0.0f;
	m_MainViewport.MaxZ = 1.0f;

	m_MiniViewport.X = 1020;
	m_MiniViewport.Y = 90;
	m_MiniViewport.Width = 200;
	m_MiniViewport.Height = 140;
	m_MiniViewport.MinZ = 0.0f;
	m_MiniViewport.MaxZ = 1.0f;
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eType, CGameObject * pGameObject)
{
	if (RENDER_END <= eType || nullptr == pGameObject)
		return;

	m_RenderGroup[eType].push_back(pGameObject);
	pGameObject->AddRef();
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetViewport(&m_MainViewport);
	CCamera* pCamera = dynamic_cast<CCamera*>(Engine::CManagement::GetInstance()->Get_GameObject(L"Layer_Environment", L"DynamicCamera"));

	// 원근 투영
	if (pCamera)
	{
		pCamera->Set_Render(TYPE_PERSPECTIVE);
	}
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);

	if (pCamera)
	{
		if (!dynamic_cast<CDynamicCamera*>(pCamera)->Get_IsWorldMap())
		{
		}
		else
		{
			//Render_WorldMap(pGraphicDev);
		}
	}

	if (pCamera)
	{
		if (!dynamic_cast<CDynamicCamera*>(pCamera)->Get_IsWorldMap())
		{
			pGraphicDev->SetViewport(&m_MiniViewport);
			Render_MiniMap(pGraphicDev);
		}
	}

	// UI 출력시에만 직교 투영하도록 정해줌
	if (pCamera)
	{
		pCamera->Set_Render(TYPE_ORTHOGRAPHIC);
	}

	if (pCamera)
	{
		if (!dynamic_cast<CDynamicCamera*>(pCamera)->Get_IsWorldMap())
			Render_UI(pGraphicDev);
		else
			Render_WorldMap(pGraphicDev);
	}
	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Delete_Renderer(RENDERID _eType, CGameObject* pGameObject)
{
	if (RENDER_END <= _eType || nullptr == pGameObject)
		return;
	
	for (auto iter : m_RenderGroup[_eType])
	{
		if (iter == pGameObject)
		{
			m_RenderGroup[_eType].remove(iter);
			return;
		}
	}
}

void CRenderer::Expand_MiniMap()
{

}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
		pGameObject->Render_GameObject();
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
		pGameObject->Render_GameObject();
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
		pGameObject->Render_GameObject();


	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


void CRenderer::Render_UIALPHA(LPDIRECT3DDEVICE9& pGraphicDev)
{
	
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& pGameObject : m_RenderGroup[RENDER_UIALPHA])
		pGameObject->Render_GameObject();


	pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_Window(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pGameObject : m_RenderGroup[RENDER_WINDOW])
		pGameObject->Render_GameObject();
}

void CRenderer::Render_MiniMap(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

	if (pPlayerTransform)
	{
		_vec3 playerPos;
		pPlayerTransform->Get_Info(INFO_POS, &playerPos);

		_vec3 vEye = playerPos + _vec3(0.0f, 20.0f, 0.0f);
		_vec3 vAt = playerPos;
		_vec3 vUp = _vec3(0.0f, 0.0f, 1.0f);

		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
		pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

		D3DXMATRIX matOrtho;
		D3DXMatrixOrthoLH(&matOrtho, 40.0f, 40.0f, 0.1f, 1000.0f);
		pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOrtho);
	}

	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		pGameObject->Render_GameObject();
	}

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	{
		pGameObject->Render_GameObject();
	}

	D3DXMATRIX matPerspective;
	D3DXMatrixPerspectiveFovLH(&matPerspective, D3DXToRadian(60), (float)WINCX / (float)WINCY, 0.1f, 1000.0f);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matPerspective);

	pGraphicDev->SetViewport(&m_MainViewport);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::Render_WorldMap(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	for (auto& pGameObject : m_RenderGroup[RENDER_MAP])
	{
		pGameObject->Render_GameObject();
	}

	pGraphicDev->SetViewport(&m_MainViewport);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	for (auto& pGameObject : m_RenderGroup[RENDER_UI])

		pGameObject->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
