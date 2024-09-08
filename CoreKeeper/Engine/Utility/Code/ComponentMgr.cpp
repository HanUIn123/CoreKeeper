#include "..\..\Header\ComponentMgr.h"

IMPLEMENT_SINGLETON(CComponentMgr)

CComponentMgr::CComponentMgr()
{
}

CComponentMgr::~CComponentMgr()
{
	Free();
}


CComponent* CComponentMgr::Clone_Proto(const _tchar * pComponentTag)
{
	CComponent*		pInstance = Find_Proto(pComponentTag);
	NULL_CHECK_RETURN(pInstance, nullptr);

	return pInstance->Clone();
}


HRESULT CComponentMgr::Ready_Proto(const _tchar * pComponentTag, CComponent * pComponent)
{
	CComponent*		pInstance = Find_Proto(pComponentTag);

	if (nullptr != pInstance)
		return E_FAIL;

	m_mapProtoType.insert({ pComponentTag, pComponent });
	
	return S_OK;
}


CComponent* CComponentMgr::Find_Proto(const _tchar * pComponentTag)
{
	auto		iter = find_if(m_mapProtoType.begin(), m_mapProtoType.end(), CTag_Finder(pComponentTag));

	if (iter == m_mapProtoType.end())
		return nullptr;

	return iter->second;
}

void CComponentMgr::Free()
{
	for_each(m_mapProtoType.begin(), m_mapProtoType.end(), CDeleteMap());
	m_mapProtoType.clear();
}
