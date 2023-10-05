#include "pch.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "imgui_spectrum.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Edit_Terrain.h"
#include "Input_Device.h"
#include "Layer.h"

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

    //ImGuiStyles();
    ImGui::Spectrum::StyleColorsSpectrum();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    auto mapObjectProto = pGameInstance->Get_Prototypes();

    string  strStoreName;
    for (auto iter : *mapObjectProto)
    {
        if (OBJECT_TYPE::MONSTER == iter.second->Get_ObjectType())
        {
            strStoreName.assign(iter.first.begin(), iter.first.end());
            m_vecMonsters.push_back(strStoreName);
        }
        if (OBJECT_TYPE::PROP == iter.second->Get_ObjectType())
        {
            strStoreName.assign(iter.first.begin(), iter.first.end());
            m_vecProps.push_back(strStoreName);
        }
    }


    RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CImgui_Manager::Tick(_float fTimeDelta)
{
    m_bFrameReady = true;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Tool Box");

    Menu(fTimeDelta);
    ToolBox(fTimeDelta);

    ImGui::End();

    // 마우스 좌표 출력
    Mouse_Pos();
    List_Object();

	return S_OK;
}

HRESULT CImgui_Manager::LateTick(_float fTimeDelta)
{
    if (nullptr != m_pSelectObject)
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
        if(nullptr == pTransform)
            return E_FAIL;

        switch (m_pSelectObject->Get_ObjectType())
        {
        case OBJECT_TYPE::MONSTER:
            if (m_IsMonTransformOpen[m_iCurLevel])
            {
                pTransform->Set_Scale(m_vMonsterScale[m_iCurLevel]);
                pTransform->Set_State(CTransform::STATE_POS, m_vMonsterPos[m_iCurLevel]);
            }
            break;
        case OBJECT_TYPE::PROP:
            if (m_IsPropTransformOpen[m_iCurLevel])
            {
                pTransform->Set_Scale(m_vPropScale[m_iCurLevel]);
                pTransform->Set_State(CTransform::STATE_POS, m_vPropPos[m_iCurLevel]);
            }
            break;
        default:
            break;
        }
    }
   

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

HRESULT CImgui_Manager::Menu(_float fTimeDelta)
{
    /* 메뉴바 */
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Save"))
    {
        // "Save" 메뉴에 하위 메뉴 아이템 추가
        if (ImGui::MenuItem("Level 1"))
        {
            // "Level1" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel &&
                m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }  
        }

        if (ImGui::MenuItem("Level 2"))
        {
            // "Level2" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel &&
                m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }
        }
        if (ImGui::MenuItem("Level 3"))
        {
            // "Level3" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel &&
                m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }
        }

        if (ImGui::MenuItem("Level 4"))
        {
            // "Level4" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel &&
                m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }
        }


        ImGui::EndMenu(); // "File" 메뉴 종료
    }
    if (ImGui::BeginMenu("Load"))
    {
        // "Load" 메뉴에 하위 메뉴 아이템 추가
        if (ImGui::MenuItem("Level 1"))
        {
            // "Level1" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::LOAD)] = true;
                Load_Data(LEVEL(m_SelectLevel));
            }
        }

        if (ImGui::MenuItem("Level 2"))
        {
            // "Level2" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::LOAD)] = true;
                Load_Data(LEVEL(m_SelectLevel));
            }
        }
        if (ImGui::MenuItem("Level 3"))
        {
            // "Level3" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::LOAD)] = true;
                Load_Data(LEVEL(m_SelectLevel));
            }
        }

        if (ImGui::MenuItem("Level 4"))
        {
            // "Level4" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::LOAD)] = true;
                Load_Data(LEVEL(m_SelectLevel));
            }
        }

        ImGui::EndMenu(); // "File" 메뉴 종료
    }
    ImGui::EndMainMenuBar();

    if (m_bShowMessage[(_uint)DATA::SAVE])
    {
        ImGui::OpenPopup("Save Message");
    }
    if (m_bShowMessage[(_uint)DATA::LOAD])
    {
        ImGui::OpenPopup("Load Message");
    }
    if (ImGui::BeginPopupModal("Save Message", &m_bShowMessage[(_uint)DATA::SAVE], ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Save Complete");
        ImGui::Separator();

        ImGui::BeginGroup();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::GetTextLineHeightWithSpacing()) / 2.0f);

        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
            m_bShowMessage[(_uint)DATA::SAVE] = false;
        }
        ImGui::EndGroup();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Load Message", &m_bShowMessage[(_uint)DATA::LOAD], ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Load Complete");
        ImGui::Separator();

        ImGui::BeginGroup();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::GetTextLineHeightWithSpacing()) / 2.0f);

        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
            m_bShowMessage[(_uint)DATA::LOAD] = false;
        }
        ImGui::EndGroup();
        ImGui::EndPopup();
    }

    return S_OK;
}

HRESULT CImgui_Manager::ToolBox(_float fTimeDelta)
{
    const char* Level[] = { "Level 1", "Level 2", "Level 3", "Level 4" }; // 콤보 박스의 옵션 목록
    if (ImGui::Combo("Select Level", &m_SelectLevel, Level, IM_ARRAYSIZE(Level)))
    {
        if (true == m_bIsCreateTerrain[m_iCurLevel] && m_iCurLevel != m_SelectLevel)
        {
            Save_MakeShift_Data(LEVEL(m_iCurLevel));

            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

            pGameInstance->Delete_Layer(LEVEL_EDIT, TEXT("Layer_Terrain"));
            pGameInstance->Delete_Layer(LEVEL_EDIT, TEXT("Layer_Object"));

            if (nullptr != m_pSelectObject)
                m_pSelectObject = nullptr;

            RELEASE_INSTANCE(CGameInstance);
        }
        m_iCurLevel = m_SelectLevel;
        if (true == m_bIsCreateTerrain[m_iCurLevel])
        {
            Load_MakeShift_Data(LEVEL(m_iCurLevel));
        }
    }

    ImGui::BeginTabBar("Level");
    if (ImGui::BeginTabItem((m_strCurLevel + to_string(m_iCurLevel + 1)).c_str()))
    {
        // 레벨 탭 내용을 이곳에 추가
        Setting_Terrain();

        ImGui::Spacing();

        Setting_Object();
    }
    ImGui::EndTabBar();

    return S_OK;
}

HRESULT CImgui_Manager::Setting_Terrain()
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
            if (0 != m_iNumVerticesX[m_iCurLevel] && 0 != m_iNumVerticesZ[m_iCurLevel])
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
                        return E_FAIL;
                    }
                    
                    m_pSelectTerrain = pGameInstance->Last_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"));
                    if (nullptr == m_pSelectTerrain)
                    {
                        RELEASE_INSTANCE(CGameInstance);
                        return E_FAIL;
                    }

                    RELEASE_INSTANCE(CGameInstance);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            if (true == m_bIsCreateTerrain[m_iCurLevel])
            {
                m_bIsCreateTerrain[m_iCurLevel] = false;

                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                if (FAILED(pGameInstance->Delete_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index())))
                {
                    RELEASE_INSTANCE(CGameInstance);
                    return E_FAIL;
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

                CEdit_Terrain* pInstance = dynamic_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index()));

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

                CEdit_Terrain* pInstance = dynamic_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index()));

               pInstance->Set_WireFrameMode(m_bIsWireFrame[m_iCurLevel]);

                RELEASE_INSTANCE(CGameInstance);
            }
        }
    }

    return S_OK;
}

HRESULT CImgui_Manager::Setting_Object()
{
    if (ImGui::CollapsingHeader("Object"))
    {
        // 레벨 오브젝트 설정
        ImGui::BeginTabBar("ObjectTabs");

        if (ImGui::BeginTabItem("Monster"))
        {
            // 레벨 몬스터 설정
            if (ImGui::BeginListBox("##MonsterList", ImVec2(ImGui::GetContentRegionAvail().x, 100)))
            {
                for (int i = 0; i < m_vecMonsters.size(); i++)
                {
                    bool isSelected = (i == m_iSelectMonster[m_iCurLevel]);

                    if (ImGui::Selectable(m_vecMonsters[i].c_str(), isSelected))
                    {
                        m_iSelectMonster[m_iCurLevel] = i;

                        if (m_strCurMonsterProtoObject[m_iCurLevel] != wstring().assign(m_vecMonsters[i].begin(), m_vecMonsters[i].end()))
                        {
                            m_vMonsterScale[m_iCurLevel] = XMVectorZero();
                            m_vMonsterRot[m_iCurLevel] = XMVectorZero();
                            m_vMonsterPos[m_iCurLevel] = XMVectorZero();
                        }
                        m_strCurMonsterProtoObject[m_iCurLevel].assign(m_vecMonsters[i].begin(), m_vecMonsters[i].end());
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();

                if (ImGui::Button("Clone"))
                {

                }
                ImGui::SameLine();
                if (ImGui::Button("Delete All"))
                {

                }
            }

            if (ImGui::CollapsingHeader("Transformation"))
            {
                m_IsMonTransformOpen[m_iCurLevel] = true;

                /* 스케일 */
                ImGui::SeparatorText("Scale");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("SizX");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##MonScaleX Input", &m_vMonsterScale[m_iCurLevel].x, -50, 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##MonScaleX Slider", &m_vMonsterScale[m_iCurLevel].x, -50, 50, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("SizY");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##MonScaleY Input", &m_vMonsterScale[m_iCurLevel].y, -50, 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##MonScaleY Slider", &m_vMonsterScale[m_iCurLevel].y, -50, 50, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("SizZ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##MonScaleZ Input", &m_vMonsterScale[m_iCurLevel].z, -50, 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##MonScaleZ Slider", &m_vMonsterScale[m_iCurLevel].z, -50, 50, "%.1f");

                /* 회전 */
                ImGui::SeparatorText("Rotation");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("RotX");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                if (ImGui::InputFloat("##MonRotX Input", &m_vMonsterRot[m_iCurLevel].x, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_vPropRot[m_iCurLevel].x));
                    }
                }
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::SliderFloat("##MonRotX Slider", &m_vMonsterRot[m_iCurLevel].x, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_vPropRot[m_iCurLevel].x));
                    }
                }
                ImGui::SetNextItemWidth(80);

                ImGui::Text("RotY");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                if (ImGui::InputFloat("##MonRotY Input", &m_vMonsterRot[m_iCurLevel].y, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_vPropRot[m_iCurLevel].y));
                    }
                }
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::SliderFloat("##MonRotY Slider", &m_vMonsterRot[m_iCurLevel].y, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_vPropRot[m_iCurLevel].y));
                    }
                }
                ImGui::SetNextItemWidth(80);

                ImGui::Text("RotZ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                if (ImGui::InputFloat("##MonRotZ Input", &m_vMonsterRot[m_iCurLevel].z, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_vPropRot[m_iCurLevel].z));
                    }
                }
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::SliderFloat("##MonRotZ Slider", &m_vMonsterRot[m_iCurLevel].z, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_vPropRot[m_iCurLevel].z));
                    }
                }
                ImGui::SetNextItemWidth(80);

                /* 위치 */
                ImGui::SeparatorText("Position");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("PosX");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##MonPosX Input", &m_vMonsterPos[m_iCurLevel].x, 0, m_iNumVerticesX[m_iCurLevel], "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##MonPosX Slider", &m_vMonsterPos[m_iCurLevel].x, 0, m_iNumVerticesX[m_iCurLevel], "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("PosY");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##MonPosY Input", &m_vMonsterPos[m_iCurLevel].y, 0, 100, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##MonPosY Slider", &m_vMonsterPos[m_iCurLevel].y, 0, 100, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("PosZ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##MonPosZ Input", &m_vMonsterPos[m_iCurLevel].z, 0, m_iNumVerticesZ[m_iCurLevel], "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##MonPosZ Slider", &m_vMonsterPos[m_iCurLevel].z, 0, m_iNumVerticesZ[m_iCurLevel], "%.1f");

                ImGui::Spacing();
                if (ImGui::Button("Set"))
                {

                }
                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                {

                }
            }
            else
            {
                m_IsMonTransformOpen[m_iCurLevel] = false;
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Prop"))
        {
            // 레벨 프롭 설정
            if (ImGui::BeginListBox("##PropList", ImVec2(ImGui::GetContentRegionAvail().x, 100)))
            {
                for (int i = 0; i < m_vecProps.size(); i++)
                {
                    bool isSelected = (i == m_iSelectProp[m_iCurLevel]);

                    if (ImGui::Selectable(m_vecProps[i].c_str(), isSelected))
                    {
                        m_iSelectProp[m_iCurLevel] = i;

                        if (m_strCurPropProtoObject[m_iCurLevel] != wstring().assign(m_vecProps[i].begin(), m_vecProps[i].end()))
                        {
                            m_vPropScale[m_iCurLevel] = XMVectorZero();
                            m_vPropRot[m_iCurLevel] = XMVectorZero();
                            m_vPropPos[m_iCurLevel] = XMVectorZero();
                        }
                        m_strCurPropProtoObject[m_iCurLevel].assign(m_vecProps[i].begin(), m_vecProps[i].end());
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();

                /* 클론생성 */
                if (ImGui::Button("Clone"))
                {
                    if (-1 != m_vTerrainPos[m_iCurLevel].x)
                    {
                        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                        if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), m_strCurPropProtoObject[m_iCurLevel])))
                            return E_FAIL;

                        m_pSelectObject = pGameInstance->Last_GameObject(LEVEL_EDIT, TEXT("Layer_Object"));
                        if (nullptr == m_pSelectObject)
                            return E_FAIL;

                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));

                        pTransform->Set_State(CTransform::STATE_POS, m_vTerrainPos[m_iCurLevel]);

                        m_matStore[m_iCurLevel] = pTransform->Get_WorldMatrix();
                        m_vPropScale[m_iCurLevel] = pTransform->Get_Scale();
                        m_vPropRot[m_iCurLevel] = XMVectorZero();
                        m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);

                        wstring strObjectName = m_pSelectObject->Get_Name();
                        string strSelectNameIndex;
                        strSelectNameIndex.assign(strObjectName.begin(), strObjectName.end());
                        strSelectNameIndex += to_string(m_pSelectObject->Get_Index());

                        m_vecObjectList.push_back(strSelectNameIndex);
                        m_iSelectObject[m_iCurLevel] = m_vecObjectList.size() - 1;

                        RELEASE_INSTANCE(CGameInstance);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete All"))
                {
                    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                    pGameInstance->Delete_Layer(LEVEL_EDIT, TEXT("Layer_Object"));

                    m_vecObjectList.clear();

                    if (nullptr != m_pSelectObject)
                        m_pSelectObject = nullptr;

                    RELEASE_INSTANCE(CGameInstance);
                }
            }

            if (ImGui::CollapsingHeader("Transformation"))
            {
                m_IsPropTransformOpen[m_iCurLevel] = true;
                /* 스케일 */
                ImGui::SeparatorText("Scale");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("SizX");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##PropScaleX Input", &m_vPropScale[m_iCurLevel].x, -50, 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##PropScaleX Slider", &m_vPropScale[m_iCurLevel].x, -50, 50, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("SizY");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##PropScaleY Input", &m_vPropScale[m_iCurLevel].y, -50, 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##PropScaleY Slider", &m_vPropScale[m_iCurLevel].y, -50, 50, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("SizZ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##PropScaleZ Input", &m_vPropScale[m_iCurLevel].z, -50, 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##PropScaleZ Slider", &m_vPropScale[m_iCurLevel].z, -50, 50, "%.1f");

                /* 회전 */
                ImGui::SeparatorText("Rotation");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("RotX");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                if (ImGui::InputFloat("##PropRotX Input", &m_vPropRot[m_iCurLevel].x, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_vPropRot[m_iCurLevel].x));
                    }
                }
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::SliderFloat("##PropRotX Slider", &m_vPropRot[m_iCurLevel].x, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_vPropRot[m_iCurLevel].x));
                    }
                }
                ImGui::SetNextItemWidth(80);

                ImGui::Text("RotY");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                if (ImGui::InputFloat("##PropRotY Input", &m_vPropRot[m_iCurLevel].y, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_vPropRot[m_iCurLevel].y));
                    }
                }
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::SliderFloat("##PropRotY Slider", &m_vPropRot[m_iCurLevel].y, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_vPropRot[m_iCurLevel].y));
                    }
                }
                ImGui::SetNextItemWidth(80);

                ImGui::Text("RotZ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                if (ImGui::InputFloat("##PropRotZ Input", &m_vPropRot[m_iCurLevel].z, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_vPropRot[m_iCurLevel].z));
                    }
                }
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::SliderFloat("##PropRotZ Slider", &m_vPropRot[m_iCurLevel].z, 0, 360, "%.1f"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_vPropRot[m_iCurLevel].z));
                    }
                }
                ImGui::SetNextItemWidth(80);

                /* 위치 */
                ImGui::SeparatorText("Position");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("PosX");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##PropPosX Input", &m_vPropPos[m_iCurLevel].x
                    , m_matStore[m_iCurLevel]._41 - 50, m_matStore[m_iCurLevel]._41 + 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##PropPosX Slider", &m_vPropPos[m_iCurLevel].x
                    , m_matStore[m_iCurLevel]._41 - 50, m_matStore[m_iCurLevel]._41 + 50, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("PosY");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##PropPosY Input", &m_vPropPos[m_iCurLevel].y, m_matStore[m_iCurLevel]._42 -10, m_matStore[m_iCurLevel]._42 + 10, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##PropPosY Slider", &m_vPropPos[m_iCurLevel].y, m_matStore[m_iCurLevel]._42 -10, m_matStore[m_iCurLevel]._42 + 10, "%.1f");
                ImGui::SetNextItemWidth(80);

                ImGui::Text("PosZ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::InputFloat("##PropPosZ Input", &m_vPropPos[m_iCurLevel].z
                    , m_matStore[m_iCurLevel]._43 - 50, m_matStore[m_iCurLevel]._43 + 50, "%.1f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::SliderFloat("##PropPosZ Slider", &m_vPropPos[m_iCurLevel].z
                    , m_matStore[m_iCurLevel]._43 - 50, m_matStore[m_iCurLevel]._43 + 50, "%.1f");

                ImGui::Spacing();
                if (ImGui::Button("Size Reset"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        if (nullptr == pTransform)
                            return E_FAIL;

                        _vector vRight, vUp, vLook;

                        memmove(&vRight, &m_matStore[m_iCurLevel].m[0], sizeof(_vector));
                        memmove(&vUp, &m_matStore[m_iCurLevel].m[1], sizeof(_vector));
                        memmove(&vLook, &m_matStore[m_iCurLevel].m[2], sizeof(_vector));

                        pTransform->Set_Scale(_vector(vRight.Length(), vUp.Length(), vLook.Length(), 0));
                        m_vPropScale[m_iCurLevel] = _vector(vRight.Length(), vUp.Length(), vLook.Length(), 0);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Rot Reset"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        if (nullptr == pTransform)
                            return E_FAIL;

                        pTransform->Fix_Rotation(pTransform->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
                        m_vPropRot[m_iCurLevel] = XMVectorZero();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Pos Reset"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        if (nullptr == pTransform)
                            return E_FAIL;

                        _vector vPos;

                        memmove(&vPos, &m_matStore[m_iCurLevel].m[3], sizeof(_vector));

                        pTransform->Set_State(CTransform::STATE_POS, vPos);
                        m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);
                    }
                }

                ImGui::Spacing();
                if (ImGui::Button("Set"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        if (nullptr == pTransform)
                            return E_FAIL;

                        m_matStore[m_iCurLevel] = pTransform->Get_WorldMatrix();
                        m_vPropScale[m_iCurLevel] = pTransform->Get_Scale();
                        m_vPropRot[m_iCurLevel] = XMVectorZero();
                        m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("All Reset"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
                        if (nullptr == pTransform)
                            return E_FAIL;

                        pTransform->Set_WorldMatrix(m_matStore[m_iCurLevel]);
                        m_vPropScale[m_iCurLevel] = pTransform->Get_Scale();
                        m_vPropRot[m_iCurLevel] = XMVectorZero();
                        m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("To MousePos"))
                {
                    if (nullptr != m_pSelectObject)
                    {
                        m_vPropPos[m_iCurLevel] = m_vTerrainPos[m_iCurLevel];
                    }
                }
            }
            else
            {
                m_IsPropTransformOpen[m_iCurLevel] = false;
            }
            
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndTabItem();

    return S_OK;
}

HRESULT CImgui_Manager::Mouse_Pos()
{
    ImGui::Begin("Mouse Pos");

    ImVec2 mousePos = ImGui::GetMousePos();

    ImGui::Text("Mouse X : %.f", mousePos.x);
    ImGui::Text("Mouse Y : %.f", mousePos.y);

    if (true == m_bIsCreateTerrain[m_iCurLevel])
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

        CEdit_Terrain* pInstance = dynamic_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index()));

        _vector vTerrainPos = pInstance->Picking_Terrain();

        ImGui::Spacing();
        ImGui::Text("Terrain X : %.f", vTerrainPos.x);
        ImGui::Text("Terrain Y : %.f", vTerrainPos.y);
        ImGui::Text("Terrain Z : %.f", vTerrainPos.z);
        vTerrainPos.w = 1.f;

        if (false == Is_MouseClickedGUI() && pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON))
        {
            m_vTerrainPos[m_iCurLevel] = vTerrainPos;
        }

        ImGui::Spacing();
        ImGui::Text("SavePos X : %.f", m_vTerrainPos[m_iCurLevel].x);
        ImGui::Text("SavePos Y : %.f", m_vTerrainPos[m_iCurLevel].y);
        ImGui::Text("SavePos Z : %.f", m_vTerrainPos[m_iCurLevel].z);
        vTerrainPos.w = 1.f;

        RELEASE_INSTANCE(CGameInstance);
    }


    ImGui::End();

    return S_OK;;
}

HRESULT CImgui_Manager::List_Object()
{
    ImGui::Begin("Object List");
   
    if (ImGui::BeginListBox("##List", ImVec2(ImGui::GetContentRegionAvail().x, 300)))
    {
        for (int i = 0; i < m_vecObjectList.size(); i++)
        {
            bool isSelected = (i == m_iSelectObject[m_iCurLevel]);

            if (ImGui::Selectable(m_vecObjectList[i].c_str(), isSelected))
            {
                m_iSelectObject[m_iCurLevel] = i;

                ImGui::SetItemDefaultFocus();
            }
        }

        if (nullptr != m_pSelectObject)
        {
            wstring strSelectName = m_pSelectObject->Get_Name();
            string strSelectNameIndex;
            strSelectNameIndex.assign(strSelectName.begin(), strSelectName.end());
            strSelectNameIndex += to_string(m_pSelectObject->Get_Index());

            string inputStr = m_vecObjectList[m_iSelectObject[m_iCurLevel]];
            string alpha;
            string digits;

            for (char c : inputStr) {
                if (std::isalpha(c)) {
                    alpha += c;
                }
                else if (std::isdigit(c)) {
                    digits += c;
                }
            }

            wstring walpha;
            walpha.assign(alpha.begin(), alpha.end());

            if (walpha != m_pSelectObject->Get_Name() || stoi(digits) != m_pSelectObject->Get_Index())
            {
                CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                m_pSelectObject = pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), walpha, stoi(digits));
                if (nullptr == m_pSelectObject)
                {
                    RELEASE_INSTANCE(CGameInstance);
                    ImGui::End();
                    return E_FAIL;
                }

                CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));

                m_matStore[m_iCurLevel] = pTransform->Get_WorldMatrix();
                m_vPropScale[m_iCurLevel] = pTransform->Get_Scale();
                m_vPropRot[m_iCurLevel] = XMVectorZero();
                m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);

                RELEASE_INSTANCE(CGameInstance);
            }
        }
        else if (nullptr == m_pSelectObject && 0 < m_vecObjectList.size())
        {
            string inputStr = m_vecObjectList[0];
            string alpha;
            string digits;

            for (char c : inputStr) {
                if (std::isalpha(c)) {
                    alpha += c;
                }
                else if (std::isdigit(c)) {
                    digits += c;
                }
            }

            wstring walpha;
            walpha.assign(alpha.begin(), alpha.end());

            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

            m_pSelectObject = pGameInstance->Find_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), walpha, stoi(digits));
            if (nullptr == m_pSelectObject)
            {
                RELEASE_INSTANCE(CGameInstance);
                ImGui::End();
                return E_FAIL;
            }

            CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));

            m_matStore[m_iCurLevel] = pTransform->Get_WorldMatrix();
            m_vPropScale[m_iCurLevel] = pTransform->Get_Scale();
            m_vPropRot[m_iCurLevel] = XMVectorZero();
            m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);

            RELEASE_INSTANCE(CGameInstance);
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Delete"))
    {
        if (nullptr != m_pSelectObject)
        {
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

            wstring strObjectName = m_pSelectObject->Get_Name();
            string strSelectNameIndex;
            strSelectNameIndex.assign(strObjectName.begin(), strObjectName.end());
            strSelectNameIndex += to_string(m_pSelectObject->Get_Index());

            for (auto iter = m_vecObjectList.begin(); iter < m_vecObjectList.end(); ++iter)
            {
                if (*iter == strSelectNameIndex)
                {
                    m_vecObjectList.erase(iter);
                    break;
                }
                    
            }

            if (FAILED(pGameInstance->Delete_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), m_pSelectObject->Get_Name(), m_pSelectObject->Get_Index())))
            {
                RELEASE_INSTANCE(CGameInstance);
                return E_FAIL;
            }

            m_pSelectObject = nullptr;
            m_iSelectObject[m_iCurLevel] = 0;

            RELEASE_INSTANCE(CGameInstance);
        }
    }
    

   
    ImGui::End();

    return S_OK;;
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
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.225f, 0.30f, 1.00f);
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
    string strFileTerrain = "../Bin/Data/MakeShift/MakeShiftLv" + to_string((_uint)eLevel + 1) + "Terrain.dat";
    string strFileObject = "../Bin/Data/MakeShift/MakeShiftLv" + to_string((_uint)eLevel + 1) + "Object.dat";

    ofstream OutTerrain(strFileTerrain, ios::binary);
    ofstream OutObject(strFileObject, ios::binary);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    auto listObject = pGameInstance->Get_LayerList(LEVEL_EDIT, TEXT("Layer_Object"));

    RELEASE_INSTANCE(CGameInstance);

    if (OutTerrain.is_open())
    {
        OutTerrain << m_iNumVerticesX[m_iCurLevel] << endl;
        OutTerrain << m_iNumVerticesZ[m_iCurLevel] << endl;
        OutTerrain << m_bIsWireFrame[m_iCurLevel] << endl;
    }

    if (OutObject.is_open())
    {
        for (auto& iter : *listObject)
        {
            _uint iType = _uint(iter->Get_ObjectType());

            wstring wstrTag = iter->Get_ProtoTag();
            string strTag;
            strTag.assign(wstrTag.begin(), wstrTag.end());

            XMFLOAT4X4 matObject = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
            _float  elements[4][4];

            OutObject << iType << endl;
            for (size_t i = 0; i < 4; i++)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    elements[i][j] = matObject.m[i][j];
                    OutObject << elements[i][j] << endl;
                }
            }
            OutObject << strTag.size();
            OutObject.write(strTag.c_str(), strTag.size());
        }
    }

    return S_OK;
}

HRESULT CImgui_Manager::Load_MakeShift_Data(LEVEL eLevel)
{
    string strFileTerrain = "../Bin/Data/MakeShift/MakeShiftLv" + to_string((_uint)eLevel + 1) + "Terrain.dat";
    string strFileObject = "../Bin/Data/MakeShift/MakeShiftLv" + to_string((_uint)eLevel + 1) + "Object.dat";

    ifstream  InTerrain(strFileTerrain, ios::binary);
    ifstream  InObject(strFileObject, ios::binary);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
   
#pragma region Terrain
    if (InTerrain.is_open())
    {
        InTerrain >> m_iNumVerticesX[m_iCurLevel];
        InTerrain >> m_iNumVerticesZ[m_iCurLevel];
        InTerrain >> m_bIsWireFrame[m_iCurLevel];
    }
    CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
    ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

    TerrainDesc.iNumVerticesX = m_iNumVerticesX[m_iCurLevel];
    TerrainDesc.iNumVerticesZ = m_iNumVerticesZ[m_iCurLevel];
    TerrainDesc.bIsWireFrame = m_bIsWireFrame[m_iCurLevel];

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
    m_pSelectTerrain = pGameInstance->Last_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"));
    if (nullptr == m_pSelectTerrain)
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
#pragma endregion

  
#pragma region Object
    if (InObject.is_open())
    {
        _uint iType;
        OBJECT_TYPE eType;

        _int iStrSize;
        string strTag;
        wstring wstrTag;

        XMFLOAT4X4 matObject;
        _float  elements[4][4];

        while (true)
        {
            InObject >> iType;
            for (size_t i = 0; i < 4; i++)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    InObject >> elements[i][j];
                }
            }
            InObject >> iStrSize;
            char* filePath = new char[iStrSize + 1]; 
            InObject.read(filePath, iStrSize);
            filePath[iStrSize] = '\0'; 

            

            eType = OBJECT_TYPE(iType);
            for (size_t i = 0; i < 4; i++)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    matObject.m[i][j] = elements[i][j];
                }
            }
            strTag = filePath;
            wstrTag.assign(strTag.begin(), strTag.end());
            Safe_Delete_Array(filePath);

            if (!InObject)
                break;

            if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), wstrTag)))
                return E_FAIL;

            CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_EDIT, TEXT("Layer_Object"));
            if (nullptr == pObject)
                return E_FAIL;

            CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
            pTransform->Set_WorldMatrix(matObject);

            wstring strObjectName = pObject->Get_Name();
            string strSelectNameIndex;
            strSelectNameIndex.assign(strObjectName.begin(), strObjectName.end());
            strSelectNameIndex += to_string(pObject->Get_Index());

            m_vecObjectList.push_back(strSelectNameIndex);
            m_iSelectObject[m_iCurLevel] = m_vecObjectList.size() - 1;
        }
    }
#pragma endregion
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

void CImgui_Manager::DeleteMakeShift_Data()
{
    for (size_t i = 0; i < _uint(LEVEL::_END); i++)
    {
        string strFileTerrain = "../Bin/Data/MakeShift/MakeShiftLv" + to_string(i + 1) + "Terrain.dat";
        string strFileObject = "../Bin/Data/MakeShift/MakeShiftLv" + to_string(i + 1) + "Object.dat";

        remove(strFileTerrain.c_str());
        remove(strFileObject.c_str());
    }
}

HRESULT CImgui_Manager::Save_Data(LEVEL eLevel)
{
    string strFileTerrain = "../Bin/Data/Level" + to_string((_uint)eLevel + 1) + "Terrain.dat";
    string strFileObject = "../Bin/Data/Level" + to_string((_uint)eLevel + 1) + "Object.dat";

    ofstream OutTerrain(strFileTerrain, ios::binary);
    ofstream OutObject(strFileObject, ios::binary);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    auto listObject = pGameInstance->Get_LayerList(LEVEL_EDIT, TEXT("Layer_Object"));

    RELEASE_INSTANCE(CGameInstance);

    if (OutTerrain.is_open())
    {
        OutTerrain << m_iNumVerticesX[m_iCurLevel] << endl;
        OutTerrain << m_iNumVerticesZ[m_iCurLevel] << endl;
        OutTerrain << m_bIsWireFrame[m_iCurLevel] << endl;
    }
    else
        return E_FAIL;

    if (OutObject.is_open())
    {
        for (auto& iter : *listObject)
        {
            _uint iType = _uint(iter->Get_ObjectType());

            wstring wstrTag = iter->Get_ProtoTag();
            string strTag;
            strTag.assign(wstrTag.begin(), wstrTag.end());

            XMFLOAT4X4 matObject = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
            _float  elements[4][4];

            OutObject << iType << endl;
            for (size_t i = 0; i < 4; i++)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    elements[i][j] = matObject.m[i][j];
                    OutObject << elements[i][j] << endl;
                }
            }
            OutObject << strTag.size();
            OutObject.write(strTag.c_str(), strTag.size());
        }
    }
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CImgui_Manager::Load_Data(LEVEL eLevel)
{
    string strFileTerrain = "../Bin/Data/Level" + to_string((_uint)eLevel + 1) + "Terrain.dat";
    string strFileObject = "../Bin/Data/Level" + to_string((_uint)eLevel + 1) + "Object.dat";

    ifstream  InTerrain(strFileTerrain, ios::binary);
    ifstream  InObject(strFileObject, ios::binary);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    pGameInstance->Delete_Layer(LEVEL_EDIT, TEXT("Layer_Terrain"));
    pGameInstance->Delete_Layer(LEVEL_EDIT, TEXT("Layer_Object"));

    if (nullptr != m_pSelectObject)
        m_pSelectObject = nullptr;

#pragma region Terrain
    if (InTerrain.is_open())
    {
        InTerrain >> m_iNumVerticesX[m_iCurLevel];
        InTerrain >> m_iNumVerticesZ[m_iCurLevel];
        InTerrain >> m_bIsWireFrame[m_iCurLevel];
    }
    else
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
        
    CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
    ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

    TerrainDesc.iNumVerticesX = m_iNumVerticesX[m_iCurLevel];
    TerrainDesc.iNumVerticesZ = m_iNumVerticesZ[m_iCurLevel];
    TerrainDesc.bIsWireFrame = m_bIsWireFrame[m_iCurLevel];

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
    {
       
    }
    m_pSelectTerrain = pGameInstance->Last_GameObject(LEVEL_EDIT, TEXT("Layer_Terrain"));
    if (nullptr == m_pSelectTerrain)
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
    m_bIsCreateTerrain[m_iCurLevel] = true;
#pragma endregion


#pragma region Object
    if (InObject.is_open())
    {
        _uint iType;
        OBJECT_TYPE eType;

        _int iStrSize;
        string strTag;
        wstring wstrTag;

        XMFLOAT4X4 matObject;
        _float  elements[4][4];

        while (true)
        {
            InObject >> iType;
            for (size_t i = 0; i < 4; i++)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    InObject >> elements[i][j];
                }
            }
            InObject >> iStrSize;
            char* filePath = new char[iStrSize + 1];
            InObject.read(filePath, iStrSize);
            filePath[iStrSize] = '\0';



            eType = OBJECT_TYPE(iType);
            for (size_t i = 0; i < 4; i++)
            {
                for (size_t j = 0; j < 4; j++)
                {
                    matObject.m[i][j] = elements[i][j];
                }
            }
            strTag = filePath;
            wstrTag.assign(strTag.begin(), strTag.end());
            Safe_Delete_Array(filePath);

            if (!InObject)
                break;

            if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, TEXT("Layer_Object"), wstrTag)))
                return E_FAIL;

            CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_EDIT, TEXT("Layer_Object"));
            if (nullptr == pObject)
                return E_FAIL;

            CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
            pTransform->Set_WorldMatrix(matObject);

            wstring strObjectName = pObject->Get_Name();
            string strSelectNameIndex;
            strSelectNameIndex.assign(strObjectName.begin(), strObjectName.end());
            strSelectNameIndex += to_string(pObject->Get_Index());

            m_vecObjectList.push_back(strSelectNameIndex);
            m_iSelectObject[m_iCurLevel] = m_vecObjectList.size() - 1;
        }
    }
    else
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
#pragma endregion
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

void CImgui_Manager::Free()
{
	__super::Free();

    DeleteMakeShift_Data();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
