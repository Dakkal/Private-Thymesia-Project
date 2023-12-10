#include "pch.h"
#include "Loading.h"
#include "GameInstance.h"

CLoading::CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CLoading::CLoading(const CLoading& rhs)
    : CGameObject(rhs)
{
}

HRESULT CLoading::Initialize_Prototype(const wstring& strProtoTag)
{
    m_strObjectName = TEXT("Object_Loading");

    return S_OK;
}

HRESULT CLoading::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    m_fSizeX = g_iWinSizeX;
    m_fSizeY = g_iWinSizeY;
    m_fX = g_iWinSizeX * 0.5f;
    m_fY = g_iWinSizeY * 0.5f;

    m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POS,
        _vector(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    m_ViewMatrix = XMMatrixIdentity();
    m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

    return S_OK;
}

void CLoading::Tick(_float fTimeDelta)
{

}

void CLoading::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_UI, this);
}

HRESULT CLoading::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);
    m_pVIBufferCom->Render();

   /* CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    wstring strLogo = TEXT("Enter키를 눌러 게임 시작");

    pGameInstance->Render_Font(TEXT("Font_Default"), strLogo, _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f + 130.f), _vector(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.6f);

    RELEASE_INSTANCE(CGameInstance);*/


    return S_OK;
}

HRESULT CLoading::Ready_Component(void* pArg)
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
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

HRESULT CLoading::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CLoading* CLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
    CLoading* pInstance = new CLoading(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
    {
        MSG_BOX("Failed to Created : CLoading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading::Clone(void* pArg)
{
    __super::Clone(pArg);

    CLoading* pInstance = new CLoading(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CLoading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pRendererCom);
}
