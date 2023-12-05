#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Lockon_Walk.h"
#include "Input_Device.h"
#include "Player.h"
#include "Navigation.h"

CState_Lockon_Walk::CState_Lockon_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Lockon_Walk::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Lockon_Walk::Tick(const _float& fTimeDelta)
{
	CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
	if (nullptr == pTarget)
		return STATE::IDLE;

	if (true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	CComponent* pCom = pTarget->Get_Component(TEXT("Com_Transform"));
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(pCom);

	_vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vDir = pTargetTransform->Get_State(CTransform::STATE_POS) - m_pOwnerTransform->Get_State(CTransform::STATE_POS);
	vDir.Normalize();
	vDir.y = 0.f;

	_float fRotMax = 66.f;
	_float fRotNormal = 20.f;

	_vector RotDir;

	if (3.f < acosf(vDir.Dot(vLook)))
	{
		RotDir = _vector::Lerp(vLook, vDir, fRotMax);
	}
	else
	{
		RotDir = _vector::Lerp(vLook, vDir, fRotNormal * fTimeDelta);
	}
	m_pOwnerTransform->Set_Look(RotDir);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_AVOID;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_PARRY;
	}
	else if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_ATTACK;
	}
	else if (pGameInstance->Key_Down(VK_LSHIFT))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(1))
		pGameInstance->CheckPlaySoundFile(TEXT("FootStep_Leather_01.ogg"), CHANNELID::CHANNEL_1, 1.f);
	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(12))
		pGameInstance->CheckPlaySoundFile(TEXT("FootStep_Leather_02.ogg"), CHANNELID::CHANNEL_1, 1.f);

	
	RELEASE_INSTANCE(CGameInstance);

	STATE eState = Key_Input(fTimeDelta);

	return eState;
}

STATE CState_Lockon_Walk::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	

	return eState;
}

void CState_Lockon_Walk::Reset_State()
{
	m_bEnter = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->StopSound(CHANNELID::CHANNEL_1);

	RELEASE_INSTANCE(CGameInstance);
}

void CState_Lockon_Walk::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CPlayer*>(m_pRealOwner)->Search_TargetEnemy();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->PlaySoundFile(TEXT("FootStep_Leather_01.ogg"), CHANNELID::CHANNEL_1, 1.f);

	_uint iAnimIndex = 0;
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		iAnimIndex += KEY::W;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		iAnimIndex += KEY::S;
	}
	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		iAnimIndex += KEY::A;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		iAnimIndex += KEY::D;
	}

	if (KEY::W == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 124, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::S == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 121, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::A == iAnimIndex)
	{

		m_pOwnerBodyPart->Set_AnimationIndex(true, 127, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::D == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 128, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::W + KEY::A == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 125, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::W + KEY::D == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 126, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::S + KEY::A == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 122, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}
	else if (KEY::S + KEY::D == iAnimIndex)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 123, 1.2f);
		m_iAnimIndex = iAnimIndex;
	}

	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Lockon_Walk::Key_Input(const _float& fTimeDelta)
{
	

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CNavigation* pNavi = dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom();

	_bool bClicked = false;
	m_iCalculeIndex = 0;
	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pOwnerTransform->Go_Left(fTimeDelta, pNavi);
		m_iCalculeIndex += KEY::A;
		bClicked = true;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pOwnerTransform->Go_Right(fTimeDelta, pNavi);
		m_iCalculeIndex += KEY::D;
		bClicked = true;
	}
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pOwnerTransform->Go_Forward(fTimeDelta, pNavi);
		m_iCalculeIndex += KEY::W;
		bClicked = true;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pOwnerTransform->Go_Backward(fTimeDelta, pNavi);
		m_iCalculeIndex += KEY::S;
		bClicked = true;
	}
	

	if (true == bClicked)
	{
		_int fChangeKeyFrame = 12 - m_pOwnerBodyPart->Get_CurKeyFrameNumb();
		if (0 > fChangeKeyFrame)
			fChangeKeyFrame = abs(fChangeKeyFrame);

		if (false == m_pOwnerBodyPart->IsAnimChange() && m_iAnimIndex != m_iCalculeIndex)
		{
			if (KEY::W == m_iCalculeIndex)
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 124, 1.2f, fChangeKeyFrame);
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::S == m_iCalculeIndex)
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 121, 1.2f, fChangeKeyFrame);
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::A == m_iCalculeIndex)
			{
				if (KEY::D == m_iAnimIndex)
				{
					m_pOwnerBodyPart->Set_First_AnimationIndex(true, 127, 1.2f);
				}
				else
				{
					m_pOwnerBodyPart->Set_AnimationIndex(true, 127, 1.2f, fChangeKeyFrame);
				}
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::D == m_iCalculeIndex)
			{
				if (KEY::A == m_iAnimIndex)
				{
					m_pOwnerBodyPart->Set_First_AnimationIndex(true, 128, 1.2f);
				}
				else
				{
					m_pOwnerBodyPart->Set_AnimationIndex(true, 128, 1.2f, fChangeKeyFrame);
				}
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::W + KEY::A == m_iCalculeIndex)
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 125, 1.2f, fChangeKeyFrame);
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::W + KEY::D == m_iCalculeIndex)
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 126, 1.2f, fChangeKeyFrame);
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::S + KEY::A == m_iCalculeIndex)
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 122, 1.2f, fChangeKeyFrame);
				m_iAnimIndex = m_iCalculeIndex;
			}
			else if (KEY::S + KEY::D == m_iCalculeIndex)
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 123, 1.2f, fChangeKeyFrame);
				m_iAnimIndex = m_iCalculeIndex;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}
	

	RELEASE_INSTANCE(CGameInstance);
	return STATE::LOCK_IDLE;
}

CState_Lockon_Walk* CState_Lockon_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Lockon_Walk* pInstance = new CState_Lockon_Walk(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Lockon_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Lockon_Walk::Free()
{
	__super::Free();
}