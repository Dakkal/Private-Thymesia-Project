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

	if (true == m_IsParry)
		return STATE::PARRY;

	if (0 >= m_pRealOwner->Get_HP())
		return STATE::STUN;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_iRecoverCnt < m_iHitCnt)
	{
		if (true == pGameInstance->Random_Coin(0.8f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::AVOID;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::ATTACK;
		}
	}


	if (m_pRealOwner->Is_Hit())
	{
		if (true == pGameInstance->Random_Coin(0.1f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::PARRY;
		}

		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

		if (true == m_bRight_Hit)
		{
			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 15, 1.2f);
			m_bLeft_Hit = true;

			m_pRealOwner->Subtract_HP();
			m_iHitCnt++;
		}
		else if (true == m_bLeft_Hit)
		{
			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 16, 1.2f);
			m_bRight_Hit = true;

			m_pRealOwner->Subtract_HP();
			m_iHitCnt++;
		}
	}
	else if (m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}
		

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Hit_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

	return eState;
}

void CState_Hit_Urd::Reset_State()
{
	m_bRight_Hit = true;
	m_IsParry = false;
	m_iHitCnt = 0;
}

void CState_Hit_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.1f))
	{
		m_IsParry = true;
	}
	else
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

		m_iRecoverCnt = pGameInstance->Random_Int(3, 4);

		m_pRealOwner->Subtract_HP();
		m_iHitCnt++;
	}

	RELEASE_INSTANCE(CGameInstance);

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
