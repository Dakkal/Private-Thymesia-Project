#pragma once
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		CCollider*			pOwner;
		_float3				vCenter;
		_vector				vCollideColor;
		_vector				vColor;
	}BOUNDING_DESC;
protected:
	CBounding();
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(const BOUNDING_DESC * pDesc);
	virtual void	Update(_matrix TransformMatrix) PURE;

public:
	virtual _bool	IsCollision(CCollider::TYPE eType, CBounding * pBouding) PURE;
	virtual void	Set_Coll(_bool _bIsColli) { m_IsColl = _bIsColli; }

	virtual _vector	Get_ColWorldPos() PURE;

protected:
	CCollider*		m_pBoundingOwner = { nullptr };
	_bool			m_IsColl = { false };

	_vector			m_vColor;
	_vector			m_vCollideColor;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>*pBatch) PURE;
#endif

public:
	virtual void Free();
};

END