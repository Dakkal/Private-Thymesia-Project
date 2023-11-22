#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Parry_Urd::CState_Parry_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (false == m_bParryEnd && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);
		m_bParryEnd = true;

		if(true == m_bLeft)
			m_pOwnerBodyPart->Set_AnimationIndex(false, 1, 1.2f, false, 20);
		else if(true == m_bRight)
			m_pOwnerBodyPart->Set_AnimationIndex(false, 3, 1.2f, false, 30);
	}

	if (true == m_bLeft && true == m_bParryEnd)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
		{
			
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(45))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
		{
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(120))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}

	}
	if (true == m_bRight && true == m_bParryEnd)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(70))
		{
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(100))
		{
			m_pRealOwner->Set_Attack(false);
		}
	

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(160))
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

STATE CState_Parry_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Parry_Urd::Reset_State()
{
	m_bParryEnd = false;
	m_bLeft =  false;
	m_bRight =  false;
}

void CState_Parry_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.5f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 20, 1.2f);
		m_bLeft = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 21, 1.2f);
		m_bRight = true;
	}

	RELEASE_INSTANCE(CGameInstance);



	
}

STATE CState_Parry_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_Urd* CState_Parry_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_Urd* pInstance = new CState_Parry_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_Urd::Free()
{
	__super::Free();
}
