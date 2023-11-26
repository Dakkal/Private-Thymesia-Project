#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Run_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Run_Urd::CState_Run_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Run_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Run_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bRush && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(32))
	{
		_float fMinDist = 4.f; _float fMinRushDist = 6.f; _float fMaxRushDist = 10.f;

		if (4.f <= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			10.f >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_SkillCnt() >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_SkillActive())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::SKILL;
		}
		else if (fMinRushDist <= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			fMaxRushDist >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			(33 == m_pOwnerBodyPart->Get_AnimationIndex() || 35 == m_pOwnerBodyPart->Get_AnimationIndex()))
		{
			if (33 == m_pOwnerBodyPart->Get_AnimationIndex())
			{
				dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Add_SkillCnt();
				m_pOwnerBodyPart->Set_AnimationIndex(false, 6, 2.f);
				m_bRush = true;
			}
			else if (35 == m_pOwnerBodyPart->Get_AnimationIndex())
			{
				dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Add_SkillCnt();
				m_pOwnerBodyPart->Set_AnimationIndex(false, 5, 2.f);
				m_bRush = true;
			}
		}
		else if (fMinDist <= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance())
		{
			
			if (32 == m_pOwnerBodyPart->Get_AnimationIndex())
			{
				_int iRandom = pGameInstance->Random_Int(0, 2);

				if (0 == iRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 34, 1.2f);
				}
				else if (1 == iRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.2f);
				}
				else if (2 == iRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 1.2f);
				}
			}
			else if (34 == m_pOwnerBodyPart->Get_AnimationIndex())
			{
				_int iRandom = pGameInstance->Random_Int(0, 2);

				if (0 == iRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 1.2f);
				}
				else if (1 == iRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.2f);
				}
				else if (2 == iRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 1.2f);
				}
			}
			else if (33 == m_pOwnerBodyPart->Get_AnimationIndex())
			{
				_bool bRandom = pGameInstance->Random_Coin(0.5f);

				if (true == bRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 1.2f);
				}
				else
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 1.2f);
				}
			}
			else if (35 == m_pOwnerBodyPart->Get_AnimationIndex())
			{
				_bool bRandom = pGameInstance->Random_Coin(0.5f);

				if (true == bRandom)
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 1.2f);
				}
				else
				{
					m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.2f);
				}
			}
		}
		else
		{
			_bool bRandom = pGameInstance->Random_Coin(0.8f);

			if (true == bRandom)
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::ATTACK;
			}
			else
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::WALK;
			}
		
		}
	}

	if (true == m_bRush)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
		{
			CTransform* pTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();
			if (nullptr != pTransform)
				m_pOwnerBodyPart->Set_Anim_TargetPos(pTransform->Get_State(CTransform::STATE_POS));

		

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(80))
		{
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(110))
		{
			if (true == m_pRealOwner->Is_Hit())
			{
				RELEASE_INSTANCE(CGameInstance)
				return STATE::HIT;
			}
				
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			RELEASE_INSTANCE(CGameInstance)
			return STATE::IDLE;
		}

	}

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Run_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Run_Urd::Reset_State()
{
	m_bDashEnd = false;
	m_bRush = false;
	m_pOwnerBodyPart->Set_Anim_TargetPos(XMVectorZero());
}

void CState_Run_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iRandom = pGameInstance->Random_Int(0, 2);

	if (0 == iRandom)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 1.2f);
	}
	else if (1 == iRandom)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.2f);
	}
	else if (2 == iRandom)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 1.2f);
	}

	RELEASE_INSTANCE(CGameInstance);

}

STATE CState_Run_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Run_Urd* CState_Run_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Run_Urd* pInstance = new CState_Run_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Run_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Run_Urd::Free()
{
	__super::Free();
}
