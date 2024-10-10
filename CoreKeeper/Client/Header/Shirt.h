#pragma once
#include "Item.h"
#include "Define.h"
#include "Export_System.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;

END

class CShirt : public CItem
{
private:
	explicit CShirt(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShirt();
public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	CTexture* Get_ShirtTexture() { return m_pTextureCom; }
	CAnimTex* Get_ShirtBuffer() { return m_pBufferCom; }
private:
	HRESULT			Add_Component();
	HRESULT			Setup_Material();
	void			Follow_Player();

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CShirt* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

