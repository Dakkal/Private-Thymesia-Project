#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Dead_Urd.h"
#include "Transform.h"
#include "GameObject.h"
#include "LandObject.h"
#include "PartObject.h"
#include "BinBone.h"


#include "Boss_Urd.h"

CState_Dead_Urd::CState_Dead_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Dead_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Dead_Urd::Tick(const _float& fTimeDelta)
{
	

	STATE eState = m_eState;

	if (false == m_bExcuted && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(100))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);
		m_pOwnerBodyPart->Set_CurAnimSpeed(0.5f);
	}
	else if (false == m_bExcuted && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(115))
	{
		m_pOwnerBodyPart->Set_CurAnimSpeed(1.5f);
	}

	if (false == m_bExcuted && m_pOwnerBodyPart->Is_AnimOverKeyFrame(220))
	{
		m_bExcuted = true;

		m_pOwnerBodyPart->Set_AnimationIndex(false, 10, 1.2f);
	}


	if (true == m_bExcuted && m_pOwnerBodyPart->IsAnimationEnd())
	{
		m_pRealOwner->Set_Dead(true);
	}

	return eState;
}

STATE CState_Dead_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Dead_Urd::Reset_State()
{
	m_bExcuted = false;
}

void CState_Dead_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 41, 1.5f);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayer = pGameInstance->Get_Player(LEVEL_GAMEPLAY);
	m_pPlayerAnimBone = dynamic_cast<CPartObject*>(pPlayer->Get_Index_Parts(CGameObject::PARTS::BODY))->Get_SocketBonePtr("AnimTargetPoint");
	_matrix matSocket = dynamic_cast<CPartObject*>(pPlayer->Get_Index_Parts(CGameObject::PARTS::BODY))->Get_SocketPivotMatrix();

	XMMATRIX	WorldMatrix = m_pPlayerAnimBone->Get_CombinedTransform() * matSocket;

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	_matrix BoneWorldMatrix = WorldMatrix;
	_matrix PlayerWorldMatrix = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform()->Get_WorldMatrix();
	_matrix CalculateMatrix = BoneWorldMatrix * PlayerWorldMatrix;

	_vector vPos;
	memcpy(&vPos, CalculateMatrix.m[3], sizeof(_vector));

	m_pOwnerTransform->Set_State(CTransform::STATE_POS, vPos);


	for (auto iter = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_Projectiles()->begin(); 
		iter != dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_Projectiles()->end(); ++iter)
	{
		(*iter)->Set_Active(false);
	}


	RELEASE_INSTANCE(CGameInstance);

}

STATE CState_Dead_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Dead_Urd* CState_Dead_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Dead_Urd* pInstance = new CState_Dead_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Dead_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Dead_Urd::Free()
{
	__super::Free();
}
