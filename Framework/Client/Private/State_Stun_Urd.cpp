#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Stun_Urd.h"
#include "Transform.h"
#include "LandObject.h"


#include "Boss_Urd.h"

CState_Stun_Urd::CState_Stun_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Stun_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Stun_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	
	if (true == m_bStuned)
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_Excute(true);

		if (true == m_pRealOwner->Is_Hit())
			return STATE::DEAD;
	}

	if (false == m_bStuned && m_pOwnerBodyPart->IsAnimationEnd())
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 39, 1.2f);

		m_bStuned = true;
	}

	return eState;
}

STATE CState_Stun_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Stun_Urd::Reset_State()
{
	m_bStuned = false;
}

void CState_Stun_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 40, 0.9f);
}

STATE CState_Stun_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Stun_Urd* CState_Stun_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Stun_Urd* pInstance = new CState_Stun_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Stun_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Stun_Urd::Free()
{
	__super::Free();
}
