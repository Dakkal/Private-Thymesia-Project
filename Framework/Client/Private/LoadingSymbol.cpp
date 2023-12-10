#include "pch.h"
#include "LoadingSymbol.h"
#include "GameInstance.h"

CLoadingSymbol::CLoadingSymbol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CLoadingSymbol::CLoadingSymbol(const CLoadingSymbol& rhs)
    : CGameObject(rhs)
{
}

HRESULT CLoadingSymbol::Initialize_Prototype(const wstring& strProtoTag)
{
    m_strObjectName = TEXT("Object_Loading");

    return S_OK;
}

HRESULT CLoadingSymbol::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    m_fSizeX = 214 * 0.2f;
    m_fSizeY = 592 * 0.2f;
    m_fX = g_iWinSizeX * 0.5f;
    m_fY = g_iWinSizeY * 0.5f;

    m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POS,
        _vector(m_fX - g_iWinSizeX * 0.5f + 550.f, -m_fY + g_iWinSizeY * 0.5f - 270.f, 0.f, 1.f));

    m_ViewMatrix = XMMatrixIdentity();
    m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

    return S_OK;
}

void CLoadingSymbol::Tick(_float fTimeDelta)
{
    m_fTime += 0.5f * fTimeDelta;

    if (true == m_IsAlpha)
        m_fAlpha = _vector::Lerp(_vector(0.0f, 0.0f, 0.0f, 0.0f), _vector(1.0f, 1.0f, 1.0f, 1.0f), m_fTime).x;
    if (false == m_IsAlpha)
        m_fAlpha = _vector::Lerp(_vector(1.0f, 1.0f, 1.0f, 1.0f), _vector(0.0f, 0.0f, 0.0f, 0.0f), m_fTime).x;

    if (1.f <= m_fAlpha)
    {
        m_IsAlpha = false;
        m_fTime = 0.f;
    }
    else if (0.f >= m_fAlpha)
    {
        m_IsAlpha = true;
        m_fTime = 0.f;
    }
}

void CLoadingSymbol::LateTick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_UI, this);
}

HRESULT CLoadingSymbol::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CLoadingSymbol::Ready_Component(void* pArg)
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Symbol"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingSymbol::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CLoadingSymbol* CLoadingSymbol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
    CLoadingSymbol* pInstance = new CLoadingSymbol(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
    {
        MSG_BOX("Failed to Created : CLoadingSymbol");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoadingSymbol::Clone(void* pArg)
{
    __super::Clone(pArg);

    CLoadingSymbol* pInstance = new CLoadingSymbol(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CLoadingSymbol");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoadingSymbol::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pRendererCom);
}
