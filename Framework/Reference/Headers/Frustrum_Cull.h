#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrustrum_Cull final : public CBase
{
	DECLARE_SINGLETON(CFrustrum_Cull)

private:
	CFrustrum_Cull();
	virtual ~CFrustrum_Cull() = default;

public:
	HRESULT Initialize();
	void	Tick();
	_bool	IsIn_Frustum_World(_vector vWorldPos, _float fRadius);

private:
	class CPipeLine*	m_pPipeLine = { nullptr };
	_float3				m_vOriginalPoints[8];
	_float3				m_vPoints[8];

	_vector				m_WorldPlanes[6];

private:
	void Make_Planes(const _float3* pPoints, _vector* pPlanes);

public:
	virtual void Free() override;
};

END

