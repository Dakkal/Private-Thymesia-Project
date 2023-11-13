#pragma once
#include "Client_Defines.h"
#include "LandObject.h"
#include "PartObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CStateMachine;
class CCollider;
END

BEGIN(Client)

class CEnemy_Halberd final : public CLandObject
{
protected:
	CEnemy_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Halberd(const CEnemy_Halberd& rhs);

	virtual ~CEnemy_Halberd() = default;

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
	CTransform* Get_PlayerTransform() const { return m_pPlayerTransform; }
	void		Set_LookPlayer(_bool bIsLook) { m_bIsLookPlayer = bIsLook; }

public:
	void	Out_Player(_float fTimeDelta);
	void	Look_Player(_float fTimeDelta);
	void	Set_PlayerTransform();

private:
	CTransform* m_pPlayerTransform = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CStateMachine* m_pStateMachineCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_int					iIndex = 0;

	_float					m_fReleaseTimeAcc = { 0.f };
	_bool					m_bIsOutPlayer = { false };
	_bool					m_bIsLookPlayer = { false };

	_bool					m_bFirstDrop = { true };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Ready_State();

public:
	static CEnemy_Halberd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
