#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(class CGameObject* pOwner, const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);

public:
	void	Update(_matrix TransformMatrix);
	_bool	IsCollision(CCollider * pTargetCollider);

#ifdef _DEBUG
public:
	HRESULT Render();

private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect*		m_pEffect = { nullptr };
	ID3D11InputLayout*  m_pInputLayout = { nullptr };
#endif

private:
	TYPE			 m_eColliderType = { TYPE_END };
	class CBounding* m_pBounding = { nullptr };

public:
	static CCollider* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eColliderType);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END

