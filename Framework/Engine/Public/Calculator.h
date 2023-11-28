#pragma once
#include "Base.h"

BEGIN(Engine)

class CCalculator final : public CBase
{
	DECLARE_SINGLETON(CCalculator)

private:
	CCalculator();
	virtual ~CCalculator() = default;

public:
	_vector		Picking_Terrain(RECT rc, POINT pt, class CTransform* pTransform,class CVIBuffer* pBuffer);
	_vector		Picking_Object(RECT rc, POINT pt, class CTransform* pTransform, class CVIBuffer* pBuffer);
	HRESULT		Detrude_Collide(class CGameObject* pColObj, class CCollider* pObjCol, class CTransform* pObjTransform, class CNavigation* pNavigation);
	HRESULT		Detrude_Sphere_Collide(class CGameObject* pColObj, class CCollider* pObjCol, class CTransform* pObjTransform, class CNavigation* pNavigation);
	_float3		QuaternionToEuler(_vector vQuaternion);
	_float		Clamp(_float value, _float min, _float max);

public:
	virtual void Free() override;
};

END