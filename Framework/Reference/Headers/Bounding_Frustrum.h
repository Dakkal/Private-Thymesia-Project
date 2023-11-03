#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_Frustrum final : public CBounding
{
public:
	typedef struct tagBoundingFrustrumDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vDegree;
		_float		fRightSlope;           
		_float		fLeftSlope;            
		_float		fTopSlope;             
		_float		fBottomSlope;          
		_float		fNear, fFar;            

	}BOUNDING_FRUSTRUM_DESC;

private:
	CBounding_Frustrum();
	virtual ~CBounding_Frustrum() = default;

public:
	const BoundingFrustum* Get_Bouding() const { return m_pFrustrum; }


public:
	virtual HRESULT			Initialize(const BOUNDING_DESC* pDesc) override;
	virtual void			Update(_matrix TransformMatrix);

	virtual _bool			IsCollision(CCollider::TYPE eType, CBounding* pBouding);

#ifdef _DEBUG
public:
	virtual HRESULT			Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingFrustum*		m_pFrustrum_Original = { nullptr };
	BoundingFrustum*		m_pFrustrum = { nullptr };

public:
	static CBounding_Frustrum* Create(const BOUNDING_DESC* pDesc);
	virtual void Free() override;
};

END