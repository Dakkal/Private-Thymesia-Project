#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLandObject abstract : public CGameObject
{
protected:
    CLandObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;

public:
    virtual HRESULT Initialize_Prototype(const wstring & strProtoTag);
    virtual HRESULT Initialize(void* pArg);
    virtual void PriorityTick(_float fTimeDelta);
    virtual void Tick(_float fTimeDelta);
    virtual void LateTick(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual class CNavigation* Get_CurNaviCom() { return m_pCurNavigationCom; }
    virtual HRESULT    Set_CurNaviCom(class CNavigation* pNavi);
    virtual _bool      Find_NaviMesh(_vector vPos);

public:
    virtual HRESULT    Set_On_NaviMesh(class CTransform* pTransform);

protected:
    vector<class CNavigation*>	m_NavigationComs;
    class CNavigation*          m_pCurNavigationCom = { nullptr };

public:
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END