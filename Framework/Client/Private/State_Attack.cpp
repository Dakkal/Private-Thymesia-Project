#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Attack.h"
#include "Input_Device.h"
#include "State_Walk.h"

CState_Attack::CState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Attack::Initialize()
{
	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pStateOwner->Get_Owner()->Get_Component(TEXT("Com_Transform")));
	CGameObject* pOwnerBodyPart = dynamic_cast<CPlayer*>(m_pStateOwner->Get_Owner())->Get_Parts(CPlayer::PARTS::BODY);
	m_pOwnerBodyPart = dynamic_cast<CPartObject*>(pOwnerBodyPart);

	return S_OK;
}

STATE CState_Attack::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (true == m_bIdle  && m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}
	else if (true == m_bParry && false == m_bIdle && false == m_IsKeepAttack && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::PARRY;
	}
	else if (true == m_bAvoid && false == m_bIdle && false == m_IsKeepAttack && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::AVOID;
	}
	else if (true == m_bWalk && false == m_bIdle && false == m_IsKeepAttack &&  true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}
	else if (true == m_bAttack2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 105, 2.f);
		m_IsKeepAttack = false;
	}
	else if (true == m_bAttack3 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 106, 2.f);
		m_IsKeepAttack = false;
	}
	else if (true == m_bAttackRe && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 103, 2.f, 10);
		m_IsKeepAttack = false;
	}
	else
		eState = m_eState;

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Attack::LateTick(const _float& fTimeDelta)
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

void CState_Attack::Enter_State()
{
	m_pOwnerBodyPart->Set_AnimationIndex(false, 103, 2.f);
	m_bAttack1 = true;
}

void CState_Attack::Reset_State()
{
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

STATE CState_Attack::Key_Input(const _float& fTimeDelta)
{
	return STATE();
}


CState_Attack* CState_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Attack* pInstance = new CState_Attack(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack::Free()
{
	__super::Free();
}
