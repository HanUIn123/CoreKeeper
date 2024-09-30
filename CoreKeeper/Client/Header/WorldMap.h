//#pragma once
//#include "GameObject.h"
//#include "Define.h"
//
//BEGIN(Engine)
//
//class CTransform;
//class CTexture;
//class CRcTex;
//class CTerrainTex;
//
//END
//
//
//class CWorldMap : public Engine::CGameObject
//{
//private:
//	explicit CWorldMap(LPDIRECT3DDEVICE9 pGraphicDev);
//	virtual ~CWorldMap();
//
//
//
//public:
//	virtual			HRESULT			Ready_GameObject();
//	virtual			_int			Update_GameObject(const _float& fTimeDelta);
//	virtual			void			LateUpdate_GameObject();
//	virtual			void			Render_GameObject();
//private:
//	HRESULT							Add_Component();
//
//private:
//	Engine::CRcTex* m_pBufferCom;
//	Engine::CTransform* m_pTransformCom;
//	Engine::CTexture* m_pTextureCom;
//
//	Engine::CTerrainTex* m_pTerrainTexCom;
//	Engine::CTexture* m_pTerrainTextureCom;
//
//public:
//	static CWorldMap*				Create(LPDIRECT3DDEVICE9 pGraphicDev);
//
//private:
//	_bool							m_bShowMap;
//	_bool							m_bIsKeyPressed;
//private:
//	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
//	_matrix							m_ViewMatrix, m_ProjMatrix;
//
//
//	_D3DVIEWPORT9					m_WorldMapViewPort;
//	_D3DVIEWPORT9					m_WorldMapMainViewPort;
//
//private:
//	virtual void		Free();
//
//};
//
