#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CEffectObject abstract : public CGameObject
{
public:
    enum TYPE
    {
        POINT, RECT, _END
    };

public:
    typedef struct tagEffectDesc
    {
        TYPE    eEffectType;

        _float2 vRotFactorX = { 0.f, 0.f };
        _float2 vRotFactorY = { 0.f, 0.f };
        _float2 vRotFactorZ = { 0.f, 0.f };

        class CTransform* pTargetTransform = { nullptr };

        _vector vTargetPos;


    }EFFECT_DESC;

protected:
    CEffectObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    CEffectObject(const CEffectObject& rhs);
    virtual ~CEffectObject() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring & strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void    PriorityTick(_float fTimeDelta);
    virtual void    Tick(_float fTimeDelta);
    virtual void    LateTick(_float fTimeDelta);
    virtual HRESULT Render();

public:
    HRESULT         Use_Effect();
    void            Set_TargetPos(_vector vPos) { m_vTargetPos = vPos; }
    const   _bool&  Is_Update();

protected:
    virtual HRESULT Ready_Components();
    virtual HRESULT Bind_ShaderResources();

    virtual void    Create_RandomRotMatrix(_float2 vAxisX, _float2 vAxisY, _float2 vAxisZ);

protected: 
    _uint   m_iNumInstance = { 0 };
    _uint   m_iMaxNumInstance = { MAX_INTANCE };
    _matrix m_matRotMatrices[3][MAX_INTANCE];

    class CTransform* m_pTargetTransform = { nullptr };
    _vector m_vTargetPos;
    _float2 m_vRotFactorX;
    _float2 m_vRotFactorY;
    _float2 m_vRotFactorZ;
    TYPE    m_eEffectType = { TYPE::_END };


protected:
    class CRenderer*                  m_pRendererCom = { nullptr };
    class CTransform*                 m_pTransformCom = { nullptr };
    class CShader*                    m_pShaderCom = { nullptr };
    class CTexture*                   m_pTextureCom = { nullptr };
    class CVIBuffer_Point_Instance*   m_pVIBufferCom = { nullptr };
    class CVIBuffer_Rect_Instance*    m_pVIRectBufferCom = { nullptr };

public:
    virtual CGameObject* Clone(void* pArg) PURE;
    virtual void Free() override;
};

END