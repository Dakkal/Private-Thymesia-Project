#include "pch.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "PartObject.h"

#include "PlayerCamera.h"

#include "StateMachine.h"
#include "State_Idle.h"
#include "State_Walk.h"
#include "State_Avoid.h"
#include "State_Attack.h"
#include "State_Parry.h"
#include "State_Lockon_Idle.h"
#include "State_Lockon_Walk.h"
#include "State_Lockon_Avoid.h"
#include "State_Lockon_Parry.h"
#include "State_Lockon_Attack.h"
#include "State_Hit.h"
#include "State_ParrySuccess.h"
#include "State_Lockon_ParrySuccess.h"
#include "State_Excute.h"

#include "Collider.h"
#include "Bounding_Sphere.h"

#include "EffectObject.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject(rhs)
{

}

HRESULT CPlayer::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PLAYER;
	m_strObjectName = TEXT("Player");

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Enter_Object()
{
	if (true == m_bFirstDrop)
	{
		m_pCurNavigationCom->Set_toCell(15, m_pTransformCom);
		m_bFirstDrop = false;
	}
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (true == g_BossSeq)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (true == pGameInstance->Key_Down('Q'))
		ReSearch_TargetEnemy();

	if (true == pGameInstance->Key_Down('T'))
	{
		CEffectObject::EFFECT_DESC	effectdesc;
		effectdesc.vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POS);

		pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_EFFECT, TEXT("Prototype_GameObject_ParrySpark"), &effectdesc);
	}


	RELEASE_INSTANCE(CGameInstance);

	m_pStateMachineCom->Tick(fTimeDelta);

	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->Tick(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (true == g_BossSeq)
		return;

	if (true == m_IsHit) m_IsHit = false;

	Check_TargetEnemy();

	m_pStateMachineCom->LateTick(fTimeDelta);

	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->LateTick(fTimeDelta);
	}

	m_pColliderCom->LateUpdate();


#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);

	if (nullptr != m_pCurNavigationCom)
		m_pRendererCom->Add_Debug(m_pCurNavigationCom);
#endif
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

void CPlayer::OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
	{
		CGameObject* pSearchTarget = nullptr;
		for (auto& pTarget : m_vecTargetEnemy)
		{
			if (_pColObj == pTarget)
				pSearchTarget = pTarget;
		}
		if(nullptr == pSearchTarget)
			m_vecTargetEnemy.push_back(_pColObj);
	}
		break;
	case OBJECT_TYPE::BOSS:
	{
		CGameObject* pSearchTarget = nullptr;
		for (auto& pTarget : m_vecTargetEnemy)
		{
			if (_pColObj == pTarget)
				pSearchTarget = pTarget;
		}
		if (nullptr == pSearchTarget)
			m_vecTargetEnemy.push_back(_pColObj);
	}
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Enter(_pColObj, fTimeDelta);
		break;
	}

}

void CPlayer::OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
		break;
	case OBJECT_TYPE::BOSS:
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Stay(_pColObj, fTimeDelta);
		break;
	}
}

void CPlayer::OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	OBJECT_TYPE eObject = _pColObj->Get_ObjectType();

	switch (eObject)
	{
	case OBJECT_TYPE::PORP:
		break;
	case OBJECT_TYPE::MONSTER:
		for (auto iter = m_vecTargetEnemy.begin(); iter != m_vecTargetEnemy.end(); ++iter)
		{
			if (_pColObj == *iter)
			{
				m_vecTargetEnemy.erase(iter);
				break;
			}
		}
		break;
	case OBJECT_TYPE::BOSS:
		for (auto iter = m_vecTargetEnemy.begin(); iter != m_vecTargetEnemy.end(); ++iter)
		{
			if (_pColObj == *iter)
			{
				m_vecTargetEnemy.erase(iter);
				break;
			}
		}
		break;
	case OBJECT_TYPE::PART:
		OnCollision_Part_Exit(_pColObj, fTimeDelta);
		break;
	default:
		break;
	}
}

void CPlayer::OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Reset_TargetEnemy()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTargetEnemy = nullptr;
	CPlayerCamera* pPlayerCamera = dynamic_cast<CPlayerCamera*>(pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_CAMERA));
	if(nullptr != pPlayerCamera)
		pPlayerCamera->Set_TargetTransform(nullptr);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPlayer::Search_TargetEnemy()
{
	if (nullptr != m_pTargetEnemy)
		return S_OK;

	if (0 >= m_vecTargetEnemy.size())
	{
		m_pTargetEnemy = nullptr;
		return S_OK;
	}
		

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float fCloseDist = 30.f;
	for (auto& pTarget : m_vecTargetEnemy)
	{
		CTransform* pTargetTransform = dynamic_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
		_vector TargetPos = pTargetTransform->Get_State(CTransform::STATE_POS);
		_vector PlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POS);
		_float fDist = _vector(TargetPos - PlayerPos).Length();

		if (fCloseDist >= fDist)
		{
			fCloseDist = fDist;
			m_pTargetEnemy = pTarget;
		}

	}

	if (nullptr == m_pTargetEnemy)
		return S_OK;

	CPlayerCamera* pPlayerCamera = dynamic_cast<CPlayerCamera*>(pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_CAMERA));
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pTargetEnemy->Get_Component(TEXT("Com_Transform")));
	pPlayerCamera->Set_TargetTransform(pTargetTransform);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::ReSearch_TargetEnemy()
{
	if (nullptr == m_pTargetEnemy || 1 >= m_vecTargetEnemy.size())
		return S_OK;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float fCloseDist = 30.f;
	for (auto& pTarget : m_vecTargetEnemy)
	{
		if (m_pTargetEnemy == pTarget)
			continue;

		CTransform* pTargetTransform = dynamic_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
		_vector TargetPos = pTargetTransform->Get_State(CTransform::STATE_POS);
		_vector PlayerPos = pTargetTransform->Get_State(CTransform::STATE_POS);
		_float fDist = _vector(TargetPos - PlayerPos).Length();

		if (fCloseDist > fDist)
		{
			fCloseDist = fDist;
			m_pTargetEnemy = pTarget;
		}

	}

	CPlayerCamera* pPlayerCamera = dynamic_cast<CPlayerCamera*>(pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_CAMERA));
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pTargetEnemy->Get_Component(TEXT("Com_Transform")));
	pPlayerCamera->Set_TargetTransform(pTargetTransform);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::Check_TargetEnemy()
{
	if (nullptr != m_pTargetEnemy && true == m_pTargetEnemy->Is_Dead())
		m_pTargetEnemy = nullptr;
	
	if (0 < m_vecTargetEnemy.size())
	{
		auto iter = m_vecTargetEnemy.begin();
		while (iter != m_vecTargetEnemy.end())
		{
			if (true == (*iter)->Is_Dead())
			{
				iter = m_vecTargetEnemy.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 3.5f;
	TransformDesc.fRotRadianPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation* pNavigation = { nullptr };
	CNavigation::NAVI_DESC		NaviDesc;
	NaviDesc.strNavi = "BossRoom";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BossRoom_Navigation"),
		TEXT("Com_Navigation1"), (CComponent**)&pNavigation, &NaviDesc)))
		return E_FAIL;
	m_NavigationComs.push_back(pNavigation);

	NaviDesc.strNavi = "SubBuilding";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_SubBuilding_Navigation"),
		TEXT("Com_Navigation2"), (CComponent**)&pNavigation, &NaviDesc)))
		return E_FAIL;
	m_NavigationComs.push_back(pNavigation);

	NaviDesc.strNavi = "Stair";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Stair_Navigation"),
		TEXT("Com_Navigation4"), (CComponent**)&pNavigation, &NaviDesc)))
		return E_FAIL;
	m_NavigationComs.push_back(pNavigation);

	NaviDesc.strNavi = "Church";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Church_Navigation"),
		TEXT("Com_Navigation5"), (CComponent**)&pNavigation, &NaviDesc)))
		return E_FAIL;
	m_NavigationComs.push_back(pNavigation);

	m_pCurNavigationCom = pNavigation;

	/* Com_Collider_Sphere */
	CBounding_Frustrum::BOUNDING_FRUSTRUM_DESC	FrustrumDesc = {};
	FrustrumDesc.vCenter = _float3(0.f, 1.f, 0.f);
	FrustrumDesc.vDegree = _float3(0.f, 0.f, 0.f);
	FrustrumDesc.fLeftSlope = -0.8f;
	FrustrumDesc.fRightSlope = 0.8f;
	FrustrumDesc.fTopSlope = 0.8f;
	FrustrumDesc.fBottomSlope = -0.8f;
	FrustrumDesc.fNear = 0.1f;
	FrustrumDesc.fFar = 30.f;
	FrustrumDesc.vCollideColor = _vector(1.f, 0.f, 0.f, 1.f);
	FrustrumDesc.vColor = _vector(1.f, 1.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Frustrum"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &FrustrumDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayerParts = nullptr;

	/* For.Part_Body */
	CPartObject::PART_DESC			PartDesc_Body;
	PartDesc_Body.pOwner = this;
	PartDesc_Body.ePart = PARTS::BODY;
	PartDesc_Body.pParentTransform = m_pTransformCom;

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Body"), &PartDesc_Body);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::BODY, pPlayerParts);

	/* For.Part_Weapon */
	CPartObject::PART_DESC			PartDesc_Weapon_Saber;
	PartDesc_Weapon_Saber.pOwner = this;
	PartDesc_Weapon_Saber.ePart = PARTS::WEAPON_R;
	PartDesc_Weapon_Saber.pParentTransform = m_pTransformCom;
	PartDesc_Weapon_Saber.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("weapon_r");
	PartDesc_Weapon_Saber.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketPivotMatrix();

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Weapon_Saber"), &PartDesc_Weapon_Saber);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::WEAPON_R, pPlayerParts);

	CPartObject::PART_DESC			PartDesc_Weapon_Dagger;
	PartDesc_Weapon_Dagger.pOwner = this;
	PartDesc_Weapon_Dagger.ePart = PARTS::WEAPON_L;
	PartDesc_Weapon_Dagger.pParentTransform = m_pTransformCom;
	PartDesc_Weapon_Dagger.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("weapon_l");
	PartDesc_Weapon_Dagger.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketPivotMatrix();

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Weapon_Dagger"), &PartDesc_Weapon_Dagger);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::WEAPON_L, pPlayerParts);

	CPartObject::PART_DESC			PartDesc_HitBox;
	PartDesc_HitBox.pOwner = this;
	PartDesc_HitBox.ePart = PARTS::HITBOX;
	PartDesc_HitBox.pParentTransform = m_pTransformCom;

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_HitBox"), &PartDesc_HitBox);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::HITBOX, pPlayerParts);


	CPartObject::PART_DESC			PartDesc_Camera;
	PartDesc_Camera.pOwner = this;
	PartDesc_Camera.ePart = PARTS::CAMERA;
	PartDesc_Camera.pParentTransform = m_pTransformCom;
	PartDesc_Camera.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("camera");
	PartDesc_Camera.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketPivotMatrix();
	PartDesc_Camera.pSocketBoneforPivot = dynamic_cast<CPartObject*>(m_Parts[PARTS::BODY])->Get_SocketBonePtr("AnimTargetPoint");

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_SeqCamera"), &PartDesc_Camera);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_Parts.emplace(CGameObject::PARTS::CAMERA, pPlayerParts);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::Ready_State()
{
	CState* pState = CState_Walk::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::WALK);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Idle::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::IDLE);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Avoid::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::AVOID);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Attack::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::ATTACK);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Parry::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::PARRY);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Lockon_Idle::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::LOCK_IDLE);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Lockon_Walk::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::LOCK_WALK);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Lockon_Avoid::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::LOCK_AVOID);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Lockon_Parry::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::LOCK_PARRY);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Lockon_Attack::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::LOCK_ATTACK);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Hit::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::HIT);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Lockon_ParrySuccess::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::LOCK_PARRY_SUCCESS);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_ParrySuccess::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::PARRY_SUCCESS);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);

	pState = CState_Excute::Create(m_pDevice, m_pContext, m_pStateMachineCom, STATE::SEQUENCE);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);


	m_pStateMachineCom->Set_State(STATE::IDLE);
	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	__super::Clone(pArg);

	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& iter : m_Parts)
		Safe_Release(iter.second);
	m_Parts.clear();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pStateMachineCom);
}