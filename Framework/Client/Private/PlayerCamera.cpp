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

    RELEASE_INSTANCE(CGameInstance);

    m_fCamOffsetY[(_uint)CAMSET::FREE] = 2.5f;
    m_fCamOffsetY[(_uint)CAMSET::LOCK] = 3.0f;

    m_fOffsetY[(_uint)CAMSET::FREE] = 1.5f;
    m_fOffsetY[(_uint)CAMSET::LOCK] = 0.5f;


    return S_OK;
}

void CPlayerCamera::Enter_Object()
{
    _vector vCamDir;
    _vector vCamOffset;

    vCamDir = { 0.f, 1.f, -1.f, 0.f };
    vCamOffset = vCamDir * m_fOffsetDis;
    vCamOffset.y -= m_fCamOffsetY[(_uint)CAMSET::FREE];

    _vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
    _vector vOffsetPos = vPlayerPos + vCamOffset;

    _vector vCamPos = vOffsetPos;
    if (vPlayerPos.y + m_fPlayerLimitY >= vCamPos.y)
        vCamPos.y = vPlayerPos.y + m_fPlayerLimitY;

    m_pTransform->Set_State(CTransform::STATE_POS, vCamPos);

    vPlayerPos.y += m_fOffsetY[(_uint)CAMSET::FREE];
    m_vAt = vPlayerPos;
    m_pTransform->LookAt(m_vAt);
}

void CPlayerCamera::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if(nullptr == m_pTargetTransform)
        MouseMove(fTimeDelta);
}

void CPlayerCamera::LateTick(_float fTimeDelta)
{
    _matrix matX = _matrix::CreateRotationX(XMConvertToRadians(m_vAngle.x));
    _matrix matY = _matrix::CreateRotationY(XMConvertToRadians(m_vAngle.y));

    _vector vCamDir;
    _vector vCamOffset;

    if (nullptr == m_pTargetTransform)
    {
        vCamDir = { 0.f, 1.f, -1.f, 0.f };
        vCamDir = XMVector3TransformNormal(vCamDir, matX);
        vCamDir = XMVector3TransformNormal(vCamDir, matY);
        vCamOffset = vCamDir * m_fOffsetDis;
        vCamOffset.y -= m_fCamOffsetY[(_uint)CAMSET::FREE];

        _vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
        _vector vOffsetPos = vPlayerPos + vCamOffset;

        _vector vCamPos = _vector::Lerp(m_pTransform->Get_State(CTransform::STATE_POS), vOffsetPos, 10.f * fTimeDelta);
        if (vPlayerPos.y + m_fPlayerLimitY >= vCamPos.y)
            vCamPos.y = vPlayerPos.y + m_fPlayerLimitY;

        m_pTransform->Set_State(CTransform::STATE_POS, vCamPos);

        vPlayerPos.y += m_fOffsetY[(_uint)CAMSET::FREE];
        m_vAt = _vector::Lerp(m_vAt, vPlayerPos, 4.f * fTimeDelta);
        m_pTransform->LookAt(m_vAt);
    }
    else
    {
        _vector vPlayer_Pos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
        _vector vTarget_Pos = m_pTargetTransform->Get_State(CTransform::STATE_POS);

        _vector vCamDir = vTarget_Pos - vPlayer_Pos;
        vCamDir.Normalize();
        vCamDir.x *= -1.f; vCamDir.y = 1;  vCamDir.z *= -1.f;
        vCamOffset = vCamDir * m_fTargetOffsetDis;
        vCamOffset.y -= m_fCamOffsetY[(_uint)CAMSET::LOCK];

        _vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);
        _vector vOffsetPos = vPlayerPos + vCamOffset;
        if (vOffsetPos.y >= vPlayerPos.y + m_fCamOffsetY[(_uint)CAMSET::LOCK])
            vOffsetPos.y = vPlayerPos.y + m_fCamOffsetY[(_uint)CAMSET::LOCK];
      
        _vector vCamPos = _vector::Lerp(m_pTransform->Get_State(CTransform::STATE_POS), vOffsetPos, 4.f * fTimeDelta);
        if (vPlayer_Pos.y + m_fPlayerLimitY >= vCamPos.y)
            vCamPos.y = vPlayer_Pos.y + m_fPlayerLimitY;

        m_pTransform->Set_State(CTransform::STATE_POS, vCamPos);

        vTarget_Pos.y += m_fOffsetY[(_uint)CAMSET::LOCK];
        m_vAt = _vector::Lerp(m_vAt, vTarget_Pos, 4.f * fTimeDelta);
        m_pTransform->LookAt(m_vAt);
    }  

    if (false == g_BossSeq && false == g_PlayerSeq)
    {
        __super::LateTick(fTimeDelta);
    }
       
}

void CPlayerCamera::Exit_Object()
{
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
