#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Avoid_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"

CState_Avoid_Halberd::CState_Avoid_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid_Halberd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(50))
	{
		if (true == m_pRealOwner->Is_Hit())
			return STATE::HIT;
	}
	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		if (true == pGameInstance->Random_Coin(0.8f))
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::WALK;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::ATTACK;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	

	return eState;
}

STATE CState_Avoid_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Avoid_Halberd::Reset_State()
{
	m_bEnter = false;
}

void CState_Avoid_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_int iRandom = pGameInstance->Random_Int(0, 4);

	if (0 == iRandom || 1 == iRandom)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 1, 1.8f);
	}
	else if (2 == iRandom || 3 == iRandom)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 2, 1.8f);
	}
	else if (4 == iRandom)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 0, 1.8f);
	}

	pGameInstance->PlaySoundFile(TEXT("FullPlateFoley_A_02.ogg"), CHANNELID::CHANNEL_5, 1.f);

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Avoid_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Avoid_Halberd* CState_Avoid_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid_Halberd* pInstance = new CState_Avoid_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid_Halberd::Free()
{
	__super::Free();
}
