#pragma once

#include "Management.h"
#include "ComponentMgr.h"
#include "Renderer.h"
#include "LightMgr.h"

#include "TriCol.h"
#include "RcCol.h"
#include "RcTex.h"
#include "TerrainTex.h"
#include "CubeTex.h"
#include "AnimTex.h"
#include "ShadowTex.h"
#include "MapToolTex.h"
#include "TileTex.h"
#include "WallTex.h"
#include "ObjectTex.h"

//#include "MinimapTex.h"

#include "Firework.h"
#include "Fall.h"
#include "Followers.h"
#include "Smoke.h"
#include "Hit.h"
#include "Smog.h"
#include "Fume.h"
#include "Heal.h"
#include "Water.h"

#include "Texture.h"
#include "ShadowTex.h"
#include "RangeTex.h"

#include "Transform.h"
#include "Camera.h"
#include "Calculator.h"
#include "Animator.h"
#include "Collider.h"
#include "ColliderCube.h"
#include "State.h"
#include "Inventory.h"
#include "Shader.h"

BEGIN(Engine)

inline HRESULT				Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagement);

inline CComponent*          Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);
inline CGameObject*         Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
inline CScene*              Get_Scene();
inline	HRESULT				Set_Scene(CScene* pScene);
inline	_int				Update_Scene(const _float& fTimeDelta);
inline	void				LateUpdate_Scene();
inline	void				Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

// componentMgr
inline HRESULT			Ready_Proto(const _tchar* pComponentTag, CComponent* pComponent);
inline CComponent* Clone_Proto(const _tchar* pComponentTag);

//renderer

inline void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
inline void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void			Clear_RenderGroup();
inline void         Delete_Renderer(RENDERID _eType, CGameObject* pGameObject);
inline void			Expand_MiniMap(LPDIRECT3DDEVICE9& pGraphicDev);
inline _float		Get_ZoomRatio();

inline _bool		Get_ExpandMap();
inline void			Set_ExpandMap(_bool _bExpandMap);

inline _bool		Get_CloseMap();
inline void			Set_CloseMap(_bool _bCloseMap);
inline void			OpenClose_MiniMap();
inline void			Set_Start(bool _bStart);
// light

inline HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
	const D3DLIGHT9* pLightInfo,
	const _uint& iIndex);

inline void		Release_Utility();

#include "Export_Utility.inl"

END
