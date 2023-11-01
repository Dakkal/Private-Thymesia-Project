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

	if (FAILED(Ready_Layer_Props(LAYER_PROP)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Boss(LAYER_BOSS)))
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

	RELEASE_INSTANCE(CGameInstance)

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

HRESULT CLevel_GamePlay::Ready_Light()
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
