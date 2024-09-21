#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;

END

class CBuilding : public Engine::CGameObject
{
protected:
	explicit						CBuilding(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CBuilding();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT							Add_Component();
protected:
	HRESULT							Setup_Material();

protected:
	Engine::CObjectTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CCalculator* m_pCalculCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void					Free();



protected:


};

