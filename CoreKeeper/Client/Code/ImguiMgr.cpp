#include "pch.h"
#include "../Header/ImguiMgr.h"
#include "Export_System.h"

IMPLEMENT_SINGLETON(CImguiMgr)

CImguiMgr::CImguiMgr()
{
}

CImguiMgr::~CImguiMgr()
{
    ImGui_ShutDown();
}

void CImguiMgr::ImGui_SetUp()
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX9_Init(CGraphicDev::GetInstance()->Get_GraphicDev());

}

void CImguiMgr::ImGui_Tick()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


}

void CImguiMgr::ImGui_Render()
{

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();

}

void CImguiMgr::ImGui_ShutDown()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

}
