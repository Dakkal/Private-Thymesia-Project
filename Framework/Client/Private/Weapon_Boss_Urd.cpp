#include "pch.h"
#include "Weapon_Boss_Urd.h"

#include "GameInstance.h"
#include "BinBone.h"
#include "Bounding_OBB.h"
#include "Collider.h"
#include "Boss_Urd.h"

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
	if (false == Is_Active())
		return;

	XMMATRIX	WorldMatrix = m_pSocketBone->Get_CombinedTransform() * m_SocketPivotMatrix;

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);

	if (true == g_BossSeq && true == dynamic_cast<CBoss_Urd*>(m_pOwner)->Is_Seq1())
	{
		XMMATRIX	WorldMatrix = m_pSocketBoneforPivot->Get_CombinedTransform() * m_SocketPivotMatrix;

		WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
		WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
		WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

		Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);
	}


	/* 계산한뒤에 월드를 업데이트 */
	m_pColliderCom->Update(m_WorldMatrix);
}

void CWeapon_Boss_Urd::LateTick(_float fTimeDelta)
{
	if (false == Is_Active())
		return;

	m_pColliderCom->LateUpdate();


	if (true == m_pOwner->Is_Cull())
	{
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
		if (false == m_pOwner->Is_Dead())
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
	}
}

HRESULT CWeapon_Boss_Urd::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool	Is_Normal, Is_ORM;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		Is_Normal = Is_ORM = true;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			Is_Normal = false;

		if (true == m_pOwner->Is_Dead())
		{
			if (true == Is_Normal)
			{
				if (FAILED(m_pShaderCom->Begin(5)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(4)))
					return E_FAIL;
			}

		}
		else
		{
			if (true == Is_Normal)
			{
				if (FAILED(m_pShaderCom->Begin(1)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;
			}
		}

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Boss_Urd::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_matrix		ViewMatrix, ProjMatrix;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vLightPos = pGameInstance->Get_ShadowLightDesc(0)->vLightPos;
	_vector vLightAt = pGameInstance->Get_ShadowLightDesc(0)->vLightAt;

	ViewMatrix = XMMatrixLookAtLH(vLightPos, vLightAt, _vector(0.f, 1.f, 0.f, 0.f));
	ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f);

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CWeapon_Boss_Urd::OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PLAYER:
		break;
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Enter(_pColObj, fTimeDelta);
		break;
	}
}

void CWeapon_Boss_Urd::OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PLAYER:
		break;
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Stay(_pColObj, fTimeDelta);
		break;
	}
}

void CWeapon_Boss_Urd::OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PLAYER:
		break;
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Exit(_pColObj, fTimeDelta);
		break;
	}
}

void CWeapon_Boss_Urd::OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::PLAYER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		}
	}
	break;
	case OBJECT_TYPE::PORP:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		default:
			break;
		}
	}
	break;
	case OBJECT_TYPE::MONSTER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		default:
			break;
		}
	}
	break;
	}
}

void CWeapon_Boss_Urd::OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::PLAYER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		}
	}
	break;
	case OBJECT_TYPE::PORP:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		default:
			break;
		}
	}
	break;
	case OBJECT_TYPE::MONSTER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		default:
			break;
		}
	}
	break;
	}
}

void CWeapon_Boss_Urd::OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::PLAYER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		}
	}
	break;
	case OBJECT_TYPE::PORP:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		default:
			break;
		}
	}
	break;
	case OBJECT_TYPE::MONSTER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
			break;
		case Engine::CGameObject::HITBOX:
			break;
		default:
			break;
		}
	}
	break;
	}
}

HRESULT CWeapon_Boss_Urd::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissolve"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
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

	CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc = {};
	OBBDesc.vExtents = _float3(0.55f, 0.05f, 0.05f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);
	OBBDesc.vDegree = _float3(0.0f, 0.f, 0.f);
	OBBDesc.vCollideColor = _vector(1.f, 0.5f, 0.f, 1.f);
	OBBDesc.vColor = _vector(0.33f, 0.63f, 0.93f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &OBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Boss_Urd::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (true == m_pOwner->Is_Dead())
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_pOwner->Get_DissolveTime(), sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveDuration", &m_pOwner->Get_DissolveDuration(), sizeof(_float))))
			return E_FAIL;
	}

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

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
