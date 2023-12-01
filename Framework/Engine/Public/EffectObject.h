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
        _vector vTargetPos;

    }EFFECT_DESC;

protected:
    CEffectObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    CEffectObject(const CEffectObject& rhs);
    virtual ~CEffectObject() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring & strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void PriorityTick(_float fTimeDelta);
    virtual void Tick(_float fTimeDelta);
    virtual void LateTick(_float fTimeDelta);
    virtual HRESULT Render();

public:
    HRESULT Use_Effect();
    void    Set_TargetPos(_vector vPos) { m_vTargetPos = vPos; }
    const   _bool& Is_Update(TYPE eType);

protected: 
    _vector m_vTargetPos;

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