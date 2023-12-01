#pragma once
#include "Client_Defines.h"
#include "EffectObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParrySpark final : public CEffectObject
{
private:
    CParrySpark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CParrySpark(const CParrySpark& rhs);
    virtual ~CParrySpark() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring & strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void PriorityTick(_float fTimeDelta);
    virtual void Tick(_float fTimeDelta);
    virtual void LateTick(_float fTimeDelta);
    virtual HRESULT Render();

private:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();

public:
    static  CParrySpark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

END