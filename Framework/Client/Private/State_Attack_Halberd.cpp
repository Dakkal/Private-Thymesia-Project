#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Attack_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"

CState_Attack_Halberd::CState_Attack_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Attack_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Attack_Halberd::Tick(const _float& fTimeDelta)
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

STATE CState_Attack_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Attack_Halberd::Reset_State()
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

void CState_Attack_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iRandom = pGameInstance->Random_Int(0, 6);

	if (0 == iRandom || 1 == iRandom)
	{
		m_bCombo1 = true;
		m_bCombo1_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 22, 1.2f);
	}
	else if (2 == iRandom || 3 == iRandom)
	{
		m_bCombo2 = true;
		m_bCombo2_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 25, 1.2f);
	}
	else if (4 == iRandom || 5 == iRandom)
	{
		m_bCombo3 = true;
		m_bCombo3_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 26, 1.2f);
	}
	else if (6 == iRandom)
	{
		m_bCombo4 = true;
		m_bCombo4_1 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 3, 2.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Attack_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

STATE CState_Attack_Halberd::Combo_1()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_01.ogg"), CHANNELID::CHANNEL_5, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_ComboA01_01.ogg"), CHANNELID::CHANNEL_6, 1.f);

		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(25))
	{
		m_pRealOwner->Set_Attack(false);

		m_bCombo1_1 = false;
		m_bCombo1_2 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 23, 1.2f);
	}

	if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(5))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_02.ogg"), CHANNELID::CHANNEL_5, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_ComboA01_02.ogg"), CHANNELID::CHANNEL_6, 1.f);

		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(28))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_03.ogg"), CHANNELID::CHANNEL_5, 0.8f);
		pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_ComboA01_03.ogg"), CHANNELID::CHANNEL_6, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(45))
	{
		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(50))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
			
	}
	if (true == m_bCombo1_2 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_Halberd::Combo_2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(25))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_AttackHeavy_02.ogg"), CHANNELID::CHANNEL_5, 0.8);
		pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_ComboB01.ogg"), CHANNELID::CHANNEL_6, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(40))
	{
		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo2_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(50))
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

STATE CState_Attack_Halberd::Combo_3()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_01.ogg"), CHANNELID::CHANNEL_5, 0.8);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(45))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo3_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(50))
	{
		m_bCombo3_1 = false;
		m_bCombo3_2 = true;
		m_pOwnerBodyPart->Set_AnimationIndex(false, 27, 1.2f);
	}

	if (true == m_bCombo3_2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(15))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_AttackHeavy_01.ogg"), CHANNELID::CHANNEL_5, 0.8);
		pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_ComboC01.ogg"), CHANNELID::CHANNEL_6, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo3_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(30))
	{
		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo3_2 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(40))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
	}
	if (true == m_bCombo3_2 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Attack_Halberd::Combo_4()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_03.ogg"), CHANNELID::CHANNEL_5, 0.8);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(80))
	{
		m_pRealOwner->Set_Attack(false);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(130))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_02.ogg"), CHANNELID::CHANNEL_5, 0.8);
		pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_ComboC02.ogg"), CHANNELID::CHANNEL_6, 1.f);
		m_pRealOwner->Set_Attack(true);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(150))
	{
		pGameInstance->PlaySoundFile(TEXT("Spear_Whoosh_02.ogg"), CHANNELID::CHANNEL_5, 0.8);
	}
	else if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(180))
	{
		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pRealOwner->Set_Attack(false);
	}

	if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(260))
	{
		if (true == m_pRealOwner->Is_Hit())
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::HIT;
		}
	}
	if (true == m_bCombo4_1 && true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

CState_Attack_Halberd* CState_Attack_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Attack_Halberd* pInstance = new CState_Attack_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Attack_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack_Halberd::Free()
{
	__super::Free();
}
