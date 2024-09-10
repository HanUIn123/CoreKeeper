#include "..\..\Header\Renderer.h"
#include "..\..\Header\Management.h"
#include "..\..\Header\Camera.h"
IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
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
	CCamera* pCamera = dynamic_cast<CCamera*>(Engine::CManagement::GetInstance()->Get_GameObject(L"Layer_Environment", L"DynamicCamera"));

	// 원근 투영
	if (pCamera)
	{
		pCamera->Set_Render(TYPE_PERSPECTIVE);
	}
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	// UI 출력시에만 직교 투영하도록 정해줌
	if (pCamera)
	{
		pCamera->Set_Render(TYPE_ORTHOGRAPHIC);
	}
	Render_UI(pGraphicDev);

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
	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
		pGameObject->Render_GameObject();


	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
		pGameObject->Render_GameObject();
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
