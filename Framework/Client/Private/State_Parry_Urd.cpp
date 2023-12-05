#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"
#include "ParrySpark.h"

CState_Parry_Urd::CState_Parry_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (false == m_bParryEnd && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		m_pRealOwner->Set_Parry(false);

		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);
		m_bParryEnd = true;

		if(true == m_bLeft)
			m_pOwnerBodyPart->Set_AnimationIndex(false, 1, 1.5f, false, 20);
		else if(true == m_bRight)
			m_pOwnerBodyPart->Set_AnimationIndex(false, 2, 2.f, false, 30);
	}

	if (true == m_bLeft && true == m_bParryEnd)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
		{
			pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_01.ogg"), CHANNELID::CHANNEL_13, 0.8f);
			pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction01.ogg"), CHANNELID::CHANNEL_14, 1.f);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(45))
		{
			pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_02.ogg"), CHANNELID::CHANNEL_13, 0.8f);
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
		{
			m_pRealOwner->Set_Attack(false);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
		{
			pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_03.ogg"), CHANNELID::CHANNEL_13, 0.8f);
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(80))
		{
			pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_04.ogg"), CHANNELID::CHANNEL_13, 0.8f);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(100))
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

		RELEASE_INSTANCE(CGameInstance);
	}
	if (true == m_bRight && true == m_bParryEnd)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
		{
			pGameInstance->PlaySoundFile(TEXT("Urd_Attack03_04_01.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(75))
		{
			pGameInstance->PlaySoundFile(TEXT("Urd_Attack03_04_02.ogg"), CHANNELID::CHANNEL_13, 0.8f);
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(85))
		{
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(130))
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

		RELEASE_INSTANCE(CGameInstance);
	}

	return eState;
}

STATE CState_Parry_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_IsParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(2))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffectObject::EFFECT_DESC	effectdesc;
		effectdesc.pTargetTransform = m_pOwnerTransform;
		effectdesc.vTargetPos = m_pOwnerWeaponRPart->Get_Collide_Center();
		effectdesc.vTargetPos.y += 0.3f;
		effectdesc.vRotFactorX = _float2(-15.f, 15.f);

		if (true == m_bLeft)
			effectdesc.vRotFactorZ = _float2(40.f, 60.f);
		else if (true == m_bRight)
			effectdesc.vRotFactorZ = _float2(-60.f, -80.f);

		effectdesc.eEffectType = CEffectObject::TYPE::RECT;

		pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_EFFECT, TEXT("Prototype_GameObject_ParrySpark"), &effectdesc);

		RELEASE_INSTANCE(CGameInstance);

		m_IsParry = false;
	}
	

	return eState;
}

void CState_Parry_Urd::Reset_State()
{
	m_IsParry = false;
	m_bParryEnd = false;
	m_bLeft =  false;
	m_bRight =  false;

	m_pOwnerBodyPart->Set_Anim_TargetPos(XMVectorZero());
}

void CState_Parry_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Add_SkillCnt();

	m_pRealOwner->Set_Parry(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->PlaySoundFile(TEXT("Urd_Parry.ogg"), CHANNELID::CHANNEL_13, 1.f);

	if (true == pGameInstance->Random_Coin(0.5f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 20, 1.2f);
		m_bLeft = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 21, 1.2f);
		m_bRight = true;
	}

	RELEASE_INSTANCE(CGameInstance);


	m_IsParry = true;
	
}

STATE CState_Parry_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_Urd* CState_Parry_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_Urd* pInstance = new CState_Parry_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_Urd::Free()
{
	__super::Free();
}
