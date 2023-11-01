#include "pch.h"
#include "Weapon_Boss_Urd.h"

#include "GameInstance.h"
#include "BinBone.h"
#include "Bounding_Sphere.h"
#include "Collider.h"

CWeapon_Boss_Urd::CWeapon_Boss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CWeapon_Boss_Urd::CWeapon_Boss_Urd(const CWeapon_Boss_Urd& rhs)
	: CPartObject(rhs)
{

}

HRESULT CWeapon_Boss_Urd::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PART;
	m_strObjectName = TEXT("Boss_Urd_Weapon");

	return S_OK;
}

HRESULT CWeapon_Boss_Urd::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 부모 소켓행렬을 기준으로 자식의 상태를 제어한다.  */
	m_pTransformCom->Fix_Rotation(_vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	return S_OK;
}

void CWeapon_Boss_Urd::Tick(_float fTimeDelta)
{
	XMMATRIX	WorldMatrix = m_pSocketBone->Get_CombinedTransform() * m_SocketPivotMatrix;

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);

	/* 계산한뒤에 월드를 업데이트 */
	m_pColliderCom->Update(m_WorldMatrix);
}

void CWeapon_Boss_Urd::LateTick(_float fTimeDelta)
{
	m_pColliderCom->LateUpdate();
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_BLEND, this);
}

HRESULT CWeapon_Boss_Urd::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CWeapon_Boss_Urd::OnCollision_Enter(CGameObject* _pColObj)
{
}

void CWeapon_Boss_Urd::OnCollision_Stay(CGameObject* _pColObj)
{
}

void CWeapon_Boss_Urd::OnCollision_Exit(CGameObject* _pColObj)
{
}

HRESULT CWeapon_Boss_Urd::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss_Urd_Weapon"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC	SphereDesc = {};
	SphereDesc.fRadius = 0.12f;
	SphereDesc.vCenter = _float3(0.9f, 0.f, 0.f);
	SphereDesc.vCollideColor = _vector(1.f, 0.f, 0.f, 1.f);
	SphereDesc.vColor = _vector(0.33f, 0.63f, 0.93f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Boss_Urd::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_CamPosToShader(m_pShaderCom, "g_vCamPosition")))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	_uint		iPassIndex = 0;

	if (LIGHT_DESC::TYPE::DIRECTIONAL == pLightDesc->eLightType)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vLightDir, sizeof(_vector))))
			return E_FAIL;
		iPassIndex = 0;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPos", &pLightDesc->vLightPos, sizeof(_vector))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &pLightDesc->fLightRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = 1;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_vector))))
		return E_FAIL;

	Safe_Release(pGameInstance);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

CWeapon_Boss_Urd* CWeapon_Boss_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CWeapon_Boss_Urd* pInstance = new CWeapon_Boss_Urd(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CWeapon_Boss_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Boss_Urd::Clone(void* pArg)
{
	__super::Clone(pArg);

	CWeapon_Boss_Urd* pInstance = new CWeapon_Boss_Urd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Boss_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Boss_Urd::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
