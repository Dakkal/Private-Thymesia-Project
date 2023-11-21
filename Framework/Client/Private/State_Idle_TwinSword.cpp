#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_TwinSword.h"
#include "Transform.h"

#include "Enemy_TwinSword.h"

CState_Idle_TwinSword::CState_Idle_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_TwinSword::Tick(const _float& fTimeDelta)
{
	if (true == m_pOwnerBodyPart->IsAnimChange())
		return m_eState;

	STATE eState = m_eState;

	if (false == dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Is_LookPlayer())
	{
		_float fLookDist = 20.f;
		if (fLookDist >= dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Get_PlayerDistance())
			dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
	}

	_float fActDist = 15.f;
	if (fActDist >= dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Get_PlayerDistance())
	{
		m_fIdleTime += fTimeDelta;

		if (0.1f <= m_fIdleTime)
		{
			_float fDist = 6.f;
			if (fDist <= dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Get_PlayerDistance())
				return STATE::RUN;

			_float fWalkDist = 4.f;
			if (fWalkDist < dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Get_PlayerDistance())
				return STATE::WALK;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (true == pGameInstance->Random_Coin(0.8f))
				{
					RELEASE_INSTANCE(CGameInstance);
					return STATE::WALK;
				}
				else
				{
					RELEASE_INSTANCE(CGameInstance);
					return STATE::AVOID;
				}
			}
		}
	}

	return eState;
}

STATE CState_Idle_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Idle_TwinSword::Reset_State()
{
	m_fIdleTime = 0.f;
}

void CState_Idle_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	_float fDist = 20.f;

	if (fDist >= dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Get_PlayerDistance())
		dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);
	else
		dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(false);


	m_pOwnerBodyPart->Set_AnimationIndex(true, 28, 1.2f);
}

STATE CState_Idle_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_TwinSword* CState_Idle_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_TwinSword* pInstance = new CState_Idle_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_TwinSword::Free()
{
	__super::Free();
}
