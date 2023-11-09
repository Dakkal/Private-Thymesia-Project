#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Attack_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Attack_Urd::CState_Attack_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Attack_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Attack_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (m_pOwnerBodyPart->IsAnimationEnd())
		return STATE::IDLE;


	return eState;
}

STATE CState_Attack_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Attack_Urd::Reset_State()
{
	m_bEnter = false;
}

void CState_Attack_Urd::Enter_State()
{
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 0, 1.2f);
}

STATE CState_Attack_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Attack_Urd* CState_Attack_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Attack_Urd* pInstance = new CState_Attack_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Attack_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack_Urd::Free()
{
	__super::Free();
}
