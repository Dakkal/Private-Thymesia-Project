#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CThrow_Weapon_Urd final : public CGameObject
{
protected:
	CThrow_Weapon_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThrow_Weapon_Urd(const CThrow_Weapon_Urd& rhs);
	virtual ~CThrow_Weapon_Urd() = default;

public:
	virtual HRESULT Initialize(_matrix Push_Matrix, class CTransform* pPlayerTransform);
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

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	class CRenderer*	m_pRendererCom = { nullptr };
	class CShader*		m_pShaderCom = { nullptr };
	class CBinModel*	m_pModelCom = { nullptr };
	class CTransform*	m_pTransformCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };

private:
	class CTransform*	m_pPlayerTransform = { nullptr };
	_matrix				m_WorldMatrix;

	_bool				m_bGoForward = { true };


public:
	static CThrow_Weapon_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _matrix Push_Matrix, class CTransform* pPlayerTransform = nullptr);
	virtual void Free() override;
};

END