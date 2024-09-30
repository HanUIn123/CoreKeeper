#pragma once
#include "GameObject.h"
#include "Define.h"
#include "..\Header\Item.h"
#include "..\Header\UICraft.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;
class CAnimTex;
class CAnimator;

END

class CUICraftSlot : public Engine::CGameObject
{
public:
	enum UIPLAYERCRAFT { UCITEM_TORCH, UCITEM_WOODENPICK, UCITEM_WORKBENCH, UCITEM_CHEST, UCITEM_END };

	struct UIITEM
	{
		_int      iItemIndex;

		ITEMNUM   eItemNum;

		MATERIAL  eItemMat = MATERIAL_WOOD;

		_int      iTextureNum;
	};

private:
	explicit CUICraftSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUICraftSlot();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, _int iIndex, _bool bDirection);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	void            Set_Window(TABLETYPE _eTableType = TABLE_PLAYER, _bool _bDirection = true);

	void            Ready_Table();

private:
	HRESULT			Add_Component();

private:
	_int m_iIndex;

	_vec2 m_vPos;

	RECT m_BRect;

	_bool m_bWindow;
	_bool m_bCollapse;

	_bool m_bEnough;

	UIPLAYERCRAFT m_eSlotType;

	UIITEM      m_eItemType;

	TABLETYPE   m_eTableType;

	_bool       m_bDirection;

	map<pair<TABLETYPE, _bool>, UIITEM> mapItemType;

private:
	Engine::CAnimTex* m_pSlotBufferCom;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pItemTextureCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CTexture* m_pSlotTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CUICraftSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _int iIndex, _bool bDirection);

private:
	virtual void		Free();
};
