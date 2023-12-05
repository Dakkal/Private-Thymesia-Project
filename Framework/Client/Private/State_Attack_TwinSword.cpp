#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Attack_TwinSword.h"
#include "Transform.h"

#include "Enemy_TwinSword.h"

CState_Attack_TwinSword::CState_Attack_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Attack_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Attack_TwinSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bCombo1)
		eState = Combo_1();

	if (true == m_bCombo2)
		eState = Combo_2();

	if (true == m_bCombo3)
		eState = Combo_3();

	return eState;
}

STATE CState_Attack_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Attack_TwinSword::Reset_State()
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

void CState_Attack_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iRandom = pGameInstance->Random_Int(0, 6);

	if (0 == iRandom || 1 == iRandom)
	{
		m_bCombo1 = true;
		m_bCombo1_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 0, 2.f);
	}
	else if (2 == iRandom || 3 == iRandom || 6 == iRandom)
	{
		m_bCombo2 = true;
		m_bCombo2_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 3, 2.f);
	}
	else if (4 == iRandom || 5 == iRandom)
	{
		m_bCombo3 = true;
		m_bCombo3_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 5, 2.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Attack_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

STATE CState_Attack_TwinSword::Combo_1()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(45))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(70))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboA_01.ogg"), CHANNELID::CHANNEL_12, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(90))
	{
		m_pRealOwner->Set_Attack(false);
		m_bCombo1_1 = false;
		m_bCombo1_2 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 1, 2.f);
	}

	if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(45))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboA_02.ogg"), CHANNELID::CHANNEL_12, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(100))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}

		if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->IsAnimationEnd())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::IDLE;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_TwinSword::Combo_2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboB01_1-001.ogg"), CHANNELID::CHANNEL_12, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboB01_1-002.ogg"), CHANNELID::CHANNEL_12, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(75))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(100))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_03.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboB01_2.ogg"), CHANNELID::CHANNEL_12, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(110))
	{
		dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(140))
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
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_TwinSword::Combo_3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(41))
	{
		pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_11, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("UM_V_TwinSword_ComboC01.ogg"), CHANNELID::CHANNEL_12, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(90))
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
		return STATE::AVOID;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_TwinSword::Combo_4()
{
	

	return m_eState;
}

CState_Attack_TwinSword* CState_Attack_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Attack_TwinSword* pInstance = new CState_Attack_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Attack_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack_TwinSword::Free()
{
	__super::Free();
}
