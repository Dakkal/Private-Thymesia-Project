#include "pch.h"
#include "HitBox_Player.h"

#include "GameInstance.h"
#include "BinBone.h"
#include "Bounding_AABB.h"
#include "Collider.h"

CHitBox_Player::CHitBox_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CHitBox_Player::CHitBox_Player(const CHitBox_Player& rhs)
	: CPartObject(rhs)
{

}

HRESULT CHitBox_Player::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PART;
	m_strObjectName = TEXT("Player_HitBox");

	return S_OK;
}

HRESULT CHitBox_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 부모 소켓행렬을 기준으로 자식의 상태를 제어한다.  */
	//m_pTransformCom->Set_State(CTransform::STATE_POS, _vector(0.f, 1.5f, 0.f, 1.f));

	return S_OK;
}

void CHitBox_Player::Tick(_float fTimeDelta)
{
	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix());

	/* 계산한뒤에 월드를 업데이트 */
	m_pColliderCom->Update(m_WorldMatrix);
}

void CHitBox_Player::LateTick(_float fTimeDelta)
{
	m_pColliderCom->LateUpdate();

#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
}

HRESULT CHitBox_Player::Render()
{

	return S_OK;
}

void CHitBox_Player::OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::BOSS:
		break;
	case OBJECT_TYPE::PROJECTILE:
		m_pOwner->Set_Hit(true);
		break;
	case OBJECT_TYPE::MONSTER:
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Enter(_pColObj, fTimeDelta);
		break;
	}
}

void CHitBox_Player::OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::BOSS:
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

void CHitBox_Player::OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::BOSS:
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

void CHitBox_Player::OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::BOSS:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			if (true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);

				m_bJustHitOne = true;
			}
			break;
		case Engine::CGameObject::WEAPON_L:
			if (true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);

				m_bJustHitOneSub = true;
			}
			break;
		case Engine::CGameObject::HITBOX:
			break;

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
			if (true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);

				m_bJustHitOne = true;
			}
			break;
		case Engine::CGameObject::WEAPON_L:
			if (true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);

				m_bJustHitOneSub = true;
			}
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

void CHitBox_Player::OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::BOSS:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			if (true == m_bJustHitOne && false == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_bJustHitOne = false;
			}
			if (false == m_bJustHitOne && true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);
				m_bJustHitOne = true;
			}
			break;
		case Engine::CGameObject::WEAPON_L:
			if (true == m_bJustHitOneSub && false == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_bJustHitOneSub = false;
			}
			if (false == m_bJustHitOneSub && true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);
				m_bJustHitOneSub = true;
			}
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
			break;
		case Engine::CGameObject::WEAPON_R:
			if (true == m_bJustHitOne && false == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_bJustHitOne = false;
			}
			if (false == m_bJustHitOne && true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);
				m_bJustHitOne = true;
			}
			break;
		case Engine::CGameObject::WEAPON_L:
			if (true == m_bJustHitOneSub && false == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_bJustHitOneSub = false;
			}
			if (false == m_bJustHitOneSub && true == pPartOwner->Is_Attack() && false == m_pOwner->Is_Hit())
			{
				m_pOwner->Set_Hit(true);
				m_bJustHitOneSub = true;
			}
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

void CHitBox_Player::OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameObject* pPartOwner = dynamic_cast<CPartObject*>(_pColObj)->Get_PartOwner();
	OBJECT_TYPE eOwnerType = pPartOwner->Get_ObjectType();
	CGameObject::PARTS ePart = dynamic_cast<CPartObject*>(_pColObj)->Get_Part_Index();

	switch (eOwnerType)
	{
	case OBJECT_TYPE::BOSS:
	{
		switch (ePart)
		{
		case Engine::CGameObject::BODY:
			break;
		case Engine::CGameObject::WEAPON_R:
			m_bJustHitOne = false;
			break;
		case Engine::CGameObject::WEAPON_L:
			m_bJustHitOneSub = false;
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
			m_bJustHitOne = false;
			break;
		case Engine::CGameObject::WEAPON_L:
			m_bJustHitOneSub = false;
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

HRESULT CHitBox_Player::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc = {};
	AABBDesc.vExtents = _float3(0.4f, 0.85f, 0.4f);
	AABBDesc.vCenter = _float3(0.0f, AABBDesc.vExtents.y + 0.01f, 0.f);
	AABBDesc.vCollideColor = _vector(1.f, 0.5f, 0.f, 1.f);
	AABBDesc.vColor = _vector(0.33f, 0.63f, 0.93f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitBox_Player::Bind_ShaderResources()
{


	return S_OK;
}

CHitBox_Player* CHitBox_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CHitBox_Player* pInstance = new CHitBox_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CHitBox_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHitBox_Player::Clone(void* pArg)
{
	__super::Clone(pArg);

	CHitBox_Player* pInstance = new CHitBox_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHitBox_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHitBox_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
