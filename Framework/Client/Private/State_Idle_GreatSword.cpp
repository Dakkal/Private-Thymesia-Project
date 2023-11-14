#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_GreatSword.h"
#include "Transform.h"

#include "Enemy_GreatSword.h"

CState_Idle_GreatSword::CState_Idle_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_GreatSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_GreatSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;


	return eState;
}

STATE CState_Idle_GreatSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Idle_GreatSword::Reset_State()
{
	m_bEnter = false;
}

void CState_Idle_GreatSword::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CEnemy_GreatSword*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 26, 1.2f);
}

STATE CState_Idle_GreatSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_GreatSword* CState_Idle_GreatSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_GreatSword* pInstance = new CState_Idle_GreatSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_GreatSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_GreatSword::Free()
{
	__super::Free();
}
