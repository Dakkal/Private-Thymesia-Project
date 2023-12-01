#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPointInstanceDesc : public CVIBuffer_Instancing::INSTANCE_DESC
	{
		_float3 vPosition = _float3(0.f, 0.f, 0.f);
		_float2 vPSize = _float2(1.f, 1.f);
	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Point_Instance(class CGameObject* pOwner, const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(XYZ xyz, _float fTimeDelta);



public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END
