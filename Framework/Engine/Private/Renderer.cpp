#include "..\Public\Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "TargetManager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTargetManager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	_uint				iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &m_ClientViewPort);


	POINT Shadow;
	Shadow.x = m_ClientViewPort.Width * 9.f;
	Shadow.y = m_ClientViewPort.Height * 9.f;

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"),
		m_ClientViewPort.Width, m_ClientViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _vector(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"),
		m_ClientViewPort.Width, m_ClientViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _vector(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"),
		m_ClientViewPort.Width, m_ClientViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _vector(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"),
		m_ClientViewPort.Width, m_ClientViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _vector(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"),
		m_ClientViewPort.Width, m_ClientViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _vector(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_LightDepth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LightDepth"),
		Shadow.x, Shadow.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _vector(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.0f, 100.f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.0f, 300.0f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.0f, 500.0f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.0f, 100.f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.0f, 300.0f, 200.0f, 200.0f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_LightDepth"), m_ClientViewPort.Width - 250.0f, 250.0f, 500.0f, 500.0f)))
		return E_FAIL;

#endif
	/* For.MRT_GameObjects */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	/* For.MRT_Lights */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_LightDepth"))))
		return E_FAIL;


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::tElements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_WorldMatrix = XMMatrixIdentity();
	m_WorldMatrix._11 = m_ClientViewPort.Width;
	m_WorldMatrix._22 = m_ClientViewPort.Height;
	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(m_ClientViewPort.Width, m_ClientViewPort.Height, 0.f, 1.f);

	if (FAILED(Create_Shadow_ViewPort(Shadow)))
		return E_FAIL;

	if (FAILED(Create_Shadow_DSV(Shadow)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRG, CGameObject* pGameobject)
{
	if (eRG >= RENDERGROUP::RG_END || eRG < RENDERGROUP::RG_PRIORITY)
		return E_FAIL;

	m_listRenderObject[eRG].push_back(pGameobject);

	Safe_AddRef(pGameobject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderObject()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_LightDepth()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Create_Shadow_ViewPort(POINT Shadow)
{
	ZeroMemory(&m_ShadowViewPort, sizeof(D3D11_VIEWPORT));

	m_ShadowViewPort.TopLeftX = 0;
	m_ShadowViewPort.TopLeftY = 0;
	m_ShadowViewPort.Width = (_float)Shadow.x;
	m_ShadowViewPort.Height = (_float)Shadow.y;
	m_ShadowViewPort.MinDepth = 0.f;
	m_ShadowViewPort.MaxDepth = 1.f;

	return S_OK;
}

HRESULT CRenderer::Create_Shadow_DSV(POINT Shadow)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = Shadow.x;
	TextureDesc.Height = Shadow.y;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_LightDepth()
{
	m_pContext->RSSetViewports(1, &m_ShadowViewPort);

	m_pContext->ClearDepthStencilView(m_pShadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightDepth"), m_pShadowDSV)))
		return E_FAIL;

	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_SHADOW])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_SHADOW].clear();

	m_pContext->RSSetViewports(1, &m_ClientViewPort);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_NONBLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* 빛연산의 결과를 그려놓을 Shade + Specular 타겟을 장치에 바인딩 한다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Lights"))))
		return E_FAIL;


	/* 사각형 버퍼를 직교투영으로 Shade타겟의 사이즈만큼 꽉 채워서 그릴꺼야. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix ViewInv = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix ProjInv = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_vector CamPos = pPipeLine->Get_CamPosition_Vector();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &ViewInv)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &ProjInv)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_vector))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);


	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);


	/* 다시 장치의 0번째 소켓에 백 버퍼를 올린다. */
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;



	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_LightDepth"), "g_LightDepthTexture")))
		return E_FAIL;


	_matrix		ViewMatrix, ProjMatrix;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != pGameInstance->Get_ShadowLightDesc(0))
	{
		_vector vLightPos = pGameInstance->Get_ShadowLightDesc(0)->vLightPos;
		_vector vLightAt = pGameInstance->Get_ShadowLightDesc(0)->vLightAt;

		ViewMatrix = XMMatrixLookAtLH(vLightPos, vLightAt, _vector(0.f, 1.f, 0.f, 0.f));
		ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (_float)m_ClientViewPort.Width / m_ClientViewPort.Height, 0.1f, 1000.f);
	}

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &ProjMatrix)))
		return E_FAIL;


	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix		ViewMatrixInv = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix		ProjMatrixInv = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &ViewMatrixInv)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &ProjMatrixInv)))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Tool()
{
	for (auto& pGameObject : m_listRenderObject[(_uint)RENDERGROUP::RG_TOOL])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_listRenderObject[(_uint)RENDERGROUP::RG_TOOL].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_RenderDebug)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_RenderDebug.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_LightDepth"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(CGameObject* pOwner, void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShadowDSV);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
}
