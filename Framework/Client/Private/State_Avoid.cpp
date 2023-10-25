#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Avoid.h"
#include "State_Walk.h"

CState_Avoid::CState_Avoid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Avoid::Initialize()
{
	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pStateOwner->Get_Owner()->Get_Component(TEXT("Com_Transform")));
	CGameObject* pOwnerBodyPart = dynamic_cast<CPlayer*>(m_pStateOwner->Get_Owner())->Get_Parts(CPlayer::PARTS::BODY);
	m_pOwnerBodyPart = dynamic_cast<CPartObject*>(pOwnerBodyPart);

	return S_OK;
}

STATE CState_Avoid::Tick(const _float& fTimeDelta)
{
	STATE eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pOwnerBodyPart->IsAnimationEnd())
	{
		eState = STATE::IDLE;

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 || 
			pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
				eState = STATE::WALK;

		if(true == m_bIsAttack)
			eState = STATE::ATTACK;
	
	}
	else
		eState = m_eState;

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Avoid::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bIsAttack = true;
	}

	RELEASE_INSTANCE(CGameInstance);
	

	return eState;
}

void CState_Avoid::Enter_State()
{
	m_pOwnerBodyPart->Set_AnimationIndex(false, 51, 2.f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	CState_Walk* pWalkState = dynamic_cast<CState_Walk*>(m_pStateOwner->Get_State(STATE::WALK));

	_bool	bClick = false;
	_vector vDir = XMVectorZero();
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		vDir += pWalkState->Get_KeyVector(CState_Walk::KEY::KEY_W);
		bClick = true;
	}
	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		vDir += pWalkState->Get_KeyVector(CState_Walk::KEY::KEY_S);
		bClick = true;
	}
	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		vDir += pWalkState->Get_KeyVector(CState_Walk::KEY::KEY_A);
		bClick = true;
	}
	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		vDir += pWalkState->Get_KeyVector(CState_Walk::KEY::KEY_D);
		bClick = true;
	}

	if(true == bClick)
	m_pOwnerTransform->Set_Look(vDir);

	RELEASE_INSTANCE(CGameInstance);
}

void CState_Avoid::Reset_State()
{
	m_bIsAttack = false;
}

STATE CState_Avoid::Key_Input(const _float& fTimeDelta)
{
	return STATE();
}


CState_Avoid* CState_Avoid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Avoid* pInstance = new CState_Avoid(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Avoid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid::Free()
{
	__super::Free();
}
