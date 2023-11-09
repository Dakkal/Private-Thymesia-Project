#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_Urd.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Idle_Urd::CState_Idle_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	CTransform* pPlayerTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();
	if (nullptr != pPlayerTransform)
	{
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POS);
		_vector vOwnerPos = m_pOwnerTransform->Get_State(CTransform::STATE_POS);

		_float fDist = _vector(vPlayerPos - vOwnerPos).Length();

		cout << fDist << endl;

		if (5 >= fDist)
		{
			return STATE::AVOID;
		}
		else if (5 < fDist && 10 >= fDist)
		{
			return STATE::AVOID;
		}
		else if(10 < fDist)
		{
			return STATE::AVOID;
		}
	}
	

	return eState;
}

STATE CState_Idle_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Idle_Urd::Reset_State()
{
	m_bEnter = false;
}

void CState_Idle_Urd::Enter_State()
{
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 19, 1.2f);
}

STATE CState_Idle_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_Urd* CState_Idle_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_Urd* pInstance = new CState_Idle_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_Urd::Free()
{
	__super::Free();
}
