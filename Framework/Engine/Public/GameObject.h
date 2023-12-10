#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
    enum PARTS { BODY, WEAPON_R, WEAPON_L, HITBOX, CAMERA, _END };

protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject& rhs);
    virtual ~CGameObject() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void Enter_Object();
    virtual void PriorityTick(_float fTimeDelta);
    virtual void Tick(_float fTimeDelta);
    virtual void LateTick(_float fTimeDelta);
    virtual void Exit_Object();
    virtual HRESULT Render();
    virtual HRESULT Render_LightDepth() { return S_OK; }

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
    const _bool&          Is_Active()  { return m_IsActive; }

    const _bool&          Is_Remain()       { return m_IsRemain; }
    const _bool&          Is_Cloned()       { return m_IsCloned; }
    const _bool&          Is_Dead()         { return m_IsDead; }
    const _bool&          Is_Hit()          { return m_IsHit; }
    const _bool&          Is_Parry()        { return m_IsParry; }
    const _bool&          Is_Attack()       { return m_IsAttack; }
    const _bool&          Is_SubAttack()    { return m_IsAttack; }
    const _bool&          Is_OktoHit()      { return m_bOktoHit; }
    const _bool&          Is_Move()         { return m_IsMove; }
    const _bool&          Is_Cull()         { return m_IsCull; }
    const _bool&          Is_Parried()      { return m_IsParried; }

public:
    void            Set_Active(_bool IsActive)      { m_IsActive = IsActive; }
    void            Set_Index(_uint iIndex)         { m_iIndex = iIndex; }

    void            Set_Dead(_bool IsDead)          { m_IsDead = IsDead; }
    void            Set_Hit(_bool IsHit)            { m_IsHit = IsHit; }
    void            Set_Parry(_bool IsParry)        { m_IsParry = IsParry; }

    void            Set_Attack(_bool IsAttack)      { m_IsAttack = IsAttack; }
    void            Set_SubAttack(_bool IsSubAttack) { m_IsSubAttack = IsSubAttack; }

    void            Set_OktoHit(_bool OktoHit)      { m_bOktoHit = OktoHit; }
    void            Set_Move(_bool IsMove)          { m_IsMove = IsMove; }
    void            Set_Parried(_bool IsParried)    { m_IsParried = IsParried; }

public:
    const _uint&    Get_HP() const              { return m_iHp; }
    void            Subtract_HP()               { --m_iHp; }

public:
    wstring         Get_Name()                    { return m_strObjectName; }
    wstring         Get_ProtoTag()                { return m_strProtoTag; }
    OBJECT_TYPE     Get_ObjectType()              { return m_eObjType; }

    _uint           Get_Index()                   { return m_iIndex; }
    CGameObject*    Get_Index_Parts(_uint iParts) { return m_Parts[iParts]; }

    auto&           Get_Parts() const             { return m_Parts; }
    _uint           Get_Parts_Size()              { return m_Parts.size(); }

    const _float&   Get_DissolveTime()            { return m_fDissolveTime; }
    const _float&   Get_DissolveDuration()        { return m_fDissolveDuraton; }


public:
    void            Reset_CloneIndex() { m_iCloneIndex = 0; }

protected:
    map<_uint, class CGameObject*>		m_Parts;

protected:
    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };

    map<const wstring, class CComponent*>   m_Components;

protected:
    _bool       m_IsActive = { true };

    _bool	    m_IsCloned = { false };
    _bool       m_IsRemain = { false };
    _bool       m_IsDead = { false };

    _bool       m_IsAttack = { false };
    _bool       m_IsSubAttack = { false };

    _bool       m_IsHit = { false };
    _bool       m_IsMove = { false };
    _bool       m_bOktoHit = { false };
    _bool       m_IsCull = { false };
    _bool       m_IsParry = { false };
    _bool       m_IsParried = { false };

    OBJECT_TYPE m_eObjType = OBJECT_TYPE::_END;
    wstring     m_strObjectName = { TEXT("") };
    wstring     m_strProtoTag = { TEXT("") };

    _uint       m_iIndex = { 0 };
    _uint       m_iCloneIndex = { 0 };

    _float      m_fDissolveTime = { 0.f };
    _float      m_fDissolveDuraton = { 0.f };
    _bool       m_bDissolveSound = { false };

    _uint       m_iHp = { 10 };

protected:
    HRESULT Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
    class CComponent* Find_Component(const wstring & strComponentTag);


public:
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END