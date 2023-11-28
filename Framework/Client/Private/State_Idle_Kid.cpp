#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_Kid.h"
#include "Transform.h"

#include "Npc_Kid.h"

CState_Idle_Kid::CState_Idle_Kid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_Kid::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_Kid::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

STATE CState_Idle_Kid::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Idle_Kid::Reset_State()
{
	m_fIdleTime = 0.f;
}

void CState_Idle_Kid::Enter_State()
{
	m_pRealOwner->Set_Move(false);


	m_pOwnerBodyPart->Set_AnimationIndex(true, 9, 1.2f);
}

STATE CState_Idle_Kid::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_Kid* CState_Idle_Kid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_Kid* pInstance = new CState_Idle_Kid(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_Kid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_Kid::Free()
{
	__super::Free();
}
