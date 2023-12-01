#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Rect_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Rect_Instance(class CGameObject* pOwner, const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(XYZ xyz, _float fTimeDelta);

public:

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END