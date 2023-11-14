#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"

CState_Idle_Halberd::CState_Idle_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_Halberd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;


	return eState;
}

STATE CState_Idle_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Idle_Halberd::Reset_State()
{
	m_bEnter = false;
}

void CState_Idle_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 51, 1.2f);
}

STATE CState_Idle_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_Halberd* CState_Idle_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_Halberd* pInstance = new CState_Idle_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_Halberd::Free()
{
	__super::Free();
}
