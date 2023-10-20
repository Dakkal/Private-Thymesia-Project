#include "pch.h"
#include "..\Public\ToolCamera.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"

CToolCamera::CToolCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CToolCamera::CToolCamera(const CToolCamera& rhs)
	: CCamera(rhs)
{
}


HRESULT CToolCamera::Initialize_Prototype(const wstring& strProtoTag)
{
    m_strObjectName = TEXT("Object_ToolCamera");

	return S_OK;
}

HRESULT CToolCamera::Initialize(void* pArg)
{
    CAMERA_TOOL_DESC* pCamera_Tool_Desc = (CAMERA_TOOL_DESC*)pArg;

    m_fMouseSensitive = pCamera_Tool_Desc->fMouseSensitive;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	return S_OK;
}

void CToolCamera::Tick(_float fTimeDelta)
{
    MouseMove(fTimeDelta);

    __super::Tick(fTimeDelta);
}

void CToolCamera::LateTick(_float fTimeDelta)
{

}

void CToolCamera::MouseMove(_float fTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    if (LEVELID::LEVEL_EDIT == pGameInstance->Get_CurLevel())
    {
        CImgui_Manager* pImgui = GET_INSTANCE(CImgui_Manager);
        if (true == pImgui->Is_MouseClickedGUI())
        {
            RELEASE_INSTANCE(CGameInstance);
            RELEASE_INSTANCE(CImgui_Manager);
            return;
        }
        RELEASE_INSTANCE(CImgui_Manager);
    }

    _long MouseMove = 0;

    /* Move */
    if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
    {
        m_pTransform->Go_Left(fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
    {
        m_pTransform->Go_Right(fTimeDelta);
    }
    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::WHEEL))
    {
        m_pTransform->Go_Up(m_fMouseSensitive * MouseMove * fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
    {
        m_pTransform->Go_Forward(fTimeDelta);
    }
    if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
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
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_LOOK), -fTimeDelta);
    }
    if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::RBUTTON))
    {
        if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::X))
        {
            m_pTransform->Turn(AXIS::Y, m_fMouseSensitive * MouseMove * fTimeDelta);
        }
        if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::Y))
        {
            m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), m_fMouseSensitive * MouseMove * fTimeDelta);
        }
    }
  

    RELEASE_INSTANCE(CGameInstance)
}

CToolCamera* CToolCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CToolCamera* pInstance = new CToolCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CToolCamera* CToolCamera::Clone(void* pArg)
{
	CToolCamera* pInstance = new CToolCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolCamera::Free()
{
	__super::Free();

}
