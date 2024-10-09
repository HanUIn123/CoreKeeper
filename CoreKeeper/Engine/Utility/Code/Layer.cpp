#include "..\..\Header\Layer.h"
#include "..\..\Header\Renderer.h"
#include "..\..\Header\Export_Utility.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent * CLayer::Get_Component(COMPONENTID eID, const _tchar * pObjTag, const _tchar * pComponentTag)
{
	auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second->Get_Component(eID, pComponentTag);
}
CGameObject* CLayer::Get_GameObject(const _tchar* pObjTag)
{
	auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second;
}

HRESULT CLayer::Add_GameObject(const _tchar * pObjTag, CGameObject * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_mapObject.insert({ pObjTag, pGameObject });

	return S_OK;
}

HRESULT CLayer::Delete_GameObject(const _tchar* pObjTag, CGameObject* pGameObject)
{
	NULL_CHECK_RETURN(pObjTag, E_FAIL);

	auto iter = m_mapObject.find(pObjTag);
	if (iter == m_mapObject.end())
		return E_FAIL;

	CRenderer::GetInstance()->Delete_Renderer(RENDER_PRIORITY, iter->second);
	CRenderer::GetInstance()->Delete_Renderer(RENDER_ALPHA, iter->second);
	CRenderer::GetInstance()->Delete_Renderer(RENDER_WALL, iter->second);

	iter->second->Free_Component();
	Safe_Delete(iter->second);

	m_mapObject.erase(iter);

	return S_OK;
}


HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	_int		iResult(0);

	for (auto& pObj : m_mapObject)
	{
		iResult = pObj.second->Update_GameObject(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	return iResult;
}

void CLayer::LateUpdate_Layer()
{
	for (auto& pObj : m_mapObject)
		pObj.second->LateUpdate_GameObject();
}

void CLayer::Render_Layer()
{
	for (auto& pObj : m_mapObject)
		pObj.second->Render_GameObject();
}

void CLayer::Delete_GameMap(const _tchar* tObjTag)
{
	auto iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(tObjTag));

	if (iter == m_mapObject.end())
		return;

	CRenderer::GetInstance()->Delete_Renderer(RENDER_PRIORITY, iter->second);
	CRenderer::GetInstance()->Delete_Renderer(RENDER_ALPHA, iter->second);
	CRenderer::GetInstance()->Delete_Renderer(RENDER_WALL, iter->second);
	
	iter->second->Free_Component();
	Safe_Delete(iter->second);
	
	m_mapObject.erase(iter);
}

CLayer * CLayer::Create()
{
	CLayer*		pLayer = new CLayer;
	
	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("layer create failed");
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}
