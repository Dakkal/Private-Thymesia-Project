#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Attack_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Attack_Urd::CState_Attack_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Attack_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Attack_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bCombo1)
		eState = Combo_1();

	if (true == m_bCombo2)
		eState = Combo_2();

	if (true == m_bCombo3)
		eState = Combo_3();

	if (true == m_bCombo4)
		eState = Combo_4();

	return eState;
}

STATE CState_Attack_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Attack_Urd::Reset_State()
{
	m_pOwnerBodyPart->Set_Anim_TargetPos(XMVectorZero());

	if (true == m_bCombo1)
	{
		m_bCombo1 = false;
		m_bCombo1_1 = false;
		m_bCombo1_2 = false;
		m_bCombo1_3 = false;
	}
	if (true == m_bCombo2)
	{
		m_bCombo2 = false;
		m_bCombo2_1 = false;
		m_bCombo2_2 = false;
		m_bCombo2_3 = false;
	}
	if (true == m_bCombo3)
	{
		m_bCombo3 = false;
		m_bCombo3_1 = false;
		m_bCombo3_2 = false;
		m_bCombo3_3 = false;
	}
	if (true == m_bCombo4)
	{
		m_bCombo4 = false;
		m_bCombo4_1 = false;
		m_bCombo4_2 = false;
		m_bCombo4_3 = false;
	}

}

void CState_Attack_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Add_SkillCnt();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iRandom = pGameInstance->Random_Int(0, 6);

	if (0 == iRandom || 1 == iRandom)
	{
		m_bCombo1 = true;
		m_bCombo1_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 0, 1.5f);
	}
	else if (2 == iRandom || 3 == iRandom)
	{
		m_bCombo2 = true;
		m_bCombo2_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 2, 2.f);
	}
	else if (4 == iRandom || 5 == iRandom)
	{
		m_bCombo3 = true;
		m_bCombo3_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 7, 2.f);
	}
	else if (6 == iRandom)
	{
		m_bCombo4 = true;
		m_bCombo4_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 0, 1.5f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Attack_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

STATE CState_Attack_Urd::Combo_1()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_01.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction01.ogg"), CHANNELID::CHANNEL_14, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_02.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_03.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(70))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_04.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(110))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_Urd::Combo_2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack03_04_01.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction03_01.ogg"), CHANNELID::CHANNEL_14, 1.f);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(75))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack03_04_02.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(85))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(130))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::AVOID;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_Urd::Combo_3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack05_01.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack05_02.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction06_01.ogg"), CHANNELID::CHANNEL_14, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(100))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack05_03.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction06_02.ogg"), CHANNELID::CHANNEL_14, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(120))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack05_04.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(false);
	}
	
	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(160))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_Urd::Combo_4()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_01.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction01.ogg"), CHANNELID::CHANNEL_14, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_02.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(55))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_03.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(70))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack01_04.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		m_pRealOwner->Set_Attack(false);
		m_bCombo4_1 = false;

		if (true == pGameInstance->Random_Coin(0.5f))
		{
			m_bCombo4_2 = true;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 8, 2.f);
		}
		else
		{
			m_bCombo4_3 = true;
			m_pOwnerBodyPart->Set_AnimationIndex(false, 9, 2.f);
		}
	}
	
	/* 4-2 ÄÞº¸ */
	if (true == m_bCombo4_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack06.ogg"), CHANNELID::CHANNEL_13, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction09_01.ogg"), CHANNELID::CHANNEL_14, 1.f);

		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(60))
	{
		m_pRealOwner->Set_Attack(false);
	}
	
	if (true == m_bCombo4_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(105))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo4_2 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	/* 4-3 ÄÞº¸ */
	if (true == m_bCombo4_3 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		pGameInstance->PlaySoundFile(TEXT("Urd_Attack07.ogg"), CHANNELID::CHANNEL_13, 0.8f);
	}
	else if (true == m_bCombo4_3 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(90))
	{
		CTransform* pTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();
		m_pOwnerBodyPart->Set_Anim_TargetPos(pTransform->Get_State(CTransform::STATE_POS));
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_3 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(120))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo4_3 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(150))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo4_3 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

CState_Attack_Urd* CState_Attack_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Attack_Urd* pInstance = new CState_Attack_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Attack_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack_Urd::Free()
{
	__super::Free();
}
