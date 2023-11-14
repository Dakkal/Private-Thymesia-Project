#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
    enum PARTS { BODY, WEAPON_R, WEAPON_L, SIGHT, HITBOX, _END };

protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject& rhs);
    virtual ~CGameObject() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void PriorityTick(_float fTimeDelta);
    virtual void Tick(_float fTimeDelta);
    virtual void LateTick(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual void	OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta) {};
    virtual void	OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta) {};
    virtual void	OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta) {};
    virtual void	OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta) {};
    virtual void	OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta) {};
    virtual void	OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta) {};

public:
    class  CComponent* Get_Component(const wstring& strComponentTag) { return Find_Component(strComponentTag); }

public:
    _bool           Is_Remain()  { return m_IsRemain; }
    _bool           Is_Cloned()  { return m_IsCloned; }
    _bool           Is_Dead()    { return m_IsDead; }
    _bool           Is_Hit()     { return m_IsHit; }
    _bool           Is_Attack()  { return m_IsAttack; }
    _bool           Is_OktoHit() { return m_bOktoHit; }
    _bool           Is_Move()    { return m_IsMove; }
    _bool           Is_Cull()    { return m_IsCull; }

public:
    void            Set_Index(_uint iIndex)     { m_iIndex = iIndex; }
    void            Set_Hit(_bool IsHit)        { m_IsHit = IsHit; }
    void            Set_Attack(_bool IsAttack)  { m_IsAttack = IsAttack; }
    void            Set_OktoHit(_bool OktoHit)  { m_bOktoHit = OktoHit; }
    void            Set_Move(_bool IsMove)      { m_IsMove = IsMove; }

public:
    wstring         Get_Name()       { return m_strObjectName; }
    wstring         Get_ProtoTag()   { return m_strProtoTag; }
    OBJECT_TYPE     Get_ObjectType() { return m_eObjType; }

    _uint           Get_Index() { return m_iIndex; }
    CGameObject*    Get_Index_Parts(_uint iParts) { return m_Parts[iParts]; }

    auto&           Get_Parts() const { return m_Parts; }
    _uint           Get_Parts_Size()  { return m_Parts.size(); }




public:
    void            Reset_CloneIndex() { m_iCloneIndex = 0; }

protected:
    map<_uint, class CGameObject*>		m_Parts;

protected:
    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };

    map<const wstring, class CComponent*>   m_Components;

protected:
    _bool	    m_IsCloned = { false };
    _bool       m_IsRemain = { false };
    _bool       m_IsDead = { false };
    _bool       m_IsAttack = { false };
    _bool       m_IsHit = { false };
    _bool       m_IsMove = { false };
    _bool       m_bOktoHit = { false };
   
    OBJECT_TYPE m_eObjType = OBJECT_TYPE::_END;
    wstring     m_strObjectName = { TEXT("") };
    wstring     m_strProtoTag = { TEXT("") };

    _uint       m_iIndex = { 0 };
    _uint       m_iCloneIndex = { 0 };

    _bool       m_IsCull = { false };

protected:
    HRESULT Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
    class CComponent* Find_Component(const wstring & strComponentTag);


public:
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END