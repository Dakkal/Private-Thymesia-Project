#include "pch.h"
#include "..\Public\Body_Npc_Kid.h"

#include "GameInstance.h"
#include "LandObject.h"
#include "Navigation.h"
#include "BinMesh.h"

#include "StateMachine.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "Collider.h"

CBody_Npc_Kid::CBody_Npc_Kid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CBody_Npc_Kid::CBody_Npc_Kid(const CBody_Npc_Kid& rhs)
	: CPartObject(rhs)
{

}

HRESULT CBody_Npc_Kid::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PART;
	m_strObjectName = TEXT("Npc_Kid_Body");

	return S_OK;
}

HRESULT CBody_Npc_Kid::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody_Npc_Kid::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (false == g_EditMode)
	{
		m_pModelCom->Set_OwnerPosToRootPos(m_pParentTransform, fTimeDelta, dynamic_cast<CLandObject*>(m_pOwner)->Get_CurNaviCom(), m_vTargetPos);
		dynamic_cast<CLandObject*>(m_pOwner)->Set_On_NaviMesh(m_pParentTransform);
	}


	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix());

	/* 계산한뒤에 월드를 업데이트 */
	if (false == g_EditMode)
	{
		m_pColliderCom->Update(m_WorldMatrix);
	}

}

void CBody_Npc_Kid::LateTick(_float fTimeDelta)
{
	if (false == g_EditMode)
		m_pColliderCom->LateUpdate();

	if (true == m_pOwner->Is_Cull())
	{
#ifdef _DEBUG
		if (false == g_EditMode)
			m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
	}
}

HRESULT CBody_Npc_Kid::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool	Is_Normal, Is_ORM;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		Is_Normal = Is_ORM = true;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			Is_Normal = false;


		if ("Mat_CharacterSSS_Inst" == m_pModelCom->Get_MaterialName(i))
		{
			if (true == Is_Normal)
			{
				if (FAILED(m_pShaderCom->Begin(3)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(2)))
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

HRESULT CBody_Npc_Kid::Render_LightDepth()
{
	return E_NOTIMPL;
}

void CBody_Npc_Kid::OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta)
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

void CBody_Npc_Kid::OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta)
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

void CBody_Npc_Kid::OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta)
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

void CBody_Npc_Kid::OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(pPartOwner->Get_Component(TEXT("Com_Transform")));
	CNavigation* pNavigation = dynamic_cast<CLandObject*>(m_pOwner)->Get_CurNaviCom();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::PLAYER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			if (true == m_pOwner->Is_Move())
				pGameInstance->Detrude_Sphere_Collide(_pColObj, m_pColliderCom, m_pParentTransform, pNavigation);
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
	case OBJECT_TYPE::MONSTER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			if (true == m_pOwner->Is_Move())
				pGameInstance->Detrude_Sphere_Collide(_pColObj, m_pColliderCom, m_pParentTransform, pNavigation);
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


	RELEASE_INSTANCE(CGameInstance);
}

void CBody_Npc_Kid::OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(pPartOwner->Get_Component(TEXT("Com_Transform")));
	CNavigation* pNavigation = dynamic_cast<CLandObject*>(m_pOwner)->Get_CurNaviCom();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::PLAYER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			if (true == m_pOwner->Is_Move())
				pGameInstance->Detrude_Sphere_Collide(_pColObj, m_pColliderCom, m_pParentTransform, pNavigation);
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
	case OBJECT_TYPE::MONSTER:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			if (true == m_pOwner->Is_Move())
				pGameInstance->Detrude_Sphere_Collide(_pColObj, m_pColliderCom, m_pParentTransform, pNavigation);
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

	RELEASE_INSTANCE(CGameInstance);
}

void CBody_Npc_Kid::OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta)
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

HRESULT CBody_Npc_Kid::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	TransformDesc = m_pParentTransform->Get_TransformDesc();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (true == g_EditMode)
	{
		/* Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		/* Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Npc_Kid_Body"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
	{
		/* Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		/* Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Npc_Kid_Body"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereDesc = {};
		SphereDesc.vCenter = _float3(0.f, 1.f, 0.f);
		SphereDesc.fRadius = 0.7f;
		SphereDesc.vCollideColor = _vector(1.f, 0.5f, 0.f, 1.f);
		SphereDesc.vColor = _vector(0.33f, 0.63f, 0.93f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &SphereDesc)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CBody_Npc_Kid::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

CBody_Npc_Kid* CBody_Npc_Kid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CBody_Npc_Kid* pInstance = new CBody_Npc_Kid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CBody_Npc_Kid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Npc_Kid::Clone(void* pArg)
{
	__super::Clone(pArg);

	CBody_Npc_Kid* pInstance = new CBody_Npc_Kid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Npc_Kid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Npc_Kid::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
