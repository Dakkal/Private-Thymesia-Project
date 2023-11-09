#include "..\Public\State_Avoid_Urd.h"

#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "Transform.h"

#include "Boss_Urd.h"

CState_Avoid_Urd::CState_Avoid_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	cout << "È¸ÇÇ" << endl;

	if (m_pOwnerBodyPart->Is_AnimCurKeyFrame(32))
		return STATE::IDLE;

	return eState;
}

STATE CState_Avoid_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Avoid_Urd::Reset_State()
{
	m_bEnter = false;
}

void CState_Avoid_Urd::Enter_State()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);


	CTransform* pPlayerTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POS);
	_vector vOwnerPos = m_pOwnerTransform->Get_State(CTransform::STATE_POS);

	_float fDist = _vector(vPlayerPos - vOwnerPos).Length();

	if (5 >= fDist)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 31, 1.2f, true);
	}
	else if (5 < fDist && 10 >= fDist)
	{
		if (pGameInstance->Random_Coin(0.5))
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 36, 1.2f, true);
		}
		else
			m_pOwnerBodyPart->Set_AnimationIndex(false, 37, 1.2f, true);
	}
	else if (10 < fDist)
	{
		_int iSelect = pGameInstance->Random_Int(0, 2);

		if (0 == iSelect)
			m_pOwnerBodyPart->Set_AnimationIndex(false, 35, 1.2f, true);
		else if(1 == iSelect)
			m_pOwnerBodyPart->Set_AnimationIndex(false, 32, 1.2f, true);
		else
			m_pOwnerBodyPart->Set_AnimationIndex(false, 33, 1.2f, true);
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Avoid_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Avoid_Urd* CState_Avoid_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid_Urd* pInstance = new CState_Avoid_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid_Urd::Free()
{
	__super::Free();
}

