#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vDegree;
	}BOUNDING_OBB_DESC;

private:
	CBounding_OBB();
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Bouding() const { return m_pOBB; }


public:
	virtual HRESULT			Initialize(const BOUNDING_DESC* pDesc) override;
	virtual void			Update(_matrix TransformMatrix);

	virtual _bool			IsCollision(CCollider::TYPE eType, CBounding* pBouding);

	virtual _vector			Get_ColWorldPos() override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingOrientedBox* m_pOBB_Original = { nullptr };
	BoundingOrientedBox* m_pOBB = { nullptr };

public:
	static CBounding_OBB* Create(const BOUNDING_DESC* pDesc);
	virtual void Free() override;
};

END

