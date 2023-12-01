#include "pch.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "Sound_Manager.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(GetAsyncKeyState('P') & 0x8000)
		pGameInstance->PlaySoundFile(TEXT("Success.wav"), CHANNELID::CHANNEL_1, 0.5f);
	if (GetAsyncKeyState('U') & 0x8000)
		pGameInstance->CheckPlaySoundFile(TEXT("Quest Complete.wav"), CHANNELID::CHANNEL_1, 0.5f);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::LateTick(_float fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Key_Down(VK_RETURN))
	{
	

#ifdef _DEBUG
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return E_FAIL;
#else
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return E_FAIL;
#endif // !EDIT


		
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _uint& iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, iLayerIndex, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
