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
	__super::Initialize();

	return S_OK;
}

STATE CState_Avoid::Tick(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pOwnerBodyPart->IsAnimationEnd())
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::WALK;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::IDLE;
		}
		
	}
	else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::PARRY;
		}
		if (true == m_bIsAttack)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::ATTACK;
		}
			
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
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
	m_pRealOwner->Set_Move(true);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 51, 2.f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	pGameInstance->PlaySoundFile(TEXT("WhooshClothSoft_01_01.ogg"), CHANNELID::CHANNEL_1, 1.f);

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
