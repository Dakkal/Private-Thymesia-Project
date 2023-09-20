#include "pch.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "GameCamera.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
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

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameCamera::CAMERA_GAME_DESC			CameraGameDesc;
	ZeroMemory(&CameraGameDesc, sizeof CameraGameDesc);

	CameraGameDesc.fMouseSensitive = 0.3f;
	CameraGameDesc.vEye = _vector(0.f, 0.f, -1.f, 1.f);
	CameraGameDesc.vAt = _vector(0.f, 0.f, 0.f, 1.f);
	CameraGameDesc.fFovy = XMConvertToRadians(90.0f);
	CameraGameDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraGameDesc.fNear = 0.2f;
	CameraGameDesc.fFar = 1000.f;
	CameraGameDesc.fSpeedPerSec = 80.f;
	CameraGameDesc.fRotRadianPerSec = XMConvertToRadians(30.f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera"), &CameraGameDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

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
