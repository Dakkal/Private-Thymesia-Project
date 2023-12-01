#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Avoid_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"

CState_Avoid_Shield::CState_Avoid_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(20))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	else if(false == m_pOwnerBodyPart->Is_AnimOverKeyFrame(20))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 6, 1.2f, true);

			m_iGuardTime++;
		}
			

		if (pGameInstance->Random_Coin(0.2f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::PARRY;
		}
		if (m_iGuardCount <= m_iGuardTime)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::ATTACK;
		}
	}

	if (true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		if (true == pGameInstance->Random_Coin(0.8f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::WALK;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::ATTACK;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	

	return eState;
}

STATE CState_Avoid_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Avoid_Shield::Reset_State()
{
	m_bEnter = false;
	m_iGuardTime = 0;
}

void CState_Avoid_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 6, 1.2f);

	m_iGuardCount =  pGameInstance->Random_Int(3, 5);

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Avoid_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Avoid_Shield* CState_Avoid_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid_Shield* pInstance = new CState_Avoid_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid_Shield::Free()
{
	__super::Free();
}
