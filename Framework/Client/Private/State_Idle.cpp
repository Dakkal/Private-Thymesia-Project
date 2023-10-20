#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle.h"

CState_Idle::CState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle::Initialize()
{
	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pStateOwner->Get_Owner()->Get_Component(TEXT("Com_Transform")));
	CGameObject* pOwnerBodyPart = dynamic_cast<CPlayer*>(m_pStateOwner->Get_Owner())->Get_Parts(CPlayer::PARTS::BODY);
	m_pOwnerBodyPart = dynamic_cast<CPartObject*>(pOwnerBodyPart);
	m_pOwnerBodyPart->First_Set_AnimationIndex(true, 92);
	m_bEnter = true;
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
	m_pOwnerBodyPart->Set_AnimationIndex(true, 92);
}

STATE CState_Idle::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}
	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		//m_pOwnerBodyPart->Set_AnimationIndex(false, 126);
		m_pOwnerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}
	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		//m_pOwnerBodyPart->Set_AnimationIndex(false, 126);
		m_pOwnerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
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
