#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;

END


class CMiniMapFrame : public Engine::CGameObject
{
private:
	explicit CMiniMapFrame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniMapFrame();



public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CMiniMapFrame* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	_bool			m_bIsMinimapExpanded;

private:
	virtual void		Free();
};

