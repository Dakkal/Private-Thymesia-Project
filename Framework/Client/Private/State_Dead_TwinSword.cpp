#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Dead_TwinSword.h"
#include "Transform.h"

#include "Enemy_TwinSword.h"

CState_Dead_TwinSword::CState_Dead_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Dead_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Dead_TwinSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pOwnerBodyPart->IsAnimationEnd())
		m_pRealOwner->Set_Dead(true);

	return eState;
}

STATE CState_Dead_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Dead_TwinSword::Reset_State()
{
	m_bEnter = false;
}

void CState_Dead_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	
	dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 27, 1.f);
}

STATE CState_Dead_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Dead_TwinSword* CState_Dead_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Dead_TwinSword* pInstance = new CState_Dead_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Dead_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Dead_TwinSword::Free()
{
	__super::Free();
}
