#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_ParrySuccess.h"
#include "Input_Device.h"
#include "Player.h"

#include "ParrySpark.h"

CState_ParrySuccess::CState_ParrySuccess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_ParrySuccess::Initialize()
{
	__super::Initialize();

	m_bParryLeft = true;

	return S_OK;
}

STATE CState_ParrySuccess::Tick(const _float& fTimeDelta)
{
	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(10) && true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	STATE eState = Key_Input(fTimeDelta);

	return eState;
}

STATE CState_ParrySuccess::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_IsParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(2))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffectObject::EFFECT_DESC	effectdesc;
		effectdesc.pTargetTransform = m_pOwnerTransform;
		effectdesc.vTargetPos = m_pOwnerWeaponRPart->Get_Collide_Center();
		effectdesc.vRotFactorX = _float2(-15.f, 15.f);

		if (true == m_bParryLeftUp)
			effectdesc.vRotFactorZ = _float2(60.f, 80.f);

		else if (true == m_bParryLeftDown)
			effectdesc.vRotFactorZ = _float2(100.f, 120.f);

		else if (true == m_bParryRightUp)
			effectdesc.vRotFactorZ = _float2(-60.f, -80.f);

		else if (true == m_bParryRightDown)
			effectdesc.vRotFactorZ = _float2(-100.f, -120.f);


		effectdesc.eEffectType = CEffectObject::TYPE::RECT;

		pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_EFFECT, TEXT("Prototype_GameObject_ParrySpark"), &effectdesc);

		RELEASE_INSTANCE(CGameInstance);

		m_IsParry = false;
	}


	return eState;
}

void CState_ParrySuccess::Reset_State()
{
	m_IsParry = false;
	m_bParryLeftUp = false;
	m_bParryRightUp = false;
	m_bParryLeftDown = false;
	m_bParryRightDown = false;
}

void CState_ParrySuccess::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bParryLeft)
	{
		m_bParryLeft = false;

		if (true == pGameInstance->Random_Coin(0.5f))
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 111, 3.5f);
		}
		else
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 113, 3.5f);
		}

		m_bParryRight = true;
	}
	else if (true == m_bParryRight)
	{
		m_bParryRight = false;

		if (true == pGameInstance->Random_Coin(0.5f))
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 115, 3.5f);
		}
		else
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 116, 3.5f);
		}

		m_bParryLeft = true;
	}


	pGameInstance->PlaySoundFile(TEXT("ParrySuc_03_01.ogg"), CHANNELID::CHANNEL_2, 1.f);


	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_ParrySuccess::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(10))
	{
		if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_AVOID;
		}
		if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_ATTACK;
		}
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_WALK;
		}
	}
	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(2))
	{
		if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_PARRY;
		}
	}

	if (true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_IDLE;
	}


	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

CState_ParrySuccess* CState_ParrySuccess::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_ParrySuccess* pInstance = new CState_ParrySuccess(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_ParrySuccess");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_ParrySuccess::Free()
{
	__super::Free();
}