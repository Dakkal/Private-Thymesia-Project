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
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

    _long MouseMove = 0;

    /* Move */
    if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
    {
        m_pTransform->Go_Left(fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
    {
        m_pTransform->Go_Right(fTimeDelta);
    }
    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::WHEEL))
    {
        m_pTransform->Go_Up(m_fMouseSensitive * MouseMove * fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
    {
        m_pTransform->Go_Forward(fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
    {
        m_pTransform->Go_Backward(fTimeDelta);
    }

    /* Rotation */
    if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_LOOK), fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
    {
        m_pTransform->Turn_Invert(m_pTransform->Get_State(CTransform::STATE_LOOK), fTimeDelta);
    }
    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::X))
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_UP), m_fMouseSensitive * MouseMove * fTimeDelta);
    }
    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::Y))
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), m_fMouseSensitive * MouseMove * fTimeDelta);
    }

    RELEASE_INSTANCE(CGameInstance)

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
