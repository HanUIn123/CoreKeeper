#include "..\..\Header\Renderer.h"
#include "..\..\Header\Management.h"
#include "..\..\Header\Camera.h"
#include "../../Client/Header/DynamicCamera.h"
#include "../../Header/Export_System.h"
#include "../../Header/Export_Utility.h"
#include "../../Client/Header/Terrain.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
	:m_bExpandMinimap(false)
	, m_fZoomRatio(240.0f)
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
		pCamera->Set_Render(TYPE_PERSPECTIVE);

	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_Wall(pGraphicDev);

	// 미니맵 출력
	if (pCamera)
	{
		if (!dynamic_cast<CDynamicCamera*>(pCamera)->Get_IsWorldMap())
		{
			pGraphicDev->SetViewport(&m_MiniViewport);
			Render_MiniMap(pGraphicDev);
			pGraphicDev->SetViewport(&m_MainViewport);
		}
	}

	// UI 출력
	if (pCamera)
	{
		pCamera->Set_Render(TYPE_ORTHOGRAPHIC);
		if (!dynamic_cast<CDynamicCamera*>(pCamera)->Get_IsWorldMap())
		{
			Render_UI(pGraphicDev);

			Render_Subordinate(pGraphicDev);
		}
		else
			Render_WorldMap(pGraphicDev);
		pCamera->Set_Render(TYPE_PERSPECTIVE);
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

void CRenderer::Expand_MiniMap(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (!m_bExpandMinimap)
	{
		// 미니맵 확대
		m_MiniViewport.X = 430;
		m_MiniViewport.Y = 10;
		m_MiniViewport.Width = 700;
		m_MiniViewport.Height = 550;
		m_MiniViewport.MinZ = 0.0f;
		m_MiniViewport.MaxZ = 1.0f;

		m_bExpandMinimap = true;
	}
	else
	{
		// 미니맵 축소
		m_MiniViewport.X = 1020;
		m_MiniViewport.Y = 90;
		m_MiniViewport.Width = 200;
		m_MiniViewport.Height = 140;
		m_MiniViewport.MinZ = 0.0f;
		m_MiniViewport.MaxZ = 1.0f;

		m_bExpandMinimap = false;
	}
}

_float CRenderer::Get_ZoomRatio()
{
	return m_fZoomRatio;
}

_bool CRenderer::Get_ExpandMap()
{
	return m_bExpandMinimap;
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

void CRenderer::Render_Wall(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pGameObject : m_RenderGroup[RENDER_WALL])
		pGameObject->Render_GameObject();
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
	CDynamicCamera* pDynamicCamera = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Layer_Environment", L"DynamicCamera"));

	if (pPlayerTransform)
	{
		_vec3 playerPos;
		pPlayerTransform->Get_Info(INFO_POS, &playerPos);

		_vec3 vEye, vAt, vUp;

		if (!m_bExpandMinimap)
		{
			vEye = playerPos + _vec3(0.0f, 20.0f, 0.0f);
			vAt = playerPos;
			vUp = _vec3(0.0f, 0.0f, 1.0f);

			D3DXMATRIX matView;
			D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
			pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

			D3DXMATRIX matOrtho;
			D3DXMatrixOrthoLH(&matOrtho, 40.0f, 40.0f, 0.1f, 1000.0f);
			pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOrtho);
		}
		else
		{
			vEye = { 128.5f, 350.0f, 128.499f };
			vAt = { 128.5f,0,128.5f };
			playerPos.z -= 0.01f;
			vUp = _vec3(0.0f, 0.0f, 1.0f);

		/*	if (pDynamicCamera)
			{
				_long dwMouseWheel = Engine::Get_DIMouseMove(DIMS_Z);

				if (dwMouseWheel > 0)
				{
					m_fZoomRatio -= 10.0f;
					if (m_fZoomRatio < 100.0f)
						m_fZoomRatio = 100.0f;
				}
				else if (dwMouseWheel < 0)
				{
					m_fZoomRatio += 10.0f;
					if (m_fZoomRatio > 260.0f)
						m_fZoomRatio = 260.0f;
				}
			}*/

			D3DXMATRIX matView;
			D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
			pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

			D3DXMATRIX matOrtho;
			D3DXMatrixOrthoLH(&matOrtho, m_fZoomRatio, m_fZoomRatio, 0.1f, 1000.0f);
			pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOrtho);
		}
	}

	//for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	//{
	//	pGameObject->Render_GameObject();
	//}

	//for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	//{
	//	pGameObject->Render_GameObject();
	//}

	for (auto& pGameObject : m_RenderGroup[RENDER_MAP])
	{
		if (dynamic_cast<CTerrain*>(pGameObject))
		{
			dynamic_cast<CTerrain*>(pGameObject)->Set_Mini(true);
			pGameObject->Render_GameObject();
			dynamic_cast<CTerrain*>(pGameObject)->Set_Mini(false);
		}
		else
		{
			pGameObject->Render_GameObject();
		}
	}

	D3DXMATRIX matPerspect;
	D3DXMatrixPerspectiveFovLH(&matPerspect, D3DXToRadian(60), (float)WINCX / (float)WINCY, 0.1f, 1000.0f);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matPerspect);

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

//가장 후순위에 렌더할 대상(커서가 겹치는 현상때문에 추가합니다.)
void CRenderer::Render_Subordinate(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	for (auto& pGameObject : m_RenderGroup[RENDER_SUBORDINATE])
	{
		pGameObject->Render_GameObject();
	}

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
