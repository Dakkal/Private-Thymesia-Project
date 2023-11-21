#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_TwinSword.h"
#include "Transform.h"

#include "Enemy_TwinSword.h"

CState_Parry_TwinSword::CState_Parry_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_TwinSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (false == m_bParryEnd && true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
			m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.f);
			m_bParryEnd = true;
		}
	}
	else if (false == m_bParryEnd && true == m_bParryRight1)
	{
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
			m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 2.2f);
			m_bParryEnd = true;
		}
	}

	if(true == m_bParryEnd && true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(21))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(41))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(65))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(70))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(75))
		{
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
		{
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(100))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(110))
		{
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryEnd && true == m_bParryRight1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(85))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(95))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(125))
		{
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(145))
		{
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(220))
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

STATE CState_Parry_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Parry_TwinSword::Reset_State()
{
	m_bParryEnd = false;
	m_bParryLeft1 = false;
	m_bParryRight1 = false;
	m_bParryLeft2 = false;
	m_bParryRight2 = false;
}

void CState_Parry_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.5f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 2.f);
		m_bParryLeft1 = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 34, 2.f);
		m_bParryRight1 = true;
	}

	

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Parry_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_TwinSword* CState_Parry_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_TwinSword* pInstance = new CState_Parry_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_TwinSword::Free()
{
	__super::Free();
}
