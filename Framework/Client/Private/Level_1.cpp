#include "pch.h"
#include "..\Public\Level_1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "ToolCamera.h"

CLevel_1::CLevel_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_1::Initialize()
{
    m_eLevel = LEVELID::LEVEL_1;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

    if (FAILED(Load_Level(m_eLevel)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_1::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_1::LateTick(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨1 입니다."));

	return S_OK;
}

HRESULT CLevel_1::Load_Level(LEVELID eLevel)
{
    string strFileTerrain = "../Bin/Data/Level" + to_string((_uint)eLevel - 2) + "Terrain.dat";
    string strFileObject = "../Bin/Data/Level" + to_string((_uint)eLevel + - 2) + "Object.dat";

    ifstream  InTerrain(strFileTerrain, ios::binary);
    ifstream  InObject(strFileObject, ios::binary);

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region Terrain
	CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

    if (InTerrain.is_open())
    {
        InTerrain >> TerrainDesc.iNumVerticesX;
        InTerrain >> TerrainDesc.iNumVerticesZ;
        InTerrain >> TerrainDesc.bIsWireFrame;
    }
    else
    {
        RELEASE_INSTANCE(CGameInstance);
        return E_FAIL;
    }

	TerrainDesc.bIsWireFrame = false;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_1, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
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

            switch (eType)
            {
            case OBJECT_TYPE::PLAYER:
            {
                if (FAILED(pGameInstance->Add_GameObject(LEVEL_1, TEXT("Layer_Player"), wstrTag)))
                    return E_FAIL;

                CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_1, TEXT("Layer_Player"));
                if (nullptr == pObject)
                    return E_FAIL;

                CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
                pTransform->Set_WorldMatrix(matObject);
            }
                break;
            case OBJECT_TYPE::PROP:
            {
                if (FAILED(pGameInstance->Add_GameObject(LEVEL_1, TEXT("Layer_Prop"), wstrTag)))
                    return E_FAIL;

                CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_1, TEXT("Layer_Prop"));
                if (nullptr == pObject)
                    return E_FAIL;

                CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
                pTransform->Set_WorldMatrix(matObject);
            }
                break;
            case OBJECT_TYPE::MONSTER:
            {
                if (FAILED(pGameInstance->Add_GameObject(LEVEL_1, TEXT("Layer_Monster"), wstrTag)))
                    return E_FAIL;

                CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_1, TEXT("Layer_Monster"));
                if (nullptr == pObject)
                    return E_FAIL;

                CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
                pTransform->Set_WorldMatrix(matObject);
            }
                break;
            default:
                break;
            }
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

HRESULT CLevel_1::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CToolCamera::CAMERA_TOOL_DESC			CameraToolDesc;
	ZeroMemory(&CameraToolDesc, sizeof CameraToolDesc);

	CameraToolDesc.fMouseSensitive = 0.3f;
	CameraToolDesc.vEye = _vector(0.f, 0.f, -1.f, 1.f);
	CameraToolDesc.vAt = _vector(0.f, 0.f, 0.f, 1.f);
	CameraToolDesc.fFovy = XMConvertToRadians(90.0f);
	CameraToolDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraToolDesc.fNear = 0.2f;
	CameraToolDesc.fFar = 1000.f;
	CameraToolDesc.fSpeedPerSec = 10.f;
	CameraToolDesc.fRotRadianPerSec = XMConvertToRadians(30.f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera"), &CameraToolDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

		return S_OK;
}

HRESULT CLevel_1::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHT_DESC			LightDesc;

	/* 방향성 광원을 추가하낟. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vLightDir = _vector(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _vector(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _vector(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _vector(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	/* 점 광원을 추가한다. */
	/*ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE::POINT;
	LightDesc.vLightPos = _vector(35.f, 3.f, 35.f, 1.f);
	LightDesc.fLightRange = 20.f;

	LightDesc.vDiffuse = _vector(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _vector(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _vector(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_1* CLevel_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_1* pInstance = new CLevel_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_1::Free()
{
	__super::Free();
}
