#include "pch.h"
#include "..\Public\Boss_Urd.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "StateMachine.h"
#include "State_Idle_Urd.h"
#include "State_Hit_Urd.h"
#include "State_Avoid_Urd.h"
#include "State_Attack_Urd.h"
#include "State_Seq_Urd.h"
#include "State_Walk_Urd.h"
#include "State_Parry_Urd.h"
#include "State_Run_Urd.h"
#include "State_Stun_Urd.h"
#include "State_Dead_Urd.h"
#include "State_Skill_Urd.h"

#include "Collider.h"
#include "Bounding_Sphere.h"

CBoss_Urd::CBoss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{

}

CBoss_Urd::CBoss_Urd(const CBoss_Urd& rhs)
	: CLandObject(rhs)
{

}

HRESULT CBoss_Urd::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::BOSS;
	m_strObjectName = TEXT("Boss_Urd");

	return S_OK;
}

HRESULT CBoss_Urd::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_fDissolveDuraton = 4.f;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iRandom = pGameInstance->Random_Int(2, 3);

	m_iSkillActive = iRandom;

	RELEASE_INSTANCE(CGameInstance);
		

	return S_OK;
}

void CBoss_Urd::Enter_Object()
{
	if (true == m_bFirstDrop)
	{
		m_pCurNavigationCom->Set_toCell(17, m_pTransformCom);
		m_bFirstDrop = false;
	}
}

void CBoss_Urd::Tick(_float fTimeDelta)
{
	
	if (false == m_IsSeq)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		auto pObj = pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_PLAYER);
		if (nullptr == pObj)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		CNavigation* pNavi = dynamic_cast<CLandObject*>(pObj)->Get_CurNaviCom();
		if ("BossRoom" == pNavi->Get_NaviName())
		{
			m_IsSeq = true;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	Out_Player(fTimeDelta);
	Look_Player(fTimeDelta);
	
	m_pStateMachineCom->Tick(fTimeDelta);

	if (true == m_IsDead)
	{
		if (false == m_bDissolveSound)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->PlaySoundFile(TEXT("SlowMotion_01.ogg"), CHANNELID::CHANNEL_13, 0.25f);

			RELEASE_INSTANCE(CGameInstance);

			m_bDissolveSound = true;
		}

		m_fDissolveTime += fTimeDelta;

		if (m_fDissolveDuraton < m_fDissolveTime)
			m_IsActive = false;
	}
	/* 작동이 멈추면 리턴, 데드 스테이트 틱에서 꺼지니까 여기가 맞을거다 */
	if (false == m_IsActive)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->Tick(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	RELEASE_INSTANCE(CGameInstance)
}

void CBoss_Urd::LateTick(_float fTimeDelta)
{
	if (false == m_IsSeq)
		return;

	if (false == m_IsActive)
		return;

	if (true == m_IsHit) m_IsHit = false;



	m_pStateMachineCom->LateTick(fTimeDelta);

	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->LateTick(fTimeDelta);
	}


	if (3 < m_listProjectile.size())
	{
		CThrow_Weapon_Urd* pObject = m_listProjectile.front();
		pObject->Set_Active(false);
		m_listProjectile.pop_front();
	}


	m_pColliderCom->LateUpdate();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->IsIn_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POS), 2.f))
	{
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);

		if (nullptr != m_pCurNavigationCom)
			m_pRendererCom->Add_Debug(m_pCurNavigationCom);
#endif
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);

		m_IsCull = true;
	}
	else
		m_IsCull = false;

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBoss_Urd::Render()
{
	return S_OK;
}

void CBoss_Urd::OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PLAYER:
		Set_PlayerTransform();
		m_bIsOutPlayer = false;
		m_fReleaseTimeAcc = 0.f;
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

void CBoss_Urd::OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta)
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
		/* For. Parts */
	case OBJECT_TYPE::PART:
		OnCollision_Part_Stay(_pColObj, fTimeDelta);
		break;
	}
}

void CBoss_Urd::OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PLAYER:
		m_bIsOutPlayer = true;
		break;
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
		break;
		/* For. Parts */
	case OBJECT_TYPE::PART:
		OnCollision_Part_Exit(_pColObj, fTimeDelta);
		break;
	}
}

void CBoss_Urd::OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta)
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
			m_bIsLookPlayer = true;
			break;
		case Engine::CGameObject::WEAPON_R:
			break;
		case Engine::CGameObject::WEAPON_L:
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
		default:
			break;
		}
	}
	break;
	}
}

void CBoss_Urd::OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta)
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
		default:
			break;
		}
	}
	break;
	}
}

void CBoss_Urd::OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta)
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
		default:
			break;
		}
	}
	break;
	}
}

_float CBoss_Urd::Get_PlayerDistance()
{
	if (nullptr == m_pPlayerTransform)
		return -1.f;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POS);

	_float fLength = _vector(vPlayerPos - vPos).Length();

	return fLength;
}

void CBoss_Urd::Out_Player(_float fTimeDelta)
{
	if (false == m_bIsOutPlayer)
		return;

	m_fReleaseTimeAcc += fTimeDelta;
	if (5.f <= m_fReleaseTimeAcc)
	{
		m_pPlayerTransform = nullptr;
		m_fReleaseTimeAcc = 0.f;
		m_bIsOutPlayer = false;
		m_bIsLookPlayer = false;
	}
}

void CBoss_Urd::Look_Player(_float fTimeDelta)
{
	if (nullptr == m_pPlayerTransform || false == m_bIsLookPlayer)
		return;

	_vector vDir = m_pPlayerTransform->Get_State(CTransform::STATE_POS) - m_pTransformCom->Get_State(CTransform::CTransform::STATE_POS);
	_float fDist = vDir.Length();

	if (50 >= fDist)
	{
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vDir.Normalize();
		vDir.y = 0.f;

		_float fRotMax = 20.f;
		_float fRotNormal = 15.f;

		_vector RotDir;

		if (3.f < acosf(vDir.Dot(vLook)))
		{
			RotDir = _vector::Lerp(vLook, vDir, fRotMax * fTimeDelta);
		}
		else
		{
			RotDir = _vector::Lerp(vLook, vDir, fRotNormal * fTimeDelta);
		}
		m_pTransformCom->Set_Look(RotDir);
	}

}

void CBoss_Urd::Set_PlayerTransform()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	auto pObj = pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_PLAYER);
	if (nullptr == pObj)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_pPlayerTransform = dynamic_cast<CTransform*>(pObj->Get_Component(TEXT("Com_Transform")));

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CBoss_Urd::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotRadianPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_StateMachine*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BossRoom_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
		return E_FAIL;


	/* Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	SphereDesc = {};
	SphereDesc.fRadius = 5.f;
	SphereDesc.vCenter = _float3(0.f, 0.01f, 0.f);
	SphereDesc.vCollideColor = _vector(1.f, 0.f, 0.f, 1.f);
	SphereDesc.vColor = _vector(1.f, 1.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Urd::Ready_Parts()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pParts = nullptr;

	/* For.Part_Body */
	CPartObject::PART_DESC			PartDesc_Body;
	PartDesc_Body.pOwner = this;
	PartDesc_Body.ePart = PARTS::BODY;
	PartDesc_Body.pParentTransform = m_pTransformCom;

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_Body"), &PartDesc_Body);
	if (nullptr == pParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::BODY, pParts);

	/* For.Part_Weapon */
	CPartObject::PART_DESC			PartDesc_Weapon;
	PartDesc_Weapon.pOwner = this;
	PartDesc_Weapon.ePart = PARTS::WEAPON_R;
	PartDesc_Weapon.pParentTransform = m_pTransformCom;
	PartDesc_Weapon.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("weapon_r");
	PartDesc_Weapon.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketPivotMatrix();
	PartDesc_Weapon.pSocketBoneforPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("AnimTargetPoint");

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_Weapon"), &PartDesc_Weapon);
	if (nullptr == pParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::WEAPON_R, pParts);

	/* For.Part_WeaponGara */
	CPartObject::PART_DESC			PartDesc_WeaponGara;
	PartDesc_WeaponGara.pOwner = this;
	PartDesc_WeaponGara.ePart = PARTS::WEAPON_L;
	PartDesc_WeaponGara.pParentTransform = m_pTransformCom;
	PartDesc_WeaponGara.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("weapon_l");
	PartDesc_WeaponGara.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketPivotMatrix();

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_WeaponGara"), &PartDesc_WeaponGara);
	if (nullptr == pParts)
		return E_FAIL;
	pParts->Set_Active(false);
	m_Parts.emplace(CGameObject::PARTS::WEAPON_L, pParts);

	/* For.HitBox */
	CPartObject::PART_DESC			PartDesc_HitBox;
	PartDesc_HitBox.pOwner = this;
	PartDesc_HitBox.ePart = PARTS::HITBOX;
	PartDesc_HitBox.pParentTransform = m_pTransformCom;

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_HitBox"), &PartDesc_HitBox);
	if (nullptr == pParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::HITBOX, pParts);

	CPartObject::PART_DESC			PartDesc_Camera;
	PartDesc_Camera.pOwner = this;
	PartDesc_Camera.ePart = PARTS::CAMERA;
	PartDesc_Camera.pParentTransform = m_pTransformCom;
	PartDesc_Camera.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("camera");
	PartDesc_Camera.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketPivotMatrix();
	PartDesc_Camera.pSocketBoneforPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("AnimTargetPoint");

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_SeqCamera"), &PartDesc_Camera);
	if (nullptr == pParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::CAMERA, pParts);
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBoss_Urd::Ready_State()
{
	CState* pState = CState_Idle_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::IDLE);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::IDLE, pState);

	pState = CState_Hit_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::HIT);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::HIT, pState);

	pState = CState_Avoid_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::AVOID);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::AVOID, pState);

	pState = CState_Attack_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::ATTACK);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::ATTACK, pState);

	pState = CState_Parry_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::PARRY);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::PARRY, pState);

	pState = CState_Run_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::RUN);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::RUN, pState);

	pState = CState_Walk_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::WALK);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::WALK, pState);

	pState = CState_Seq_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::SEQUENCE);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::SEQUENCE, pState);

	pState = CState_Stun_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::STUN);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::STUN, pState);

	pState = CState_Dead_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::DEAD);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::DEAD, pState);

	pState = CState_Skill_Urd::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::SKILL);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(STATE::SKILL, pState);


	m_pStateMachineCom->Set_State(STATE::SEQUENCE);

	return S_OK;
}

CBoss_Urd* CBoss_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CBoss_Urd* pInstance = new CBoss_Urd(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CBoss_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Urd::Clone(void* pArg)
{
	__super::Clone(pArg);

	CBoss_Urd* pInstance = new CBoss_Urd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Urd::Free()
{
	__super::Free();

	m_listProjectile.clear();

	for (auto& iter : m_Parts)
		Safe_Release(iter.second);
	m_Parts.clear();

	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pStateMachineCom);
}