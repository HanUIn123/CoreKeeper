inline HRESULT Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagement)
{
	CManagement* pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

	*ppManagement = pManagement;

	return S_OK;
}

inline CComponent* Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	return CManagement::GetInstance()->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}

inline CGameObject* Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	return CManagement::GetInstance()->Get_GameObject(pLayerTag, pObjTag);
}

inline CScene* Get_Scene()
{
	return CManagement::GetInstance()->Get_Scene();
}

inline HRESULT Set_Scene(CScene* pScene)
{
	return CManagement::GetInstance()->Set_Scene(pScene);
}

inline _int Update_Scene(const _float& fTimeDelta)
{
	return CManagement::GetInstance()->Update_Scene(fTimeDelta);
}

inline void LateUpdate_Scene()
{
	CManagement::GetInstance()->LateUpdate_Scene();
}

inline void Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CManagement::GetInstance()->Render_Scene(pGraphicDev);
}

inline HRESULT			Ready_Proto(const _tchar* pComponentTag, CComponent* pComponent)
{
	return CComponentMgr::GetInstance()->Ready_Proto(pComponentTag, pComponent);
}
inline CComponent* Clone_Proto(const _tchar* pComponentTag)
{
	return CComponentMgr::GetInstance()->Clone_Proto(pComponentTag);
}

inline void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	CRenderer::GetInstance()->Add_RenderGroup(eType, pGameObject);
}

inline void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
}
inline void			Clear_RenderGroup()
{
	CRenderer::GetInstance()->Clear_RenderGroup();
}

inline void         Delete_Renderer(RENDERID _eType, CGameObject* pGameObject)
{
	CRenderer::GetInstance()->Delete_Renderer(_eType, pGameObject);
}

inline HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
	const D3DLIGHT9* pLightInfo,
	const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}

void		Release_Utility()
{
	CLightMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CComponentMgr::DestroyInstance();
	CManagement::DestroyInstance();
}
