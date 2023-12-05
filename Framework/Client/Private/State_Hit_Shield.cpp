#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Hit_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"

CState_Hit_Shield::CState_Hit_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Hit_Shield::Initialize()
{
	__super::Initialize();

	m_bRight_Hit = true;

	return S_OK;
}

STATE CState_Hit_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bParry)
		return STATE::PARRY;

	if (true == m_bAvoid)
		return STATE::AVOID;

	if (0 >= m_pRealOwner->Get_HP())
		return STATE::DEAD;


	if (m_iRecoverCnt < m_iHitCnt)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
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
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (true == pGameInstance->Random_Coin(0.3f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::AVOID;
		}
		if (true == pGameInstance->Random_Coin(0.1f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::PARRY;
		}
		RELEASE_INSTANCE(CGameInstance);

		dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

		if (true == m_bRight_Hit)
		{
			if (1 == m_pRealOwner->Get_Index())
			{
				pGameInstance->PlaySoundFile(TEXT("Magician_Hurt_02.ogg"), CHANNELID::CHANNEL_7, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_HurtM.ogg"), CHANNELID::CHANNEL_8, 1.f);
			}
			else
			{
				pGameInstance->PlaySoundFile(TEXT("Magician_Hurt_02.ogg"), CHANNELID::CHANNEL_9, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_HurtM.ogg"), CHANNELID::CHANNEL_10, 1.f);
			}
			

			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 17, 0.9f, false, 0, 0.35f);
			m_bLeft_Hit = true;

			m_pRealOwner->Subtract_HP();
			m_iHitCnt++;
		}
		else if (true == m_bLeft_Hit)
		{
			if (1 == m_pRealOwner->Get_Index())
			{
				pGameInstance->PlaySoundFile(TEXT("Magician_Hurt_03.ogg"), CHANNELID::CHANNEL_7, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_HurtM.ogg"), CHANNELID::CHANNEL_8, 1.f);
			}
			else
			{
				pGameInstance->PlaySoundFile(TEXT("Magician_Hurt_03.ogg"), CHANNELID::CHANNEL_9, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_HurtM.ogg"), CHANNELID::CHANNEL_10, 1.f);
			}
			

			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 18, 0.9f, false, 0, 0.35f);
			m_bRight_Hit = true;

			m_pRealOwner->Subtract_HP();
			m_iHitCnt++;
		}
	}
	else if (m_pOwnerBodyPart->IsAnimationEnd())
		return STATE::IDLE;


	return eState;
}

STATE CState_Hit_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);

	return eState;
}

void CState_Hit_Shield::Reset_State()
{
	m_iHitCnt = 0;
	m_bRight_Hit = true;
	m_bParry = false;
	m_bAvoid = false;
}

void CState_Hit_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.1f))
	{
		m_bParry = true;
	}
	else if (true == pGameInstance->Random_Coin(0.3f))
	{
		m_bAvoid = true;
	}
	else 
	{

		if (1 == m_pRealOwner->Get_Index())
		{
			pGameInstance->PlaySoundFile(TEXT("Magician_Hurt_01.ogg"), CHANNELID::CHANNEL_7, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_HurtM.ogg"), CHANNELID::CHANNEL_8, 1.f);
		}
		else
		{
			pGameInstance->PlaySoundFile(TEXT("Magician_Hurt_01.ogg"), CHANNELID::CHANNEL_9, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_HurtM.ogg"), CHANNELID::CHANNEL_10, 1.f);
		}

	


		if (true == m_bRight_Hit)
		{
			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 17, 0.9f, false, 0, 0.35f);
			m_bLeft_Hit = true;
		}
		else if (true == m_bLeft_Hit)
		{
			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 18, 0.9f, false, 0, 0.35f);
			m_bRight_Hit = true;
		}

		m_iRecoverCnt = pGameInstance->Random_Int(2, 3);

		m_pRealOwner->Subtract_HP();
		m_iHitCnt++;
	}

	RELEASE_INSTANCE(CGameInstance);
	
}

STATE CState_Hit_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Hit_Shield* CState_Hit_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Hit_Shield* pInstance = new CState_Hit_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Hit_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Hit_Shield::Free()
{
	__super::Free();
}
