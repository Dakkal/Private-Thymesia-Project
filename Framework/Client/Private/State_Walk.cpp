#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Walk.h"
#include "LandObject.h"

CState_Walk::CState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Walk::Initialize()
{
	__super::Initialize();

	Set_KeyVector();

	return S_OK;
}

STATE CState_Walk::Tick(const _float& fTimeDelta)
{
	STATE eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::AVOID;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::PARRY;
	}
	else if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::ATTACK;
	}
	else if (pGameInstance->Key_Down(VK_LSHIFT))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_WALK;
	}

	if (true == pGameInstance->Is_MouseMove())
	{
		m_bIsMouseMove = true;
		m_fMouseMoveTime = 0.f;
	}

	if (true == m_bIsMouseMove)
	{
		eState = Mouse_Move(fTimeDelta);
		
	}
	else
	{
		eState = Key_Input(fTimeDelta);
	}
	
	
	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Walk::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;
	
	if (true == m_bIsMouseMove)
	{
		m_fMouseMoveTime += fTimeDelta;

		if (m_fMouseMoveTime >= 0.45f)
		{
			m_fMouseMoveTime = 0.f;
			m_bIsMouseMove = false;
			Set_KeyVector();
		}
	}

	return eState;
}

void CState_Walk::Enter_State()
{
	dynamic_cast<CPlayer*>(m_pRealOwner)->Reset_TargetEnemy();

	m_pOwnerBodyPart->Set_AnimationIndex(true, 124, 1.2f);
}

void CState_Walk::Reset_State()
{
	if (true == m_bIsMouseMove)
		Set_KeyVector();

	m_fMouseMoveTime = 0.f;
	m_bIsMouseMove = false;
}

STATE CState_Walk::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
	m_bIsClick = false;
	m_vDir = XMVectorZero();
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_vDir += m_vKeyVector[KEY_W];
		m_bIsClick = true;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_vDir += m_vKeyVector[KEY_S];
		m_bIsClick = true;	
	}
	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_vDir += m_vKeyVector[KEY_A];
		m_bIsClick = true;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_vDir += m_vKeyVector[KEY_D];
		m_bIsClick = true;
	}


	if (true == m_bIsClick)
	{
		_float fRotMax = 66.f;
		_float fRotNormal = 20.f;

		_vector RotDir;

		if (3.f < acosf(m_vDir.Dot(vLook)))
		{
			RotDir = _vector::Lerp(vLook, m_vDir, fRotMax);
		}
		else
		{
			RotDir = _vector::Lerp(vLook, m_vDir, fRotNormal * fTimeDelta);
		}

		m_pOwnerTransform->Set_Look(RotDir);
		m_pOwnerTransform->Go_Forward(fTimeDelta, dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom());
		dynamic_cast<CLandObject*>(m_pRealOwner)->Set_On_NaviMesh(m_pOwnerTransform);
		

		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}

	RELEASE_INSTANCE(CGameInstance);
	return STATE::IDLE;
}

STATE CState_Walk::Mouse_Move(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_bIsClick = false;
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_bIsClick = true;
	}

	if (true == m_bIsClick)
	{
		_vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vDir = pGameInstance->Get_CamLook_Vector();
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
		m_pOwnerTransform->Go_Forward(fTimeDelta, dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom());
		dynamic_cast<CLandObject*>(m_pRealOwner)->Set_On_NaviMesh(m_pOwnerTransform);

		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}
	

	RELEASE_INSTANCE(CGameInstance);
	return STATE::IDLE;
}

void CState_Walk::Set_KeyVector()
{
	_vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
	m_vKeyVector[KEY_W] = vLook;

	_matrix matY = _matrix::CreateRotationY(XMConvertToRadians(90.f));
	m_vKeyVector[KEY_D] = XMVector3TransformNormal(m_vKeyVector[KEY_W], matY);
	m_vKeyVector[KEY_S] = XMVector3TransformNormal(m_vKeyVector[KEY_D], matY);
	m_vKeyVector[KEY_A] = XMVector3TransformNormal(m_vKeyVector[KEY_S], matY);
}

CState_Walk* CState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Walk* pInstance = new CState_Walk(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Walk::Free()
{
	__super::Free();
}
