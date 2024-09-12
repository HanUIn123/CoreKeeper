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
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, _int iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	void            Calculate_Pos(_float _fCurLength, int _iCurHp, int _iMaxHp);

private:
	//_int m_iCurHp, m_iMaxHp;
	_bool m_bActive;

	//_float m_fLength;
	_vec2 m_vPos;

	_int m_iIndex;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CHpDivider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _int iIndex);

private:
	virtual void		Free();
};
