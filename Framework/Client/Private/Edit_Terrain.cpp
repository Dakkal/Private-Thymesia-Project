#include "pch.h"
#include "..\Public\Edit_Terrain.h"
#include "GameInstance.h"

CEdit_Terrain::CEdit_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEdit_Terrain::CEdit_Terrain(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEdit_Terrain::Initialize_Prototype()
{
	m_strObjectName = TEXT("Object_Edit_Terrain");

	return S_OK;
}

HRESULT CEdit_Terrain::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	CVIBuffer_Terrain::TERRAIN_DESC* TerrainDesc = (CVIBuffer_Terrain::TERRAIN_DESC*)pArg;

	m_iNumVerticesX = TerrainDesc->iNumVerticesX;
	m_iNumVerticesZ = TerrainDesc->iNumVerticesZ;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEdit_Terrain::Tick(_float fTimeDelta)
{
}

void CEdit_Terrain::LateTick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CEdit_Terrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CEdit_Terrain::Set_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
}

HRESULT CEdit_Terrain::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	CVIBuffer_Terrain::TERRAIN_DESC			TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof TerrainDesc);

	TerrainDesc.iNumVerticesX = m_iNumVerticesX;
	TerrainDesc.iNumVerticesZ = m_iNumVerticesZ;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_VIBuffer_Edit_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &TerrainDesc)))
		return E_FAIL;

	/* Com_Texture*/

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Terrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEdit_Terrain* CEdit_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_Terrain* pInstance = new CEdit_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEdit_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEdit_Terrain::Clone(void* pArg)
{
	__super::Clone(pArg);

	CEdit_Terrain* pInstance = new CEdit_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEdit_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}