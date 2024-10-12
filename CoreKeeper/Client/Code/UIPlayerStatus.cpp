#include "pch.h"
#include "..\Header\UIPlayerStatus.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

CUIPlayerStatus::CUIPlayerStatus(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bFirst(true)

{
}

CUIPlayerStatus::~CUIPlayerStatus()
{
}

HRESULT CUIPlayerStatus::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y , 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);

	m_bRect.left = vPos.x - vSize.x;
	m_bRect.right = vPos.x + vSize.x;
	m_bRect.top = vPos.y - vSize.y;
	m_bRect.bottom = vPos.y + vSize.y;

	return S_OK;
}

_int CUIPlayerStatus::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bFirst)
	{
		m_pEye = dynamic_cast<CEye*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Eye"));

		m_pShirt = dynamic_cast<CShirt*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Shirt"));

		m_pPants = dynamic_cast<CPants*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Pants"));

		m_pHair = dynamic_cast<CHair*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Hair"));

		m_pEquipInv = dynamic_cast<Engine::CInventory*>
			(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_EquipInventory"));

		m_bFirst = false;
	}

	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 100);

	m_pAnimatorCom->Update_Animation();

	return iExit;
}

void CUIPlayerStatus::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIPlayerStatus::Render_GameObject()
{	
	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	matWorld._11 = 45.f;
	matWorld._22 = 60.f;
	matWorld._41 -= 10.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pSkinTextureCom->Set_Texture();

	m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pAnimBufferCom->Render_Buffer();

	m_pHair->Get_HairTexture()->Set_Texture(m_pHair->Get_TextureNumber());

	m_pHair->Get_HairBuffer()->Render_First();

	m_pEye->Get_EyeTexture()->Set_Texture(m_pEye->Get_TextureNumber());

	m_pEye->Get_EyeBuffer()->Render_First();

	m_pShirt->Get_ShirtTexture()->Set_Texture(m_pShirt->Get_TextureNumber());

	m_pShirt->Get_ShirtBuffer()->Render_First();

	m_pPants->Get_PantsTexture()->Set_Texture(m_pPants->Get_TextureNumber());

	m_pPants->Get_PantsBuffer()->Render_First();
}

HRESULT CUIPlayerStatus::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIStatusBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pSkinTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SkinTexture", pComponent });

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_AnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_AnimBuffer", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CUIPlayerStatus* CUIPlayerStatus::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIPlayerStatus* pUIPlayerStatus = new CUIPlayerStatus(pGraphicDev);

	if (FAILED(pUIPlayerStatus->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIPlayerStatus);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIPlayerStatus;
}

void CUIPlayerStatus::Free()
{
	Engine::CGameObject::Free();
}
