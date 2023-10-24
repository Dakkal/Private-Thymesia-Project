#include "pch.h"
#include "..\Public\PlayerCamera.h"
#include "GameInstance.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
    : CCamera(rhs)
{
}


HRESULT CPlayerCamera::Initialize_Prototype(const wstring& strProtoTag)
{
    m_strObjectName = TEXT("Object_PlayerCamera");

    return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
    CAMERA_PLAYER_DESC* pCamera_Tool_Desc = (CAMERA_PLAYER_DESC*)pArg;

    m_fMouseSensitive = pCamera_Tool_Desc->fMouseSensitive;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Last_GameObject(LEVEL_GAMEPLAY, LAYER_PLAYER)->Get_Component(TEXT("Com_Transform")));

    m_vEye = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
    m_vAt = m_pPlayerTransform->Get_State(CTransform::STATE_POS);

    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

void CPlayerCamera::Tick(_float fTimeDelta)
{
    MouseMove(fTimeDelta);

    __super::Tick(fTimeDelta);
}

void CPlayerCamera::LateTick(_float fTimeDelta)
{
    _matrix matX = _matrix::CreateRotationX(XMConvertToRadians(m_vAngle.x));
    _matrix matY = _matrix::CreateRotationY(XMConvertToRadians(m_vAngle.y));

    _vector vCamDir = { 0.f, 1.f, -1.f, 0.f };

    vCamDir = XMVector3TransformNormal(vCamDir, matX);
    vCamDir = XMVector3TransformNormal(vCamDir, matY);
    _vector vCamOffset = vCamDir * m_fOffsetDis;
    vCamOffset.y += 3.f;

    _vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
    _vector vOffsetPos = vPlayerPos + vCamOffset;

    m_pTransform->Set_State(CTransform::STATE_POS, XMVectorLerp(m_pTransform->Get_State(CTransform::STATE_POS), vOffsetPos, 10.f * fTimeDelta));

    _vector vLookAt = vPlayerPos;
    vLookAt.y += 1.5f;
    m_pTransform->LookAt(vLookAt);


    __super::LateTick(fTimeDelta);
}

void CPlayerCamera::MouseMove(_float fTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    _long MouseMove = 0;

    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::Y))
    {
        m_vAngle.x += MouseMove * fTimeDelta * m_fMouseSensitive;
        if (360.f <= m_vAngle.x)
            m_vAngle.x = 0.f;
        else if (0.f >= m_vAngle.x)
            m_vAngle.x = 360.f;
    }

    if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE::X))
    {
        m_vAngle.y += MouseMove * fTimeDelta * m_fMouseSensitive;
        if (360.f <= m_vAngle.y)
            m_vAngle.y = 0.f;
        else if (0.f >= m_vAngle.y)
            m_vAngle.y = 360.f;
    }


    RELEASE_INSTANCE(CGameInstance)
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
    CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
    {
        MSG_BOX("Failed to Created : CPlayerCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CPlayerCamera* CPlayerCamera::Clone(void* pArg)
{
    CPlayerCamera* pInstance = new CPlayerCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayerCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerCamera::Free()
{
    __super::Free();

}
