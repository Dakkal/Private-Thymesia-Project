#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"

CState_Parry_Shield::CState_Parry_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(65))
		{
			m_pRealOwner->Set_Parry(false);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(80))
		{
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(140))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryRight1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
		{
			m_pRealOwner->Set_Parry(false);

			dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(100))
		{
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(140))
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

STATE CState_Parry_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	

	return eState;
}

void CState_Parry_Shield::Reset_State()
{
	m_bParryEnd = false;
	m_bParryLeft1 = false;
	m_bParryRight1 = false;
	m_bParryLeft2 = false;
	m_bParryRight2 = false;
}

void CState_Parry_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pRealOwner->Set_Parry(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.3f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 27, 2.f);
		m_bParryLeft1 = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 28, 2.f);
		m_bParryRight1 = true;
	}

	

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Parry_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_Shield* CState_Parry_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_Shield* pInstance = new CState_Parry_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_Shield::Free()
{
	__super::Free();
}
