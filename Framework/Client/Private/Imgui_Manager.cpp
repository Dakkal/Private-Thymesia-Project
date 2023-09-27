#include "pch.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Edit_Terrain.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::Ready_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

    ImGuiStyles();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    for (size_t i = 0; i < (_uint)LEVEL::_END; i++)
    {
        m_bIsWireFrame[i] = true;
    }

	return S_OK;
}

HRESULT CImgui_Manager::Tick(_float fTimeDelta)
{
    m_bFrameReady = true;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Tool Box");

    Menu();
    ToolBox();

    ImGui::End();

    // 마우스 좌표 출력
    ImGui::Begin("Mouse Pos");

    ImVec2 mousePos = ImGui::GetMousePos();

    ImGui::Text("Mouse X : %.f, Mouse Y : %.f", mousePos.x, mousePos.y);

    if (true == m_bIsCreateTerrain[m_iCurLevel])
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

        CEdit_Terrain* pInstance = static_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), 1));

        _vector vTerrainPos = pInstance->Picking_Terrain();

        ImGui::Spacing();
        ImGui::Text("Terrain X : %.f", vTerrainPos.x);
        ImGui::Text("Terrain Y : %.f", vTerrainPos.y);
        ImGui::Text("Terrain Z : %.f", vTerrainPos.z);

        RELEASE_INSTANCE(CGameInstance);
    }
   

    ImGui::End();

	return S_OK;
}

HRESULT CImgui_Manager::LateTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CImgui_Manager::Render()
{
    if (m_bFrameReady)
    {
        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
	return S_OK;
}

void CImgui_Manager::Menu()
{
    /* 메뉴바 */
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Save"))
    {
        // "Save" 메뉴에 하위 메뉴 아이템 추가
        if (ImGui::MenuItem("Level 1"))
        {
            // "Level1" 메뉴 아이템이 클릭될 때 수행할 작업
        }

        if (ImGui::MenuItem("Level 2"))
        {
            // "Level2" 메뉴 아이템이 클릭될 때 수행할 작업
        }
        if (ImGui::MenuItem("Level 3"))
        {
            // "Level3" 메뉴 아이템이 클릭될 때 수행할 작업
        }

        if (ImGui::MenuItem("Level 4"))
        {
            // "Level4" 메뉴 아이템이 클릭될 때 수행할 작업
        }


        ImGui::EndMenu(); // "File" 메뉴 종료
    }
    if (ImGui::BeginMenu("Load"))
    {
        // "Load" 메뉴에 하위 메뉴 아이템 추가
        if (ImGui::MenuItem("Level 1"))
        {
            // "Level1" 메뉴 아이템이 클릭될 때 수행할 작업
        }

        if (ImGui::MenuItem("Level 2"))
        {
            // "Level2" 메뉴 아이템이 클릭될 때 수행할 작업
        }
        if (ImGui::MenuItem("Level 3"))
        {
            // "Level3" 메뉴 아이템이 클릭될 때 수행할 작업
        }

        if (ImGui::MenuItem("Level 4"))
        {
            // "Level4" 메뉴 아이템이 클릭될 때 수행할 작업
        }

        ImGui::EndMenu(); // "File" 메뉴 종료
    }
    ImGui::EndMainMenuBar();
}

void CImgui_Manager::ToolBox()
{
    const char* Level[] = { "Level 1", "Level 2", "Level 3", "Level 4" }; // 콤보 박스의 옵션 목록
    if (ImGui::Combo("Select Level", &m_SelectLevel, Level, IM_ARRAYSIZE(Level)))
    {
        if (true == m_bIsCreateTerrain[m_iCurLevel] && m_iCurLevel != m_SelectLevel)
        {
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
            if (FAILED(pGameInstance->Delete_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), 1)))
            {
                RELEASE_INSTANCE(CGameInstance);
                return;
            }

            RELEASE_INSTANCE(CGameInstance);

            m_bIsCreateTerrain[m_iCurLevel] = false;
        }
        m_iCurLevel = m_SelectLevel;
    }

    ImGui::BeginTabBar("Level");
    if (ImGui::BeginTabItem((m_strCurLevel + to_string(m_iCurLevel + 1)).c_str()))
    {
        // 레벨 탭 내용을 이곳에 추가
        Setting_Terrain();

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Object"))
        {
            // 레벨 오브젝트 설정
            ImGui::BeginTabBar("ObjectTabs");

            if (ImGui::BeginTabItem("Monster"))
            {
                // 레벨 몬스터 설정
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Prop"))
            {
                // 레벨 프롭 설정
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}

void CImgui_Manager::Setting_Terrain()
{
    if (ImGui::CollapsingHeader("Terrain"))
    {
        // 레벨 터레인 설정
        ImGui::SeparatorText("Vertical");
        ImGui::SetNextItemWidth(50);
        ImGui::InputInt("##Vertical Input", &m_iNumVerticesX[m_iCurLevel], 0, 500);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::SliderInt("##Vertical Slider", &m_iNumVerticesX[m_iCurLevel], 0, 500);

        ImGui::SeparatorText("Horizon");
        ImGui::SetNextItemWidth(50);
        ImGui::InputInt("##Horizon Input", &m_iNumVerticesZ[m_iCurLevel], 0, 500);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::SliderInt("##Horizon Slider", &m_iNumVerticesZ[m_iCurLevel], 0, 500);

        ImGui::Spacing();

        if (ImGui::Button("Create"))
        {
            if (false == m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bIsCreateTerrain[m_iCurLevel] = true;

                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
                ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

                TerrainDesc.iNumVerticesX = m_iNumVerticesX[m_iCurLevel];
                TerrainDesc.iNumVerticesZ = m_iNumVerticesZ[m_iCurLevel];
                TerrainDesc.bIsWireFrame = m_bIsWireFrame[m_iCurLevel];

                if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
                {
                    RELEASE_INSTANCE(CGameInstance);
                    return;
                }
                RELEASE_INSTANCE(CGameInstance);
            }

        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            if (true == m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bIsCreateTerrain[m_iCurLevel] = false;

                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                if (FAILED(pGameInstance->Delete_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), 1)))
                {
                    RELEASE_INSTANCE(CGameInstance);
                    return;
                }

                RELEASE_INSTANCE(CGameInstance);
            }
        }
        ImGui::SameLine();
        if (true == m_bIsWireFrame[m_iCurLevel])
        {
            m_bWireCheckBox = true;
        }
        else
        {
            m_bWireCheckBox = false;
        }
        ImGui::Checkbox("WireFrame", &m_bWireCheckBox);
        if (true == m_bWireCheckBox)
        {
            m_bIsWireFrame[m_iCurLevel] = true;

            if (true == m_bIsCreateTerrain[m_iCurLevel])
            {
                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                CEdit_Terrain* pInstance = static_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), 1));

                pInstance->Set_WireFrameMode(m_bIsWireFrame[m_iCurLevel]);

                RELEASE_INSTANCE(CGameInstance);
            }
        }
        else if (false == m_bWireCheckBox)
        {
            m_bIsWireFrame[m_iCurLevel] = false;

            if (true == m_bIsCreateTerrain[m_iCurLevel])
            {
                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                CEdit_Terrain* pInstance = static_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), 1));

               pInstance->Set_WireFrameMode(m_bIsWireFrame[m_iCurLevel]);

                RELEASE_INSTANCE(CGameInstance);
            }
        }
    }
}

void CImgui_Manager::ImGuiStyles()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameBorderSize = 0.f;
    style.ChildBorderSize = 2.f;
    style.TabBorderSize = 0.f;
    style.PopupBorderSize = 0.f;

    style.WindowPadding = ImVec2(5, 5);
    style.WindowRounding = 2.f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 2.f;
    style.ItemSpacing = ImVec2(5, 5);
    style.ItemInnerSpacing = ImVec2(5, 5);

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.1f, 0.5f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.1f, 0.3f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.15f, 0.2f, 1.f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.225f, 0.4f, 1.f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.05f, 0.075f, 0.1f, 1.f);

    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.f, 0.f, 0.f, 1.f);

    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.f, 0.f, 0.1f, 0.2f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5f, 0.6f, 0.9f, 1.f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.f, 1.f / 255.f * 150.f, 1.f, 1.f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.f, 1.f / 255.f * 130.f, 1.f, 1.f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.f, 1.f / 255.f * 130.f, 1.f, 0.8f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.225f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.05f, 0.075f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(.8f, .8f, .8f, 1.f);

}

_bool CImgui_Manager::Is_MouseClickedGUI()
{
    ImGuiContext* Context = ImGui::GetCurrentContext();
    if (!Context)
        return false;

    for (int i = 0; i < Context->Windows.Size; i++)
    {
        ImGuiWindow* window = Context->Windows[i];
        if (window->Flags & ImGuiWindowFlags_NoMouseInputs)
            continue;

        if (window->Rect().Contains(Context->IO.MousePos))
            return true;
    }

    return false;
}

HRESULT CImgui_Manager::Save_MakeShift_Data(LEVEL eLevel)
{
    return S_OK;
}

HRESULT CImgui_Manager::Load_MakeShift_Data(LEVEL eLevel)
{
    return S_OK;
}

HRESULT CImgui_Manager::Save_Data(LEVEL eLevel)
{
    return S_OK;
}

HRESULT CImgui_Manager::Load_Data(LEVEL eLevel)
{
    return S_OK;
}

void CImgui_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
