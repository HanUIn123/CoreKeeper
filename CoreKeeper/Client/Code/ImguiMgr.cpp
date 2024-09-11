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
    // create context 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking


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
    // imgui ::render ~~ 
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    // (Your code calls swapchain's Present() function)
    ImGui::EndFrame();

}

void CImguiMgr::ImGui_ShutDown()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

}
