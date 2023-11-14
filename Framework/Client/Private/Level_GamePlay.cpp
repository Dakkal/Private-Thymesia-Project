#include "pch.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "ToolCamera.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	/*if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_SkyDome(LAYER_SKY)))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Props(LAYER_PROP)))
		return E_FAIL;*/

	if (FAILED(Load_Level(LEVEL_1)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Boss(LAYER_BOSS)))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_Monster(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::LateTick(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨 입니다."));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Check_Collision(LEVEL_GAMEPLAY, LAYER_PLAYER, LAYER_BOSS, fTimeDelta);
	pGameInstance->Check_Collision(LEVEL_GAMEPLAY, LAYER_PLAYER, LAYER_MONSTER, fTimeDelta);

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Level(LEVELID eLevel)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//wstring strFileTerrain = TEXT("../Bin/Data/Level") + to_wstring((_uint)eLevel - 2) + TEXT("Terrain.dat");
	wstring strFileObject = TEXT("../Bin/Data/Level") + to_wstring((_uint)eLevel - 2) + TEXT("Object.dat");

#pragma region Terrain
	//CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
	//ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

 //   shared_ptr<CAsFileUtils> LoadTerrain = make_shared<CAsFileUtils>();
 //   LoadTerrain->Open(strFileTerrain, FileMode::Read);

 //   LoadTerrain->Read<_int>(TerrainDesc.iNumVerticesX);
 //   LoadTerrain->Read<_int>(TerrainDesc.iNumVerticesZ);
 //   LoadTerrain->Read<_bool>(TerrainDesc.bIsWireFrame);
	//TerrainDesc.bIsWireFrame = false;

 //   if (FAILED(pGameInstance->Add_GameObject(LEVEL_1, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Edit_Terrain"), &TerrainDesc)))
 //   {
 //       RELEASE_INSTANCE(CGameInstance);
 //       return E_FAIL;
 //   }
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


		switch (eType)
		{
		case OBJECT_TYPE::PLAYER:
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_PLAYER, wstrTag)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_PLAYER);
			if (nullptr == pObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
			pTransform->Set_WorldMatrix(matObject);
		}
		break;
		case OBJECT_TYPE::PORP:
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_PROP, wstrTag)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_PROP);
			if (nullptr == pObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
			pTransform->Set_WorldMatrix(matObject);
		}
		break;
		case OBJECT_TYPE::MONSTER:
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, wstrTag)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER);
			if (nullptr == pObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
			pTransform->Set_WorldMatrix(matObject);
		}
		break;
		case OBJECT_TYPE::BOSS:
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, wstrTag)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			CGameObject* pObject = pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER);
			if (nullptr == pObject)
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
				

			CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
			pTransform->Set_WorldMatrix(matObject);
		}
		break;
		default:
			break;
		}
	}
#pragma endregion
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyDome(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_SkyDome"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CToolCamera::CAMERA_TOOL_DESC			CameraToolDesc;
	ZeroMemory(&CameraToolDesc, sizeof CameraToolDesc);

	CameraToolDesc.fMouseSensitive = 10.f;
	CameraToolDesc.vEye = _vector(0.f, 0.f, -1.f, 1.f);
	CameraToolDesc.vAt = _vector(0.f, 0.f, 0.f, 1.f);
	CameraToolDesc.fFovy = XMConvertToRadians(45.f);
	CameraToolDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraToolDesc.fNear = 0.1f;
	CameraToolDesc.fFar = 1000.f;
	CameraToolDesc.fSpeedPerSec = 10.f;
	CameraToolDesc.fRotRadianPerSec = XMConvertToRadians(90.f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_PlayerCamera"), &CameraToolDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Props(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_Church"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Boss(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_Boss_Urd"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, iLayerIndex, TEXT("Prototype_GameObject_Enemy_GreatSword"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHT_DESC			LightDesc;

	/* 방향성 광원을 추가하낟. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vLightDir = _vector(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _vector(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vAmbient = _vector(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _vector(0.f, 0.f, 0.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	/* 점 광원을 추가한다. */
	/*ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::TYPE::POINT;
	LightDesc.vLightPos = _vector(50.f, 20.f, 10.f, 1.f);
	LightDesc.fLightRange = 120.f;
	LightDesc.vDiffuse = _vector(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vAmbient = _vector(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _vector(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
