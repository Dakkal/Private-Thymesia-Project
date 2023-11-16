#pragma once
#include "Client_Defines.h"
#include "LandObject.h"
#include "PartObject.h"

BEGIN(Engine)
class CTransform;
class CStateMachine;
class CNavigation;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CLandObject
{
protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);

	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta) override;

public:
	void					Reset_TargetEnemy();
	HRESULT					Search_TargetEnemy();
	HRESULT					ReSearch_TargetEnemy();
	HRESULT					Check_TargetEnemy();
	CGameObject*			Get_TargetEnemy() const { return m_pTargetEnemy; }
	vector<CGameObject*>	Get_TargetEnemies() const { return m_vecTargetEnemy; }

	void					Set_Parry(_bool IsParry) { m_IsParry = IsParry; }
	const _bool&			Is_Parry() const		 { return m_IsParry; }

private:
	vector<CGameObject*>	m_vecTargetEnemy;	
	CGameObject*			m_pTargetEnemy = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CStateMachine*			m_pStateMachineCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

	_bool					m_bFirstDrop = { true };
	_bool					m_IsParry = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PlayerParts();
	HRESULT Ready_State();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override; 
	virtual void Free() override;
};

END
