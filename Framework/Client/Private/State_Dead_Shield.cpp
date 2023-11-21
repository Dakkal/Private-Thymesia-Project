#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Dead_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"

CState_Dead_Shield::CState_Dead_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Dead_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Dead_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pOwnerBodyPart->IsAnimationEnd())
		m_pRealOwner->Set_Dead(true);

	return eState;
}

STATE CState_Dead_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Dead_Shield::Reset_State()
{
	m_bEnter = false;
}

void CState_Dead_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	
	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 23, 1.2f);
}

STATE CState_Dead_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Dead_Shield* CState_Dead_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Dead_Shield* pInstance = new CState_Dead_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Dead_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Dead_Shield::Free()
{
	__super::Free();
}
