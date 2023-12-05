#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Hit.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Hit::CState_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Hit::Initialize()
{
	__super::Initialize();

	m_bRight_Hit = true;

	return S_OK;
}

STATE CState_Hit::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (m_pRealOwner->Is_Hit())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == m_bRight_Hit)
		{
			pGameInstance->PlaySoundFile(TEXT("PlayerHit_02.ogg"), CHANNELID::CHANNEL_1, 1.f);

			m_bRight_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 84, 2.f);
			m_bLeft_Hit = true;
		}
		else if (true == m_bLeft_Hit)
		{
			pGameInstance->PlaySoundFile(TEXT("PlayerHit_03.ogg"), CHANNELID::CHANNEL_1, 1.f);

			m_bLeft_Hit = false;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 83, 2.f);
			m_bRight_Hit = true;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		if (m_pOwnerBodyPart->IsAnimationEnd())
		{
			CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
			if (nullptr == pTarget)
				return STATE::IDLE;
			else
				return STATE::LOCK_IDLE;
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(15))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
			{
				CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
				if (nullptr == pTarget)
				{
					RELEASE_INSTANCE(CGameInstance);
					return STATE::PARRY;
				}
				else
				{
					RELEASE_INSTANCE(CGameInstance);
					return STATE::LOCK_PARRY;
				}
			}
			
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(20))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
			{
				CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
				if (nullptr == pTarget)
				{
					RELEASE_INSTANCE(CGameInstance);
					return STATE::AVOID;
				}
				else
				{
					RELEASE_INSTANCE(CGameInstance);
					return STATE::LOCK_AVOID;
				}
			}

			RELEASE_INSTANCE(CGameInstance);
		}
		
	}
	
	return eState;
}

STATE CState_Hit::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Hit::Reset_State()
{
	m_bRight_Hit = true;
}

void CState_Hit::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pRealOwner->Is_Parried())
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 85, 2.f);
		m_pRealOwner->Set_Parried(false);
		
		//pGameInstance->PlaySoundFile(TEXT("Alert_Dangerious_01.ogg"), CHANNELID::CHANNEL_1, 0.3f);

		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	pGameInstance->PlaySoundFile(TEXT("PlayerHit_01.ogg"), CHANNELID::CHANNEL_1, 1.f);

	RELEASE_INSTANCE(CGameInstance);

	if (true == m_bRight_Hit)
	{
		m_bRight_Hit = false;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 84, 2.f);
		m_bLeft_Hit = true;
	}
	else if (true == m_bLeft_Hit)
	{
		m_bLeft_Hit = false;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 83, 2.f);
		m_bRight_Hit = true;
	}
}

STATE CState_Hit::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Hit* CState_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Hit* pInstance = new CState_Hit(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Hit::Free()
{
	__super::Free();
}
