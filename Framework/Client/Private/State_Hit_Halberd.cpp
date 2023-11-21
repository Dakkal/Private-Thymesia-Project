#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Hit_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"

CState_Hit_Halberd::CState_Hit_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Hit_Halberd::Initialize()
{
	__super::Initialize();

	m_bRight_Hit = true;

	return S_OK;
}

STATE CState_Hit_Halberd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bParry)
		return STATE::PARRY;

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
		if (true == pGameInstance->Random_Coin(0.1f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::PARRY;
		}
		RELEASE_INSTANCE(CGameInstance);

		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);

		if (true == m_bRight_Hit)
		{
			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 43, 1.2f);
			m_bLeft_Hit = true;

			m_pRealOwner->Subtract_HP();
			m_iHitCnt++;
		}
		else if (true == m_bLeft_Hit)
		{
			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 44, 1.2f);
			m_bRight_Hit = true;

			m_pRealOwner->Subtract_HP();
			m_iHitCnt++;
		}
	}
	else if (m_pOwnerBodyPart->IsAnimationEnd())
		return STATE::IDLE;


	return eState;
}

STATE CState_Hit_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);

	return eState;
}

void CState_Hit_Halberd::Reset_State()
{
	m_iHitCnt = 0;
	m_bRight_Hit = true;
	m_bParry = false;
}

void CState_Hit_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.1f))
	{
		m_bParry = true;
	}
	else
	{
		if (true == m_bRight_Hit)
		{
			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 43, 1.2f);
			m_bLeft_Hit = true;
		}
		else if (true == m_bLeft_Hit)
		{
			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 44, 1.2f);
			m_bRight_Hit = true;
		}

		m_iRecoverCnt = pGameInstance->Random_Int(2, 3);

		m_pRealOwner->Subtract_HP();
		m_iHitCnt++;
	}

	RELEASE_INSTANCE(CGameInstance);
	
}

STATE CState_Hit_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Hit_Halberd* CState_Hit_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Hit_Halberd* pInstance = new CState_Hit_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Hit_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Hit_Halberd::Free()
{
	__super::Free();
}
