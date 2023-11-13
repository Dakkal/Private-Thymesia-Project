#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Lockon_Attack.h"
#include "Input_Device.h"
#include "State_Walk.h"

CState_Lockon_Attack::CState_Lockon_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Lockon_Attack::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Lockon_Attack::Tick(const _float& fTimeDelta)
{
	CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
	if (nullptr == pTarget)
		return STATE::IDLE;

	CComponent* pCom = pTarget->Get_Component(TEXT("Com_Transform"));
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(pCom);

	_vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vDir = pTargetTransform->Get_State(CTransform::STATE_POS) - m_pOwnerTransform->Get_State(CTransform::STATE_POS);
	vDir.Normalize();
	vDir.y = 0.f;

	_float fRotMax = 66.f;
	_float fRotNormal = 20.f;

	_vector RotDir;

	if (3.f < acosf(vDir.Dot(vLook)))
	{
		RotDir = _vector::Lerp(vLook, vDir, fRotMax);
	}
	else
	{
		RotDir = _vector::Lerp(vLook, vDir, fRotNormal * fTimeDelta);
	}
	m_pOwnerTransform->Set_Look(RotDir);

	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bIdle && m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_IDLE;
	}
	else if (true == m_bParry && false == m_bIdle && false == m_IsKeepAttack && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(25))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_PARRY;
	}
	else if (true == m_bAvoid && false == m_bIdle && false == m_IsKeepAttack && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(25))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_AVOID;
	}
	else if (true == m_bWalk && false == m_bIdle && false == m_IsKeepAttack && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_WALK;
	}
	else if (true == m_bAttack2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 105, 2.5f);
		m_IsKeepAttack = false;
	}
	else if (true == m_bAttack3 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 106, 2.8f);
		m_IsKeepAttack = false;
	}
	else if (true == m_bAttackRe && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 103, 2.5f, false, 10);
		m_IsKeepAttack = false;
	}
	else
		eState = m_eState;

	RELEASE_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(8))
		m_pRealOwner->Set_Attack(true);
	else if (true == m_bAttack2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(10))
		m_pRealOwner->Set_Attack(true);
	else if (true == m_bAttack3 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(10))
		m_pRealOwner->Set_Attack(true);
	else if (true == m_bAttackRe && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(10))
		m_pRealOwner->Set_Attack(true);


	if (true == m_bAttack1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
		m_pRealOwner->Set_Attack(false);
	else if (true == m_bAttack2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
		m_pRealOwner->Set_Attack(false);
	else if (true == m_bAttack3 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
		m_pRealOwner->Set_Attack(false);
	else if (true == m_bAttackRe && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
		m_pRealOwner->Set_Attack(false);


	return eState;
}

STATE CState_Lockon_Attack::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_bAvoid = true;
		m_bWalk = false;
		m_bIdle = false;
		m_bParry = false;
		m_IsKeepAttack = false;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		m_bAvoid = false;
		m_bWalk = false;
		m_bIdle = false;
		m_bParry = true;
		m_IsKeepAttack = false;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_bAvoid = false;
		m_bWalk = true;
		m_bIdle = false;
		m_bParry = false;
		m_IsKeepAttack = false;
	}
	else
	{
		m_bAvoid = false;
		m_bWalk = false;
		m_bIdle = true;
		m_bParry = false;
		m_IsKeepAttack = false;
	}

	if (true == m_bAttack1 && 103 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80 ||
		true == m_bAttackRe && 103 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bAttack2 = true;
		m_bAttack1 = false;
		m_bAttackRe = false;
		m_IsKeepAttack = true;
	}
	else if (true == m_bAttack2 && 105 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bAttack3 = true;
		m_bAttack2 = false;
		m_IsKeepAttack = true;
	}
	else if (true == m_bAttack3 && 106 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bAttackRe = true;
		m_bAttack3 = false;
		m_IsKeepAttack = true;
	}



	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

void CState_Lockon_Attack::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 103, 2.5f);
	m_bAttack1 = true;
}

void CState_Lockon_Attack::Reset_State()
{
	m_pRealOwner->Set_Attack(false);

	m_bAttack1 = false;
	m_bAttack2 = false;
	m_bAttack3 = false;
	m_bAttackRe = false;
	m_bAvoid = false;
	m_bWalk = false;
	m_bIdle = false;
	m_bParry = false;
	m_IsKeepAttack = false;
}

STATE CState_Lockon_Attack::Key_Input(const _float& fTimeDelta)
{
	return STATE();
}


CState_Lockon_Attack* CState_Lockon_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Lockon_Attack* pInstance = new CState_Lockon_Attack(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Lockon_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Lockon_Attack::Free()
{
	__super::Free();
}
