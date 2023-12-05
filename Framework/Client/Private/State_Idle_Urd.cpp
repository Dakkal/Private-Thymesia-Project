#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Idle_Urd::CState_Idle_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_Urd::Tick(const _float& fTimeDelta)
{
	if (true == m_pOwnerBodyPart->IsAnimChange())
		return m_eState;

	if (true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	STATE eState = m_eState;

	m_fIdleTime += fTimeDelta;

	if (0.3f <= m_fIdleTime)
	{
		_float fMinSkillDist = 4.f; _float fMaxSkilDist = 10.f;

		if (fMinSkillDist <= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			fMaxSkilDist >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_SkillCnt() >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_SkillActive())
		{
			return STATE::SKILL;
		}


		_float fMinRushDist = 4.f;
		if (fMinRushDist < dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance())
			return STATE::RUN;

		_float fWalkDist = 4.f;
		if (fWalkDist >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			_int iRandom = pGameInstance->Random_Int(0, 6);

			if (0 == iRandom ||  6 == iRandom)
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::WALK;
			}
			else if (1 == iRandom || 5 == iRandom || 4 == iRandom)
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::ATTACK;
			}
			else if (3 == iRandom || 2 == iRandom)
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::AVOID;
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}

	return eState;
}

STATE CState_Idle_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Idle_Urd::Reset_State()
{
	m_fIdleTime = 0.f;

	m_bEnter = false;
}

void CState_Idle_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 19, 1.2f);

}

STATE CState_Idle_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_Urd* CState_Idle_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_Urd* pInstance = new CState_Idle_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_Urd::Free()
{
	__super::Free();
}
