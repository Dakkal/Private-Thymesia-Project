#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_GreatSword.h"
#include "Transform.h"

#include "Enemy_GreatSword.h"

CState_Parry_GreatSword::CState_Parry_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_GreatSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_GreatSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (false == m_bParryEnd && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		m_pRealOwner->Set_Parry(false);

		m_bParryEnd = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 4, 1.2f, false , 15);
		m_pRealOwner->Set_Move(true);
	}

	if (true == m_bParryEnd)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
		{
			
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
		{
			dynamic_cast<CEnemy_GreatSword*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(90))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}
			
	}

	return eState;
}

STATE CState_Parry_GreatSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;


	return eState;
}

void CState_Parry_GreatSword::Reset_State()
{
	m_bParryEnd = false;
}

void CState_Parry_GreatSword::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	dynamic_cast<CEnemy_GreatSword*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pRealOwner->Set_Parry(true);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 29, 1.2f);
}

STATE CState_Parry_GreatSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_GreatSword* CState_Parry_GreatSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_GreatSword* pInstance = new CState_Parry_GreatSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_GreatSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_GreatSword::Free()
{
	__super::Free();
}
