#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Avoid_TwinSword.h"
#include "Transform.h"

#include "Enemy_TwinSword.h"

CState_Avoid_TwinSword::CState_Avoid_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid_TwinSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(80))
	{
		if (true == m_pRealOwner->Is_Hit())
			return STATE::HIT;
	}
	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(90))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}

	RELEASE_INSTANCE(CGameInstance);

	
	return eState;
}

STATE CState_Avoid_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Avoid_TwinSword::Reset_State()
{
	m_bEnter = false;
}

void CState_Avoid_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 8, 1.5f);

	pGameInstance->PlaySoundFile(TEXT("WhooshNylonHard_01.ogg"), CHANNELID::CHANNEL_11, 0.6f);

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Avoid_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Avoid_TwinSword* CState_Avoid_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid_TwinSword* pInstance = new CState_Avoid_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid_TwinSword::Free()
{
	__super::Free();
}
