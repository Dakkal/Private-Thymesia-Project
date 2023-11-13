#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle.h"
#include "Input_Device.h"

CState_Idle::CState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle::Tick(const _float& fTimeDelta)
{
	STATE eState = Key_Input(fTimeDelta);

	return eState;
}

STATE CState_Idle::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	

	return eState;
}

void CState_Idle::Reset_State()
{
	m_bEnter = false;
}

void CState_Idle::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CPlayer*>(m_pRealOwner)->Reset_TargetEnemy();

	m_pOwnerBodyPart->Set_AnimationIndex(true, 90, 1.2f);
}

STATE CState_Idle::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::AVOID;
	}
	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::ATTACK;
	}
	if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::PARRY;
	}
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}
	if (pGameInstance->Key_Down(VK_LSHIFT))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_IDLE;
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

CState_Idle* CState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle* pInstance = new CState_Idle(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle::Free()
{
	__super::Free();
}
