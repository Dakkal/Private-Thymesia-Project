#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Parry_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"
#include "ParrySpark.h"

CState_Parry_Halberd::CState_Parry_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Parry_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Parry_Halberd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bParryLeft1)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(25))
		{
			m_pRealOwner->Set_Parry(false);

			
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->PlaySoundFile(TEXT("Spear_AttackHeavy_01.ogg"), CHANNELID::CHANNEL_6, 1.f);

			RELEASE_INSTANCE(CGameInstance);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(40))
		{
			dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(50))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryLeft2)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
		{
			m_pRealOwner->Set_Parry(false);

		
			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(80))
		{
			dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(100))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(120))
		{
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryRight1)
	{
		if (20 <= m_pOwnerBodyPart->Get_CurKeyFrameNumb() && 40 >= m_pOwnerBodyPart->Get_CurKeyFrameNumb())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->CheckPlaySoundFile(TEXT("Spear_Whoosh_01.ogg"), CHANNELID::CHANNEL_6, 1.f);
			

			RELEASE_INSTANCE(CGameInstance);
		}

		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(42))
		{
			m_pRealOwner->Set_Parry(false);

			
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->PlaySoundFile(TEXT("Spear_AttackHeavy_01.ogg"), CHANNELID::CHANNEL_6, 1.f);
			pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_Parry_R.ogg"), CHANNELID::CHANNEL_5, 1.f);

			RELEASE_INSTANCE(CGameInstance);

			m_pRealOwner->Set_Attack(true);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(60))
		{
			dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(70))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			return STATE::IDLE;
		}
	}
	else if (true == m_bParryRight2)
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(120))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->CheckPlaySoundFile(TEXT("Spear_Whoosh_03.ogg"), CHANNELID::CHANNEL_6, 1.f);
			pGameInstance->CheckPlaySoundFile(TEXT("NM_V_Halberds_Parry_R_2.ogg"), CHANNELID::CHANNEL_5, 1.f);

			m_pRealOwner->Set_Parry(false);
			m_pRealOwner->Set_Attack(true);
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(190))
		{
			dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
			m_pRealOwner->Set_Attack(false);
		}

		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(220))
		{
			if (true == m_pRealOwner->Is_Hit())
				return STATE::HIT;
		}
		if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(240))
		{
			return STATE::IDLE;
		}
	}

	return eState;
}

STATE CState_Parry_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_IsParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(2))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffectObject::EFFECT_DESC	effectdesc;
		effectdesc.pTargetTransform = m_pOwnerTransform;
		effectdesc.vTargetPos = m_pOwnerWeaponRPart->Get_Collide_Center();
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

void CState_Parry_Halberd::Reset_State()
{
	m_IsParry = false;

	m_bParryEnd = false;
	m_bParryLeft1 = false;
	m_bParryRight1 = false;
	m_bParryLeft2 = false;
	m_bParryRight2 = false;
}

void CState_Parry_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(true);
	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pRealOwner->Set_Parry(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.5f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 11, 1.2f);
		m_bParryLeft1 = true;
	}
	else
	{
		if (true == pGameInstance->Random_Coin(0.5f))
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 13, 1.2f);
			m_bParryRight1 = true;
		}
		else
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 14, 4.f);
			m_bParryRight2 = true;
		}
	}


	pGameInstance->PlaySoundFile(TEXT("ParrySuc_02_03.ogg"), CHANNELID::CHANNEL_6, 0.8f);


	m_IsParry = true;

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Parry_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Parry_Halberd* CState_Parry_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Parry_Halberd* pInstance = new CState_Parry_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Parry_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Parry_Halberd::Free()
{
	__super::Free();
}
