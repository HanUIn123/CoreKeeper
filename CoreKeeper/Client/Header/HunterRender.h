#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;

END

class CHunterRender : public CGameObject
{
	explicit CHunterRender(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHunterRender();

public:
	virtual			HRESULT			Ready_GameObject(_int iIndex, const wstring _pickedSFName);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_MonsterType() { return MON_HUNTER; }

	virtual wstring					Get_PickedMonsterName() { return m_strPickedMonsterName; }
	virtual void					Set_PickedMonsterName(wstring _pickObjectName) { m_strPickedMonsterName = _pickObjectName; }

private:
	HRESULT			Add_Component();

private:
	Engine::CAnimTex*			m_pBufferCom;
	Engine::CTransform*			m_pTransformCom;
	Engine::CTexture*			m_pTextureCom;


	wstring	m_strPickedMonsterName;

public:
	static	CHunterRender* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex, const wstring _pickedSFName);

private:
	virtual void	Free();
};

