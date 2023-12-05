#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_TwinSword.h"
#include "Transform.h"

#include "Enemy_TwinSword.h"
#include "ParrySpark.h"

CState_Parry_TwinSword::CState_Parry_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_TwinSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (false == m_bParryEnd && true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			m_pRealOwner->Set_Parry(false);

			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
			m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.f);
			m_bParryEnd = true;
		}
	}
	else if (false == m_bParryEnd && true == m_bParryRight1)
	{
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			m_pRealOwner->Set_Parry(false);

			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
			m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 2.2f);
			m_bParryEnd = true;
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(true == m_bParryEnd && true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(21))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(41))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_03.ogg"), CHANNELID::CHANNEL_11, 0.8f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(65))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(70))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(75))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboEnd_02.ogg"), CHANNELID::CHANNEL_12, 1.f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
		{
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(90))
		{
			if (true == m_pRealOwner->Is_Hit())
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::HIT;
			}
				
		}
		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(110))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryEnd && true == m_bParryRight1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(85))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(95))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(125))
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_03.ogg"), CHANNELID::CHANNEL_11, 0.8f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboB01_2.ogg"), CHANNELID::CHANNEL_12, 1.f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(145))
		{
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(200))
		{
			if (true == m_pRealOwner->Is_Hit())
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::HIT;
			}
				
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::IDLE;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Parry_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_IsParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(3))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffectObject::EFFECT_DESC	effectdesc;
		effectdesc.pTargetTransform = m_pOwnerTransform;
		effectdesc.vTargetPos = m_pOwnerWeaponLPart->Get_Collide_Center();
		effectdesc.vTargetPos.y += 0.2f;
		effectdesc.vRotFactorX = _float2(-15.f, 15.f);

		if(true == m_bParryLeft1 || true == m_bParryLeft2)
			effectdesc.vRotFactorZ = _float2(60.f, 80.f);
		else if (true == m_bParryRight1 || true == m_bParryRight2)
			effectdesc.vRotFactorZ = _float2(-60.f, -80.f);

		effectdesc.eEffectType = CEffectObject::TYPE::RECT;

		pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_EFFECT, TEXT("Prototype_GameObject_ParrySpark"), &effectdesc);

		RELEASE_INSTANCE(CGameInstance);

		m_IsParry = false;
	}

	return eState;
}

void CState_Parry_TwinSword::Reset_State()
{
	m_IsParry = false;

	m_bParryEnd = false;
	m_bParryLeft1 = false;
	m_bParryRight1 = false;
	m_bParryLeft2 = false;
	m_bParryRight2 = false;
}

void CState_Parry_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);

	m_pRealOwner->Set_Parry(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.5f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 2.f);
		m_bParryLeft1 = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 34, 2.f);
		m_bParryRight1 = true;
	}

	m_IsParry = true;

	pGameInstance->PlaySoundFile(TEXT("ParrySuc_02_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Parry_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_TwinSword* CState_Parry_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_TwinSword* pInstance = new CState_Parry_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_TwinSword::Free()
{
	__super::Free();
}
