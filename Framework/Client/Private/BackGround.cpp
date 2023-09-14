#include "pch.h"
#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    m_fSizeX = 100;
    m_fSizeY = 100;
    m_fX = g_iWinSizeX * 0.5;
    m_fY = g_iWinSizeY * 0.5;

    m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POS
        , XMVectorSet(m_fX - g_iWinSizeX * 0.5, -m_fY + g_iWinSizeY * 0.5, 0.f, 1.f));

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(90.f, g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f));
    //XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CBackGround::Tick(_float fTimeDelta)
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
    if (GetAsyncKeyState('Q') & 0x8000)
    {
        m_pTransformCom->Turn(AXIS::Y,fTimeDelta);
    }
    if (GetAsyncKeyState('E') & 0x8000)
    {
        m_pTransformCom->Turn_Invert(AXIS::Y, fTimeDelta);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        m_pTransformCom->Turn(AXIS::Z, fTimeDelta);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        m_pTransformCom->Turn_Invert(AXIS::Z, fTimeDelta);
    }
    if (GetAsyncKeyState('Z') & 0x8000)
    {
        m_pTransformCom->Turn(AXIS::X, fTimeDelta);
    }
    if (GetAsyncKeyState('X') & 0x8000)
    {
        m_pTransformCom->Turn_Invert(AXIS::X, fTimeDelta);
    }
    
}

void CBackGround::LateTick(_float fTimeDelta)
{

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_UI, this);
}

HRESULT CBackGround::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBackGround::Ready_Component(void* pArg)
{
    /* Com.Renderer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
        , TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    /* Com.Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex")
        , TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com.VIBuffer*/
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect")
        , TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture*/
    if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Transform */
    CTransform::TRANSFORM_DESC      tTransformDesc;
    ZeroMemory(&tTransformDesc, sizeof(tTransformDesc));

    tTransformDesc.fSpeedPerSec = 5.f;
    tTransformDesc.fRotRadianPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tTransformDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderReSources(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;


    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Textures")))
        return E_FAIL;


    return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBackGround* pInstance = new CBackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pRendererCom);
}
