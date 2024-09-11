#pragma once
#include "Engine_Define.h"
#include "Define.h"
#include "GraphicDev.h"

class CImguiMgr
{
    DECLARE_SINGLETON(CImguiMgr)

public:
    CImguiMgr();
    ~CImguiMgr();

public:
    void ImGui_SetUp();
    void ImGui_Tick();
    void ImGui_Render();
    void ImGui_ShutDown();


};

