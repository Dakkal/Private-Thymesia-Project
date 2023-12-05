#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Attack_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"

CState_Attack_Shield::CState_Attack_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Attack_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Attack_Shield::Tick(const _float& fTimeDelta)
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

STATE CState_Attack_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Attack_Shield::Reset_State()
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

void CState_Attack_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iRandom = pGameInstance->Random_Int(0, 5);

	if (0 == iRandom || 1 == iRandom)
	{
		m_bCombo1 = true;
		m_bCombo1_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 0, 1.1f);
	}
	else if (2 == iRandom || 3 == iRandom)
	{
		m_bCombo2 = true;
		m_bCombo2_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 1, 1.2f);
	}
	else if (4 == iRandom || 5 == iRandom)
	{
		m_bCombo3 = true;
		m_bCombo3_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 4, 1.2f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Attack_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

STATE CState_Attack_Shield::Combo_1()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
	{
		if (1 == m_pRealOwner->Get_Index())
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_7, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack01.ogg"), CHANNELID::CHANNEL_8, 1.f);
		}
		else
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_9, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack01.ogg"), CHANNELID::CHANNEL_10, 1.f);
		}


		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(40))
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

STATE CState_Attack_Shield::Combo_2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
	{
		if (1 == m_pRealOwner->Get_Index())
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_7, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack02_01.ogg"), CHANNELID::CHANNEL_8, 1.f);
		}
		else
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_01.ogg"), CHANNELID::CHANNEL_9, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack02_01.ogg"), CHANNELID::CHANNEL_10, 1.f);
		}

		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(25))
	{
		m_pRealOwner->Set_Attack(false);

		m_bCombo2_1 = false;
		m_bCombo2_2 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 2, 1.2f);

	}

	if (true == m_bCombo2_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		if (1 == m_pRealOwner->Get_Index())
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_7, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack02_02.ogg"), CHANNELID::CHANNEL_8, 1.f);
		}
		else
		{
			pGameInstance->PlaySoundFile(TEXT("Blade_Attack_02.ogg"), CHANNELID::CHANNEL_9, 1.f);
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack02_02.ogg"), CHANNELID::CHANNEL_10, 1.f);
		}

		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}
	if (true == m_bCombo2_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(40))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo2_2 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_Shield::Combo_3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(10))
	{
		if (1 == m_pRealOwner->Get_Index())
		{
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack02_03.ogg"), CHANNELID::CHANNEL_7, 1.f);
		}
		else
		{
			pGameInstance->PlaySoundFile(TEXT("UM_V_Shield_Attack02_03.ogg"), CHANNELID::CHANNEL_9, 1.f);
		}

		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
	{
		dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}
	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(30))
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
	

	return m_eState;
}

STATE CState_Attack_Shield::Combo_4()
{
	

	return m_eState;
}

CState_Attack_Shield* CState_Attack_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Attack_Shield* pInstance = new CState_Attack_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Attack_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack_Shield::Free()
{
	__super::Free();
}
