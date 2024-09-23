#include "pch.h"
#include "..\Header\MainApp.h"
#include"../Client/Header/ImguiMgr.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	srand(unsigned(time(NULL)));
	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	Engine::Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	CImguiMgr::GetInstance()->ImGui_Tick();

	return 0;
}

void CMainApp::LateUpdate_MainApp()
{
	m_pManagementClass->LateUpdate_Scene();
}

void CMainApp::Render_MainApp()
{
	Engine::Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	CImguiMgr::GetInstance()->ImGui_Render();

	Engine::Render_End();
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement)
{
	Engine::CScene* pScene = nullptr;

	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);

	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagement), E_FAIL);
	(*ppManagement)->AddRef();

	// Engine::Set_Scene(pScene) // 이렇게 해도 됨
	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(pScene), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass), E_FAIL);
	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	(*ppGraphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);		// Z버퍼에 깊이 값을 기록은 하되 정렬을 수행할지 말지 여부를 묻는 옵션
	(*ppGraphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	// Z버퍼에 깊이 값을 기록할 지 말 지 결정	

	// 폰트 추가

	FAILED_CHECK_RETURN(Engine::Ready_Font(*ppGraphicDev, L"Font_Default", L"Unifont", 40, 40, FW_NORMAL), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Font(*ppGraphicDev, L"Font_HP", L"Unifont", 20, 20, FW_NORMAL), E_FAIL); //HP출력용
	FAILED_CHECK_RETURN(Engine::Ready_Font(*ppGraphicDev, L"Font_Inv", L"Unifont", 22, 22, FW_ULTRABOLD), E_FAIL);//Item용
	FAILED_CHECK_RETURN(Engine::Ready_Font(*ppGraphicDev, L"Font_Item", L"Unifont", 18, 18, FW_ULTRABOLD), E_FAIL);//Item용
	FAILED_CHECK_RETURN(Engine::Ready_Font(*ppGraphicDev, L"Font_Status", L"Unifont", 12, 12, FW_ULTRABOLD), E_FAIL);//Item용

	// dinput 추가
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// sound 추가
	FAILED_CHECK_RETURN(Engine::Ready_Sound(), FALSE);

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		Safe_Release(pMainApp);
		return nullptr;
	}

	return pMainApp;
}

void CMainApp::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);
	Safe_Release(m_pManagementClass);

	Engine::Release_Utility();
	Engine::Release_System();
}
