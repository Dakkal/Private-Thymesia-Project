#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Avoid_GreatSword.h"
#include "Transform.h"

#include "Enemy_GreatSword.h"

CState_Avoid_GreatSword::CState_Avoid_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid_GreatSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid_GreatSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (36 == m_pOwnerBodyPart->Get_AnimationIndex())
	{
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;

			return STATE::IDLE;
		}
	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;

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
	}

	

	return eState;
}

STATE CState_Avoid_GreatSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Avoid_GreatSword::Reset_State()
{
	m_bEnter = false;
}

void CState_Avoid_GreatSword::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_GreatSword*>(m_pRealOwner)->Set_LookPlayer(true);

	if (STATE::ATTACK == m_pStateOwner->Get_PreState())
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 36, 1.2f);

		return;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.6f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 37, 1.2f);
	}
	else
		m_pOwnerBodyPart->Set_AnimationIndex(false, 38, 1.2f);

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Avoid_GreatSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Avoid_GreatSword* CState_Avoid_GreatSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid_GreatSword* pInstance = new CState_Avoid_GreatSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid_GreatSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid_GreatSword::Free()
{
	__super::Free();
}
