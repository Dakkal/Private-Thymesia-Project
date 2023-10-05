#include "..\Public\Level_Edit.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "ToolCamera.h"

CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pImgui_Manager(CImgui_Manager::GetInstance())
{
	Safe_AddRef(m_pImgui_Manager);
}

HRESULT CLevel_Edit::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_ToolCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	m_pImgui_Manager->Ready_Manager(m_pDevice, m_pContext);

	return S_OK;
}

HRESULT CLevel_Edit::Tick(_float fTimeDelta)
{
	m_pImgui_Manager->Tick(fTimeDelta);


	return S_OK;
}

HRESULT CLevel_Edit::LateTick(_float fTimeDelta)
{
	m_pImgui_Manager->LateTick(fTimeDelta);


	SetWindowText(g_hWnd, TEXT("게임툴레벨 입니다."));

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Light()
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
	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eLightType = LIGHT_DESC::TYPE::POINT;
	//LightDesc.vLightPos = _vector(35.f, 3.f, 35.f, 1.f);
	//LightDesc.fLightRange = 20.f;

	//LightDesc.vDiffuse = _vector(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _vector(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _vector(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Camera(const wstring& strLayerTag)
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
	CameraToolDesc.fSpeedPerSec = 50.f;
	CameraToolDesc.fRotRadianPerSec = XMConvertToRadians(30.f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_EDIT, strLayerTag, TEXT("Prototype_GameObject_ToolCamera"), &CameraToolDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Edit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Edit::Free()
{
	__super::Free();

	Safe_Release(m_pImgui_Manager);
	CImgui_Manager::GetInstance()->DestroyInstance();
}
