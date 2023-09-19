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
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), g_iWinSizeX / (_float)g_iWinSizeY, 0.2f, 1000.f));
	return S_OK;
}

HRESULT CGameCamera::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CGameCamera::Tick(_float fTimeDelta)
{
    /* Move */
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        m_pTransformCom->Go_Up(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransformCom->Go_Down(fTimeDelta);
    }
    if (GetAsyncKeyState('W') & 0x8000)
    {
        m_pTransformCom->Go_Forward(fTimeDelta);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }

    /* Rotation */
    if (GetAsyncKeyState('D') & 0x8000)
    {
        m_pTransformCom->Turn(AXIS::Y, fTimeDelta);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        m_pTransformCom->Turn_Invert(AXIS::Y, fTimeDelta);
    }
    if (GetAsyncKeyState('E') & 0x8000)
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);
    }
    if (GetAsyncKeyState('Q') & 0x8000)
    {
        m_pTransformCom->Turn_Invert(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);
    }
    if (GetAsyncKeyState('X') & 0x8000)
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta);
    }
    if (GetAsyncKeyState('Z') & 0x8000)
    {
        m_pTransformCom->Turn_Invert(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta);
    }
}

void CGameCamera::LateTick(_float fTimeDelta)
{

   m_ViewMatrix = m_pTransformCom->Get_WorldMatrix_Inverse();
}

HRESULT CGameCamera::Ready_Components()
{
	/* Com_Transform */
    CTransform::TRANSFORM_DESC      tTransformDesc;
    ZeroMemory(&tTransformDesc, sizeof(tTransformDesc));

    tTransformDesc.fSpeedPerSec = 50.f;
    tTransformDesc.fRotRadianPerSec = XMConvertToRadians(30.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tTransformDesc)))
        return E_FAIL;

	return S_OK;
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

    Safe_Release(m_pTransformCom);
}
