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

    return S_OK;
}

void CBackGround::Tick(_float fTimeDelta)
{
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
    _float4x4   matIdentity;
    XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());

    _float4x4   matWorld = m_pTransformCom->Get_WorldMatrix();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matIdentity)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matIdentity)))
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
