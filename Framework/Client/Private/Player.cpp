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

#include "Collider.h"
#include "Bounding_Sphere.h"

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

void CPlayer::Tick(_float fTimeDelta)
{
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
	m_pStateMachineCom->LateTick(fTimeDelta);

	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->LateTick(fTimeDelta);
	}

	m_pColliderCom->LateUpdate();
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Render();

	m_pColliderCom->Render();
#endif // DEBUG

	return S_OK;
}

void CPlayer::OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta)
{
	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
		{
			CCollider* pCollider = dynamic_cast<CCollider*>(iter.second->Get_Component(TEXT("Com_Collider")));
			pCollider->Set_Active(true);
		}
	}
}

void CPlayer::OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta)
{
}

void CPlayer::OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta)
{
	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
		{
			CCollider* pCollider = dynamic_cast<CCollider*>(iter.second->Get_Component(TEXT("Com_Collider")));
			pCollider->Set_Active(false);
		}
	}
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
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	auto pList = pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, LAYER_BOSS);

	_float fCloseDist = 30.f;

	for (auto& pObj : *pList)
	{
		CTransform* pEnemyTransform = dynamic_cast<CTransform*>(pObj->Get_Component(TEXT("Com_Transform")));

		_vector vTargetPos = pEnemyTransform->Get_State(CTransform::STATE_POS);
		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POS);

		_float fDist = _vector(vTargetPos - vPlayerPos).Length();

		if (fDist < fCloseDist)
		{
			fCloseDist = fDist;
			m_pTargetEnemy = pObj;
		}
	}

	if (nullptr != m_pTargetEnemy)
	{
		CPlayerCamera* pPlayerCamera = dynamic_cast<CPlayerCamera*>(pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_CAMERA));
		CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pTargetEnemy->Get_Component(TEXT("Com_Transform")));
		pPlayerCamera->Set_TargetTransform(pTargetTransform);
	}
	
	RELEASE_INSTANCE(CGameInstance);

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
	TransformDesc.fSpeedPerSec = 4.f;
	TransformDesc.fRotRadianPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation* pNavigation = { nullptr };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Church_Navigation"),
		TEXT("Com_Navigation1"), (CComponent**)&pNavigation)))
		return E_FAIL;
	m_NavigationComs.push_back(pNavigation);

	m_pCurNavigationCom = pNavigation;
	m_pCurNavigationCom->Set_toCell(3, m_pTransformCom);

	/* Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	SphereDesc = {};
	SphereDesc.fRadius = 3.f;
	SphereDesc.vCenter = _float3(0.f, 0.01f, 0.f);
	SphereDesc.vCollideColor = _vector(1.f, 0.f, 0.f, 1.f);
	SphereDesc.vColor = _vector(1.f, 1.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &SphereDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Active(true);

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