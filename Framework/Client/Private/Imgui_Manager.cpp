#include "pch.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "imgui_spectrum.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "FbxExporter.h"

#include "Edit_Terrain.h"
#include "Input_Device.h"
#include "Layer.h"
#include "VIBuffer.h"
#include "BinMesh.h"
#include "Navigation.h"
#include "LandObject.h"

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
        if (OBJECT_TYPE::PORP == iter.second->Get_ObjectType())
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

    /* 네비메쉬 */
    NaviMesh();

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
                pTransform->Set_Scale(_float3(m_vMonsterScale[m_iCurLevel]));
                pTransform->Set_State(CTransform::STATE_POS, m_vMonsterPos[m_iCurLevel]);
            }
            break;
        case OBJECT_TYPE::PORP:
            if (m_IsPropTransformOpen[m_iCurLevel])
            {
                pTransform->Set_Scale(_float3(m_vPropScale[m_iCurLevel]));
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
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }  
        }

        if (ImGui::MenuItem("Level 2"))
        {
            // "Level2" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }
        }
        if (ImGui::MenuItem("Level 3"))
        {
            // "Level3" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
            {
                m_bShowMessage[_uint(DATA::SAVE)] = true;
                Save_Data(LEVEL(m_SelectLevel));
            }
        }

        if (ImGui::MenuItem("Level 4"))
        {
            // "Level4" 메뉴 아이템이 클릭될 때 수행할 작업
            if (m_iCurLevel == m_SelectLevel)
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

            pGameInstance->Delete_Layer(LEVEL_EDIT, LAYER_EDITERRAIN);
            pGameInstance->Delete_Layer(LEVEL_EDIT, LAYER_EDITOBJECT);

            if (!m_vecObjectList.empty())
            {
                m_vecObjectList.clear();
            }

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

                    if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
                    {
                        RELEASE_INSTANCE(CGameInstance);
                        return E_FAIL;
                    }
                    
                    m_pSelectTerrain = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN);
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

                if (FAILED(pGameInstance->Delete_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index())))
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

                CEdit_Terrain* pInstance = dynamic_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index()));

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

                CEdit_Terrain* pInstance = dynamic_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index()));

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
                            
                        }
                        m_strCurPropProtoObject[m_iCurLevel].assign(m_vecProps[i].begin(), m_vecProps[i].end());
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();

                /* 클론생성 */
                if (ImGui::Button("Clone"))
                {
                    if (TEXT("") != m_strCurPropProtoObject[m_iCurLevel])
                    {
                        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

                        if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT, m_strCurPropProtoObject[m_iCurLevel])))
                            return E_FAIL;

                        m_pSelectObject = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT);
                        if (nullptr == m_pSelectObject)
                            return E_FAIL;

                        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));

                        if (0 == m_vTerrainPos[m_iCurLevel].w)
                            m_vTerrainPos[m_iCurLevel].w = 1;
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

                    pGameInstance->Delete_Layer(LEVEL_EDIT, LAYER_EDITOBJECT);

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

                        pTransform->Set_Scale(_float3(vRight.Length(), vUp.Length(), vLook.Length()));
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

    if (true == m_bisTerrainPick && true == m_bIsCreateTerrain[m_iCurLevel])
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

        CEdit_Terrain* pInstance = dynamic_cast<CEdit_Terrain*>(pGameInstance->Find_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Object_Edit_Terrain"), m_pSelectTerrain->Get_Index()));

        _vector vTerrainPos = pInstance->Picking_Terrain();

        ImGui::Spacing();
        ImGui::Text("Terrain X : %.2f", vTerrainPos.x);
        ImGui::Text("Terrain Y : %.2f", vTerrainPos.y);
        ImGui::Text("Terrain Z : %.2f", vTerrainPos.z);
        vTerrainPos.w = 1.f;

        if (false == Is_MouseClickedGUI() && pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON))
        {
            m_vTerrainPos[m_iCurLevel] = vTerrainPos;
        }

        ImGui::Spacing();
        ImGui::Text("SavePos X : %.2f", m_vTerrainPos[m_iCurLevel].x);
        ImGui::Text("SavePos Y : %.2f", m_vTerrainPos[m_iCurLevel].y);
        ImGui::Text("SavePos Z : %.2f", m_vTerrainPos[m_iCurLevel].z);
        vTerrainPos.w = 1.f;

        RELEASE_INSTANCE(CGameInstance);
    }


    ImGui::End();

    return S_OK;;
}

HRESULT CImgui_Manager::List_Object()
{
    ImGui::Begin("Object List");
   
    if (ImGui::BeginListBox("##List", ImVec2(ImGui::GetContentRegionAvail().x, 200)))
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

                m_pSelectObject = pGameInstance->Find_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT, walpha, stoi(digits));
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

            if (FAILED(pGameInstance->Delete_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT, m_pSelectObject->Get_Name(), m_pSelectObject->Get_Index())))
            {
                RELEASE_INSTANCE(CGameInstance);
                return E_FAIL;
            }

            m_pSelectObject = nullptr;
            m_iSelectObject[m_iCurLevel] = 0;

            RELEASE_INSTANCE(CGameInstance);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Pick Object ON"))
    {
        if (false == m_bisObjectPick)
        {
            m_bisObjectPick = true;
            m_bisTerrainPick = false;
        }
        else
        {
            m_bisObjectPick = false;
            m_bisTerrainPick = true;
        }
    }

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    if (true == m_bisObjectPick && false == Is_MouseClickedGUI() && pGameInstance->Key_Down(VK_LBUTTON))
    {
        RECT rc = { 0, 0, g_iWinSizeX, g_iWinSizeY };

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(g_hWnd, &pt);

        const list<CGameObject*>* pList = pGameInstance->Get_LayerList(LEVEL_EDIT, LAYER_EDITOBJECT);

        _bool IsFirstCheck = true;
        _vector vClosePick;

        if (nullptr != pList)
        {
            for (auto& pObject : *pList)
            {
                CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
                CBinModel* pModel = dynamic_cast<CBinModel*>(pObject->Get_Component(TEXT("Com_Model")));

                vector<CBinMesh*> Meshes = pModel->Get_Meshes();
                

                for (auto& pMesh : Meshes)
                {
                    _vector vPick = pGameInstance->Picking_Object(rc, pt, pTransform, dynamic_cast<CVIBuffer*>(pMesh));

                    if (-1 != vPick.w)
                    {
                        if (false == IsFirstCheck)
                        {
                            _vector vPickLenght = pGameInstance->Get_CamPosition_Vector() - vPick;
                            _vector vClosePickLenght = pGameInstance->Get_CamPosition_Vector() - vClosePick;
                          
                            if (vPickLenght.Length() < vClosePickLenght.Length())
                            {
                                vClosePick = vPick;
                                m_vObjectPos[m_iCurLevel] = vClosePick;
                                m_pSelectObject = pObject;
                                ChangeListToSelectObj();
                            }
                        }
                        else
                        {
                            vClosePick = vPick;
                            m_vObjectPos[m_iCurLevel] = vClosePick;

                            m_pSelectObject = pObject;
                            ChangeListToSelectObj();

                            IsFirstCheck = false;
                        }
                        
                    }
                }
            }

            if (true == m_bIsMakeNavi)
            {
                _vector vPick = m_vObjectPos[m_iCurLevel];
                _float3 vRealPick = dynamic_cast<CLandObject*>(m_pSelectObject)->Get_CurNaviCom()->Get_Closet_Cell_Point(vPick);
                m_Points.push_back(vRealPick);
            }
        }

        if (0 == vClosePick.w)
        {
            m_vObjectPos[m_iCurLevel] = XMVectorZero();
        }
    }

    ImGui::Spacing();
    ImGui::Text("MeshPos X : %.2f", m_vObjectPos[m_iCurLevel].x);
    ImGui::Text("MeshPos Y : %.2f", m_vObjectPos[m_iCurLevel].y);
    ImGui::Text("MeshPos Z : %.2f", m_vObjectPos[m_iCurLevel].z);

    RELEASE_INSTANCE(CGameInstance);
    

   
    ImGui::End();

    return S_OK;;
}

void CImgui_Manager::ChangeListToSelectObj()
{
    if (nullptr != m_pSelectObject && 0 < m_vecObjectList.size())
    {
        wstring strObjectName = m_pSelectObject->Get_Name();
        string strSelectNameIndex;
        strSelectNameIndex.assign(strObjectName.begin(), strObjectName.end());
        strSelectNameIndex += to_string(m_pSelectObject->Get_Index());

        _uint iIndex = 0;
        for (auto& list : m_vecObjectList)
        {
            if (list == strSelectNameIndex)
                break;

            ++iIndex;
        }
        m_iSelectObject[m_iCurLevel] = iIndex;

        CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));

        m_matStore[m_iCurLevel] = pTransform->Get_WorldMatrix();
        m_vPropScale[m_iCurLevel] = pTransform->Get_Scale();
        m_vPropRot[m_iCurLevel] = XMVectorZero();
        m_vPropPos[m_iCurLevel] = pTransform->Get_State(CTransform::STATE_POS);
;
    }
}

HRESULT CImgui_Manager::NaviMesh()
{
    ImGui::Begin("Navi Mesh");

    if (ImGui::Button("Create NaviMesh"))
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

        if (nullptr != m_pSelectObject)
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
            _matrix mat = pTransform->Get_WorldMatrix();
            CNavigation* pNavi = CNavigation::Create(m_pDevice, m_pContext, mat);

            dynamic_cast<CLandObject*>(m_pSelectObject)->Set_CurNaviCom(pNavi);
        }
        RELEASE_INSTANCE(CGameInstance);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        wstring strName = m_pSelectObject->Get_Name();
        wstring strPath = TEXT("../Bin/Data/Navigation/") + strName + TEXT(".dat");

        dynamic_cast<CLandObject*>(m_pSelectObject)->Get_CurNaviCom()->Save_Navi(strPath);
    }
    if (ImGui::Button("Load NaviMesh"))
    {
        CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

        if (nullptr != m_pSelectObject)
        {
            wstring strPath = TEXT("../Bin/Data/Navigation/");
            wstring ObjName = m_pSelectObject->Get_Name();
            wstring ext = TEXT(".dat");
            wstring finalpath = strPath + ObjName + ext;
            CNavigation* pNavi = CNavigation::Create(m_pDevice, m_pContext, finalpath);

            dynamic_cast<CLandObject*>(m_pSelectObject)->Set_CurNaviCom(pNavi);
        }
        RELEASE_INSTANCE(CGameInstance);
    }
    if (ImGui::CollapsingHeader("Let's Make Navi"))
    {
        if (ImGui::Button("Start|Pause"))
        {
            if (false == m_bIsMakeNavi)
            {
                m_bIsMakeNavi = true;
            }
            else
            {
                m_bIsMakeNavi = false;
                m_Points.clear();
            }

               
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            dynamic_cast<CLandObject*>(m_pSelectObject)->Get_CurNaviCom()->Delete_Last_Cell();
            m_Points.clear();
        }
        ImGui::Spacing();
        if (ImGui::Button("Get Index"))
        {
            m_iInCell = dynamic_cast<CLandObject*>(m_pSelectObject)->Get_CurNaviCom()->CheckIn(m_vObjectPos[m_iCurLevel]);
        }
        ImGui::SameLine();
        if (ImGui::Button("SetPassage"))
        {
            if (false == m_bIsMakeNavi)
            {
                if (m_iInCell != -1)
                {
                    dynamic_cast<CLandObject*>(m_pSelectObject)->Get_CurNaviCom()->Set_CelltoPassage(m_iInCell);
                }
            }  
        }
        ImGui::Spacing();
        ImGui::Text("Navi Index : %d", m_iInCell);
        if (true == m_bIsMakeNavi)
        {
            ImGui::Text("Make Cell Activated");
        }
        else
        {
            ImGui::Text("Make Cell DeActivated");
        }

        if (nullptr != m_pSelectObject && true == m_bIsMakeNavi)
        {
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

            if (3 <= m_Points.size())
            {
                _float3* pPoints = new _float3[3];

                for (size_t i = 0; i < 3; i++)
                {
                    pPoints[i] = m_Points[i];
                }
                SortPoint(pPoints);
                dynamic_cast<CLandObject*>(m_pSelectObject)->Get_CurNaviCom()->Add_Cell(pPoints);

                Safe_Delete_Array(pPoints);
                m_Points.clear();
            }

            RELEASE_INSTANCE(CGameInstance);
        }
       
    }
  

    ImGui::End();

    return S_OK;
}

HRESULT CImgui_Manager::SortPoint(_float3* pPoints)
{
    _float fValue1 = pPoints[0].x * pPoints[1].z + pPoints[1].x * pPoints[2].z + pPoints[2].x * pPoints[0].z;
    _float fValue2 = pPoints[1].x * pPoints[0].z + pPoints[2].x * pPoints[1].z + pPoints[0].x * pPoints[2].z;
    _float fResult = fValue1 - fValue2;

    if (fResult > 0)
    {
        _float3 vTemp = pPoints[0];
        pPoints[0] = pPoints[2];
        pPoints[2] = vTemp;
    }
    return S_OK;
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
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    //Terrain
    wstring strFileTerrain = TEXT("../Bin/Data/MakeShift/MakeShiftLv") + to_wstring((_uint)eLevel + 1) + TEXT("Terrain.dat");
    wstring strFileObject = TEXT("../Bin/Data/MakeShift/MakeShiftLv") + to_wstring((_uint)eLevel + 1) + TEXT("Object.dat");

    shared_ptr<CAsFileUtils> SaveTerrain = make_shared<CAsFileUtils>();
    SaveTerrain->Open(strFileTerrain, FileMode::Write);

    SaveTerrain->Write<_int>(m_iNumVerticesX[m_iCurLevel]);
    SaveTerrain->Write<_int>(m_iNumVerticesZ[m_iCurLevel]);
    SaveTerrain->Write<_bool>(m_bIsWireFrame[m_iCurLevel]);

    // Object
    shared_ptr<CAsFileUtils> SaveObject = make_shared<CAsFileUtils>();
    SaveObject->Open(strFileObject, FileMode::Write);

    auto listObject = pGameInstance->Get_LayerList(LEVEL_EDIT, LAYER_EDITOBJECT);

    _uint iObjCnt = listObject->size();
    SaveObject->Write<_uint>(iObjCnt);

    for (auto& iter : *listObject)
    {
        _uint iType = _uint(iter->Get_ObjectType());

        wstring wstrTag = iter->Get_ProtoTag();
        string strTag;
        strTag.assign(wstrTag.begin(), wstrTag.end());

        _matrix matObject = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

        SaveObject->Write<_uint>(iType);
        SaveObject->Write<string>(strTag);
        SaveObject->Write<_matrix>(matObject);
    }

    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

HRESULT CImgui_Manager::Load_MakeShift_Data(LEVEL eLevel)
{
    if (!m_vecObjectList.empty())
    {
        m_vecObjectList.clear();
    }

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    wstring strFileTerrain = TEXT("../Bin/Data/MakeShift/MakeShiftLv") + to_wstring((_uint)eLevel + 1) + TEXT("Terrain.dat");
    wstring strFileObject = TEXT("../Bin/Data/MakeShift/MakeShiftLv") + to_wstring((_uint)eLevel + 1) + TEXT("Object.dat");
   
#pragma region Terrain
    shared_ptr<CAsFileUtils> LoadTerrain = make_shared<CAsFileUtils>();
    LoadTerrain->Open(strFileTerrain, FileMode::Read);

    LoadTerrain->Read<_int>(m_iNumVerticesX[m_iCurLevel]);
    LoadTerrain->Read<_int>(m_iNumVerticesZ[m_iCurLevel]);
    LoadTerrain->Read<_bool>(m_bIsWireFrame[m_iCurLevel]);

    CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
    ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

    TerrainDesc.iNumVerticesX = m_iNumVerticesX[m_iCurLevel];
    TerrainDesc.iNumVerticesZ = m_iNumVerticesZ[m_iCurLevel];
    TerrainDesc.bIsWireFrame = m_bIsWireFrame[m_iCurLevel];

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
    m_pSelectTerrain = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN);
    if (nullptr == m_pSelectTerrain)
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
#pragma endregion

  
#pragma region Object
        _uint iObjCnt;

        _uint iType;
        OBJECT_TYPE eType;

        string strTag;
        wstring wstrTag;

        _matrix matObject;

        shared_ptr<CAsFileUtils> LoadObject = make_shared<CAsFileUtils>();
        LoadObject->Open(strFileObject, FileMode::Read);

        LoadObject->Read<_uint>(iObjCnt);

        for (size_t i = 0; i < iObjCnt; i++)
        {
            LoadObject->Read<_uint>(iType);
            LoadObject->Read(strTag);
            LoadObject->Read<_matrix>(matObject);

            eType = OBJECT_TYPE(iType);
            wstrTag.assign(strTag.begin(), strTag.end());

            if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT, wstrTag)))
                return E_FAIL;

            CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT);
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
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    wstring strFileTerrain = TEXT("../Bin/Data/Level") + to_wstring((_uint)eLevel + 1) + TEXT("Terrain.dat");
    wstring strFileObject = TEXT("../Bin/Data/Level") + to_wstring((_uint)eLevel + 1) + TEXT("Object.dat");

    shared_ptr<CAsFileUtils> SaveTerrain = make_shared<CAsFileUtils>();
    SaveTerrain ->Open(strFileTerrain, FileMode::Write);

    SaveTerrain->Write<_int>(m_iNumVerticesX[m_iCurLevel]);
    SaveTerrain->Write<_int>(m_iNumVerticesZ[m_iCurLevel]);
    SaveTerrain->Write<_bool>(m_bIsWireFrame[m_iCurLevel]);

    shared_ptr<CAsFileUtils> SaveObject = make_shared<CAsFileUtils>();
    SaveObject->Open(strFileObject, FileMode::Write);

    auto listObject = pGameInstance->Get_LayerList(LEVEL_EDIT, LAYER_EDITOBJECT);

    _uint iObjCnt = listObject->size();
    SaveObject->Write<_uint>(iObjCnt);

    for (auto& iter : *listObject)
    {
        _uint iType = _uint(iter->Get_ObjectType());

        wstring wstrTag = iter->Get_ProtoTag();
        string strTag;
        strTag.assign(wstrTag.begin(), wstrTag.end());

        _matrix matObject = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

        SaveObject->Write<_uint>(iType);
        SaveObject->Write<string>(strTag);
        SaveObject->Write<_matrix>(matObject);
    }

    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

HRESULT CImgui_Manager::Load_Data(LEVEL eLevel)
{
    if (!m_vecObjectList.empty())
    {
        m_vecObjectList.clear();
    }

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    wstring strFileTerrain = TEXT("../Bin/Data/Level") + to_wstring((_uint)eLevel + 1) + TEXT("Terrain.dat");
    wstring strFileObject = TEXT("../Bin/Data/Level") + to_wstring((_uint)eLevel + 1) + TEXT("Object.dat");

#pragma region Terrain
    shared_ptr<CAsFileUtils> LoadTerrain = make_shared<CAsFileUtils>();
    LoadTerrain->Open(strFileTerrain, FileMode::Read);

    LoadTerrain->Read<_int>(m_iNumVerticesX[m_iCurLevel]);
    LoadTerrain->Read<_int>(m_iNumVerticesZ[m_iCurLevel]);
    LoadTerrain->Read<_bool>(m_bIsWireFrame[m_iCurLevel]);

    CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
    ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

    TerrainDesc.iNumVerticesX = m_iNumVerticesX[m_iCurLevel];
    TerrainDesc.iNumVerticesZ = m_iNumVerticesZ[m_iCurLevel];
    TerrainDesc.bIsWireFrame = m_bIsWireFrame[m_iCurLevel];

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN, TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
    m_pSelectTerrain = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITERRAIN);
    if (nullptr == m_pSelectTerrain)
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }
#pragma endregion


#pragma region Object
    _uint iObjCnt;

    _uint iType;
    OBJECT_TYPE eType;

    string strTag;
    wstring wstrTag;

    _matrix matObject;

    shared_ptr<CAsFileUtils> LoadObject = make_shared<CAsFileUtils>();
    LoadObject->Open(strFileObject, FileMode::Read);

    LoadObject->Read<_uint>(iObjCnt);

    for (size_t i = 0; i < iObjCnt; i++)
    {
        LoadObject->Read<_uint>(iType);
        LoadObject->Read(strTag);
        LoadObject->Read<_matrix>(matObject);

        eType = OBJECT_TYPE(iType);
        wstrTag.assign(strTag.begin(), strTag.end());

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT, wstrTag)))
            return E_FAIL;

        CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT);
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

   m_pSelectObject = pGameInstance->Last_GameObject(LEVEL_EDIT, LAYER_EDITOBJECT);
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
