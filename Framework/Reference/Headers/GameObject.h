#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject& rhs);
    virtual ~CGameObject() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void LateTick(_float fTimeDelta);
    virtual HRESULT Render();

public:
    _bool           Is_Remain() { return m_IsRemain; }
    _bool           Is_Cloned() { return m_IsCloned; }
    _bool           Is_Dead()   { return m_IsDead; }

    wstring         Get_Name() { return m_strObjectName; }
    wstring         Get_ProtoTag() { return m_strProtoTag; }
    OBJECT_TYPE     Get_ObjectType() { return m_eObjType; }

    _uint           Get_Index() { return m_iIndex; }
    void            Set_Index(const _uint& iIndex) { m_iIndex = iIndex; }

    void            Reset_CloneIndex() { m_iCloneIndex = 0; }

public:
    class  CComponent* Get_Component(const wstring& strComponentTag) { return Find_Component(strComponentTag); }

protected:
    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };

protected:
    map<const wstring, class CComponent*>   m_Components;

    _bool	    m_IsCloned = { false };
    _bool       m_IsRemain = { false };
    _bool       m_IsDead = { false };
   
    OBJECT_TYPE m_eObjType = OBJECT_TYPE::_END;
    wstring     m_strObjectName = { TEXT("") };
    wstring     m_strProtoTag = { TEXT("") };


    _uint       m_iIndex = { 0 };
    _uint       m_iCloneIndex = { 0 };

protected:
    HRESULT Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
    class CComponent* Find_Component(const wstring & strComponentTag);


public:
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END