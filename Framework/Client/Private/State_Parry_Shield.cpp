#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"
#include "ParrySpark.h"

CState_Parry_Shield::CState_Parry_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
		{
			m_pRealOwner->Set_Parry(false);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (1 == m_pRealOwner->Get_Index())
			{
				pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_7, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Parry_Attack01-001.ogg"), CHANNELID::CHANNEL_8, 1.f);
			}
			else
			{
				pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_9, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Parry_Attack01-001.ogg"), CHANNELID::CHANNEL_10, 1.f);
			}

			RELEASE_INSTANCE(CGameInstance);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(80))
		{
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(130))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryRight1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
		{
			m_pRealOwner->Set_Parry(false);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (1 == m_pRealOwner->Get_Index())
			{
				pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_7, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Parry_Attack02-001.ogg"), CHANNELID::CHANNEL_8, 1.f);
			}
			else
			{
				pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_9, 1.f);
				pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Parry_Attack02-001.ogg"), CHANNELID::CHANNEL_10, 1.f);
			}

			
			RELEASE_INSTANCE(CGameInstance);

			
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(100))
		{
			dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(130))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}
	}
	

	return eState;
}

STATE CState_Parry_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_IsParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(2))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffectObject::EFFECT_DESC	effectdesc;
		effectdesc.pTargetTransform = m_pOwnerTransform;
		effectdesc.vTargetPos = m_pOwnerWeaponLPart->Get_Collide_Center();
		effectdesc.vTargetPos.y += 0.2f;
		effectdesc.vRotFactorX = _float2(45.f, 100.f);

		effectdesc.vRotFactorZ = _float2(45.f, 100.f);

		effectdesc.eEffectType = CEffectObject::TYPE::RECT;

		pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_EFFECT, TEXT("Prototype_GameObject_ParrySpark"), &effectdesc);

		RELEASE_INSTANCE(CGameInstance);

		m_IsParry = false;
	}


	return eState;
}

void CState_Parry_Shield::Reset_State()
{
	m_IsParry =  false;
	m_bParryEnd = false;
	m_bParryLeft1 = false;
	m_bParryRight1 = false;
	m_bParryLeft2 = false;
	m_bParryRight2 = false;
}

void CState_Parry_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pRealOwner->Set_Parry(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.3f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 27, 2.f);
		m_bParryLeft1 = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 28, 2.f);
		m_bParryRight1 = true;
	}

	m_IsParry = true;

	if(1 == m_pRealOwner->Get_Index())
		pGameInstance->PlaySoundFile(TEXT("Armor_Metal_ShieldHitA_01.ogg"), CHANNELID::CHANNEL_7, 1.f);
	else
		pGameInstance->PlaySoundFile(TEXT("Armor_Metal_ShieldHitA_01.ogg"), CHANNELID::CHANNEL_9, 1.f);

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Parry_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_Shield* CState_Parry_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_Shield* pInstance = new CState_Parry_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_Shield::Free()
{
	__super::Free();
}
