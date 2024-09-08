#pragma once

#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComponentMgr : public CBase
{
	DECLARE_SINGLETON(CComponentMgr)

private:
	CComponentMgr();
	virtual ~CComponentMgr();

public:
	HRESULT			Ready_Proto(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*		Clone_Proto(const _tchar* pComponentTag);

private:
	CComponent*		Find_Proto(const _tchar* pComponentTag);


private:
	map<const _tchar*, CComponent*>			m_mapProtoType;

private:
	virtual void	Free();
};

END