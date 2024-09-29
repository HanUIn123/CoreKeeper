#include "pch.h"
#include "../Header/Object.h"
#include "Export_Utility.h"
#include "Export_System.h"

CObject::CObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_iBuildingImgNum(0)
{
}

CObject::~CObject()
{
}

HRESULT CObject::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CObject::Update_GameObject(const _float& fTimeDelta)
{
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CObject::Render_GameObject()
{

}

HRESULT CObject::Add_Component()
{
	return S_OK;
}

bool CObject::Check_Interaction()
{
	Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));

	// 플레이어와 충돌
	if (m_pColliderCom->Check_Sphere_Collision(pPlayerCollider))
	{
		return true;
	}

	return false;
}

void CObject::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		// UI 열리게 하는 걸 여기에 하면 됨
	}
}

HRESULT CObject::Setup_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.7f, 0.7f, 0.7f, 0.7f };

	tMtrl.Emissive = { 0.2f, 0.2f, 0.2f, 0.2f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

CObject* CObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CObject* pObject = new CObject(pGraphicDev);

	if (FAILED(pObject->Ready_GameObject()))
	{
		Safe_Release(pObject);
		MSG_BOX("pBuilding Create Failed");
		return nullptr;
	}

	return pObject;
}

void CObject::Free()
{
	Engine::CGameObject::Free();
}
