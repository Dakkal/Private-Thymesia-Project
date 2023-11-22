#include "..\Public\State_Avoid_Urd.h"

#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Avoid_Urd::CState_Avoid_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;
		
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (31 == m_pOwnerBodyPart->Get_AnimationIndex())
	{
		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(35))
		{
			if (true == pGameInstance->Random_Coin(0.7f))
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::WALK;
			}
			else
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::RUN;
			}
		}
	}
	else
	{
		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(32))
		{
			if (1 <= m_iAvoidCnt)
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::ATTACK;
			}

			if (true == pGameInstance->Random_Coin(0.4f))
			{
				if (true == m_bLeft)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 37, 1.f);
					m_bLeft = false;
					m_bRight = true;

				}
				else if (true == m_bRight)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 36, 1.f);
					m_bLeft = true;
					m_bRight = false;
				}
				m_iAvoidCnt++;
			}
			else
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::ATTACK;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Avoid_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	

	return eState;
}

void CState_Avoid_Urd::Reset_State()
{
	m_bLeft = false;
	m_bRight = false;
	m_iAvoidCnt = 0;
}

void CState_Avoid_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	if (STATE::ATTACK == m_pStateOwner->Get_PreState())
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 31, 1.f);
		return;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Random_Coin(0.5))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 36, 1.f);
		m_bLeft = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 37, 1.f);
		m_bRight = true;
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Avoid_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Avoid_Urd* CState_Avoid_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid_Urd* pInstance = new CState_Avoid_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid_Urd::Free()
{
	__super::Free();
}

