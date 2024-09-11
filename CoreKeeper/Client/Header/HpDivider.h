#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;

END

class CHpDivider : public Engine::CGameObject
{
private:
	explicit CHpDivider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHpDivider();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, _float _fLength);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	_int m_iCurHp, m_iMaxHp;
	_bool m_bFirst;

	_float m_fLength;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CHpDivider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _float _fLength);

private:
	virtual void		Free();
};
