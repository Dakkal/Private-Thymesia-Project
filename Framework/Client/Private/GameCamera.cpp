#include "pch.h"
#include "..\Public\GameCamera.h"
#include "GameInstance.h"

CGameCamera::CGameCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CGameCamera::CGameCamera(const CGameCamera& rhs)
	: CCamera(rhs)
{
}


HRESULT CGameCamera::Initialize_Prototype()
{
 
	return S_OK;
}

HRESULT CGameCamera::Initialize(void* pArg)
{
    CAMERA_GAME_DESC* pCamera_Game_Desc = (CAMERA_GAME_DESC*)pArg;

    m_fMouseSensitive = pCamera_Game_Desc->fMouseSensitive;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	return S_OK;
}

void CGameCamera::Tick(_float fTimeDelta)
{
    /* Move */
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        m_pTransform->Go_Left(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        m_pTransform->Go_Right(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        m_pTransform->Go_Up(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransform->Go_Down(fTimeDelta);
    }
    if (GetAsyncKeyState('W') & 0x8000)
    {
        m_pTransform->Go_Forward(fTimeDelta);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        m_pTransform->Go_Backward(fTimeDelta);
    }

    /* Rotation */
    if (GetAsyncKeyState('D') & 0x8000)
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), fTimeDelta);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        m_pTransform->Turn_Invert(m_pTransform->Get_State(CTransform::STATE_UP), fTimeDelta);
    }
    if (GetAsyncKeyState('E') & 0x8000)
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_LOOK), fTimeDelta);
    }
    if (GetAsyncKeyState('Q') & 0x8000)
    {
        m_pTransform->Turn_Invert(m_pTransform->Get_State(CTransform::STATE_LOOK), fTimeDelta);
    }
    if (GetAsyncKeyState('X') & 0x8000)
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta);
    }
    if (GetAsyncKeyState('Z') & 0x8000)
    {
        m_pTransform->Turn_Invert(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta);
    }

    __super::Tick(fTimeDelta);
}

void CGameCamera::LateTick(_float fTimeDelta)
{

   //m_ViewMatrix = m_pTransformCom->Get_WorldMatrix_Inverse();
}

CGameCamera* CGameCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameCamera* pInstance = new CGameCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameCamera* CGameCamera::Clone(void* pArg)
{
	CGameCamera* pInstance = new CGameCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameCamera::Free()
{
	__super::Free();

}
