#include "pch.h"

#include "MainApp.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "FbxExporter.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	/* 1. 내 게임의 초기화를 수행한다. */
	/* 1-1. 그래픽장치를 초기화한다. */
	/* 1-2. 사운드장치를 초기화한다. */
	/* 1-3. 입력장치를 초기화한다. */
	GRAPHIC_DESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(GraphicDesc, g_hInstance , &m_pDevice, &m_pContext, LEVEL_END)))
		return E_FAIL;

	
	if (FAILED(Ready_Prototype_Components()))
		return E_FAIL;

	/* 1-4. 게임내에서 사용할 레벨(씬)을 생성한다.   */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	/* 1-4-1. 게임내에서 사용할 여러 자원(텍스쳐, 모델, 객체) 들을 준비한다.  */


	/* 실프레임 워크에서는 지우자 */
	if (FAILED(Create_FakeTexture()))
		return E_FAIL;

	//CFbxExporter FbxExport;
	//FbxExport.Initialize_Static_Export(TEXT("../Bin/Resources/Models/Static/"));
	//FbxExport.Initialize_Dynamic_Export(TEXT("../Bin/Resources/Models/Dynamic/"));

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_vector(0.5f, 0.5f, 0.5f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	
	m_pRenderer->Draw_RenderObject();

	if (LEVEL_EDIT == m_pGameInstance->Get_CurLevel())
	{
		CImgui_Manager* pGuiManager = GET_INSTANCE(CImgui_Manager);

		pGuiManager->Render();

		RELEASE_INSTANCE(CImgui_Manager);
	}
	

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Components()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex")
		, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tElements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect")
		, CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform")
		, CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Create_FakeTexture()
{
	ID3D11Texture2D* pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof TextureDesc);

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			_uint		iIndex = i * 256 + j;


			pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	D3D11_SUBRESOURCE_DATA		InitialData;
	ZeroMemory(&InitialData, sizeof InitialData);
	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = TextureDesc.Width * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"));

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof MappedSubResource);

	pPixel[0] = D3DCOLOR_ARGB(255, 0, 0, 255);

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memmove(MappedSubResource.pData, pPixel, sizeof(_ulong) * 256 * 256);

	m_pContext->Unmap(pTexture2D, 0);

	SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/MyMask.dds"));

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture2D);

	// 가라 네비
	CAsFileUtils Out;
	Out.Open(TEXT("../Bin/Data/Navigation.dat"), FileMode::Write);

	_float3			vPoints;
	vector<_float3> vecPoints;

	vecPoints.push_back(_float3(0.f, 0.f, 10.f));
	vecPoints.push_back(_float3(10.f, 0.f, 0.f));
	vecPoints.push_back(_float3(0.f, 0.f, 0.f));

	vecPoints.push_back(_float3(0.f, 0.f, 10.f));
	vecPoints.push_back(_float3(10.f, 0.f, 10.f));
	vecPoints.push_back(_float3(10.f, 0.f, 0.f));

	vecPoints.push_back(_float3(0.f, 0.f, 20.f));
	vecPoints.push_back(_float3(10.f, 0.f, 10.f));
	vecPoints.push_back(_float3(0.f, 0.f, 10.f));

	vecPoints.push_back(_float3(10.f, 0.f, 10.f));
	vecPoints.push_back(_float3(20.f, 0.f, 0.f));
	vecPoints.push_back(_float3(10.f, 0.f, 0.f));

	Out.Write<_uint>(vecPoints.size());

	for (size_t i = 0; i < vecPoints.size(); i++)
	{
		Out.Write<_float3>(vecPoints[i]);
	}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainApp::Free()
{
    __super::Free();

	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();
}
