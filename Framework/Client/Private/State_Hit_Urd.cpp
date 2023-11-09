#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Hit_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Hit_Urd::CState_Hit_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Hit_Urd::Initialize()
{
	__super::Initialize();

	m_bRight_Hit = true;

	return S_OK;
}

STATE CState_Hit_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (m_pRealOwner->Is_Hit())
	{
		if (true == m_bRight_Hit)
		{
			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 15, 1.2f);
			m_bLeft_Hit = true;
		}
		else if (true == m_bLeft_Hit)
		{
			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 16, 1.2f);
			m_bRight_Hit = true;
		}
	}
	else if (m_pOwnerBodyPart->IsAnimationEnd())
		return STATE::IDLE;


	return eState;
}

STATE CState_Hit_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Hit_Urd::Reset_State()
{
	m_bRight_Hit = true;
}

void CState_Hit_Urd::Enter_State()
{
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

	if (true == m_bRight_Hit)
	{
		m_bRight_Hit = false;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 15, 1.2f);
		m_bLeft_Hit = true;
	}
	else if (true == m_bLeft_Hit)
	{
		m_bLeft_Hit = false;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 16, 1.2f);
		m_bRight_Hit = true;
	}
}

STATE CState_Hit_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Hit_Urd* CState_Hit_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Hit_Urd* pInstance = new CState_Hit_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Hit_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Hit_Urd::Free()
{
	__super::Free();
}
