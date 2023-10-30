#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry.h"
#include "Input_Device.h"
#include "State_Walk.h"

CState_Parry::CState_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry::Initialize()
{
	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pStateOwner->Get_Owner()->Get_Component(TEXT("Com_Transform")));
	CGameObject* pOwnerBodyPart = dynamic_cast<CPlayer*>(m_pStateOwner->Get_Owner())->Get_Parts(CPlayer::PARTS::BODY);
	m_pOwnerBodyPart = dynamic_cast<CPartObject*>(pOwnerBodyPart);

	return S_OK;
}

STATE CState_Parry::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bIdle && m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}
	else if (true == m_bAttack && false == m_bIdle && false == m_IsKeepParry && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::ATTACK;
	}
	else if (true == m_bAvoid && false == m_bIdle && false == m_IsKeepParry && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::AVOID;
	}
	else if (true == m_bWalk && false == m_bIdle && false == m_IsKeepParry && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}
	else if (true == m_bParry2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 45, 3.f);
		m_IsKeepParry = false;
	}
	else if (true == m_bParryRe && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 44, 3.f, 10);
		m_IsKeepParry = false;
	}
	else
		eState = m_eState;

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Parry::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bAttack = true;
		m_bAvoid = false;
		m_bWalk = false;
		m_bIdle = false;
		m_IsKeepParry = false;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_bAttack = false;
		m_bAvoid = true;
		m_bWalk = false;
		m_bIdle = false;
		m_IsKeepParry = false;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_bAttack = false;
		m_bAvoid = false;
		m_bWalk = true;
		m_bIdle = false;
		m_IsKeepParry = false;
	}
	else
	{
		m_bAttack = false;
		m_bAvoid = false;
		m_bWalk = false;
		m_bIdle = true;
		m_IsKeepParry = false;
	}

	if (true == m_bParry1 && 44 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIKeyState(DIK_F) & 0x80 ||
		true == m_bParryRe && 44 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		m_bParry2 = true;
		m_bParry1 = false;
		m_bParryRe = false;
		m_IsKeepParry = true;
	}
	else if (true == m_bParry2 && 45 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		m_bParry2 = false;
		m_bParryRe = true;
		m_IsKeepParry = true;
	}



	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

void CState_Parry::Enter_State()
{
	m_pOwnerBodyPart->Set_AnimationIndex(false, 44, 3.f);
	m_bParry1 = true;
}

void CState_Parry::Reset_State()
{
	m_bAttack = false;
	m_bAvoid = false;
	m_bWalk = false;
	m_bIdle = true;
	m_IsKeepParry = false;
	m_bParry1 = false;
	m_bParry2 = false;
	m_bParryRe = false;
}

STATE CState_Parry::Key_Input(const _float& fTimeDelta)
{
	return STATE();
}


CState_Parry* CState_Parry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry* pInstance = new CState_Parry(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry::Free()
{
	__super::Free();
}
