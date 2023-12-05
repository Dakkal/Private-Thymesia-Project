#include "..\Public\ParrySpark.h"

#include "GameInstance.h"
#include "VIBuffer_Rect_Instance.h"

CParrySpark::CParrySpark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject(pDevice, pContext)
{
}

CParrySpark::CParrySpark(const CParrySpark& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CParrySpark::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	return S_OK;
}

HRESULT CParrySpark::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_iNumInstance = 40;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Create_RandomRotMatrix(m_vRotFactorX, m_vRotFactorY, m_vRotFactorZ);

	return S_OK;
}

void CParrySpark::PriorityTick(_float fTimeDelta)
{
}

void CParrySpark::Tick(_float fTimeDelta)
{
	if (false == m_pVIRectBufferCom->Is_Update())
		m_IsActive = false;

	if (false == m_IsActive)
		return;

	m_pVIRectBufferCom->Update(CVIBuffer_Instancing::Y, fTimeDelta);
}

void CParrySpark::LateTick(_float fTimeDelta)
{
	if (false == m_IsActive)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	m_pTransformCom->LookAt_NoYaw(pGameInstance->Get_CamPosition_Vector());
	m_pTransformCom->Set_State(CTransform::STATE_POS, m_vTargetPos);

	RELEASE_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CParrySpark::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pVIRectBufferCom->Render();

	return S_OK;
}

HRESULT CParrySpark::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Rect_Instance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParryFlare"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Component_VIBuffer_Rect_Instance */
	CVIBuffer_Rect_Instance::INSTANCE_DESC	InstanceDesc = {};

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.5f);
	InstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
	InstanceDesc.fRangeOffset = 0.1f;
	InstanceDesc.vScaleMin = _float3(0.1f, 0.1f, 0.1f);
	InstanceDesc.vScaleMax = _float3(0.25f, 1.f, 0.25f);
	InstanceDesc.iNumInstance = m_iNumInstance;
	InstanceDesc.fLifeTimeMin = 0.2f;
	InstanceDesc.fLifeTimeMax = 0.4f;
	InstanceDesc.fSpeedMin = 6.f;
	InstanceDesc.fSpeedMax = 8.f;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIRectBufferCom, &InstanceDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParrySpark::Bind_ShaderResources()
{
	__super::Bind_ShaderResources();

	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTargetTransform->Bind_ShaderResources(m_pShaderCom, "g_TargetMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	return S_OK;
}

CParrySpark* CParrySpark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CParrySpark* pInstance = new CParrySpark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CParrySpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParrySpark::Clone(void* pArg)
{
	CParrySpark* pInstance = new CParrySpark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParrySpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParrySpark::Free()
{
	__super::Free();

	Safe_Release(m_pVIRectBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
