#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	enum XYZ
	{
		X, Y, Z, XY, XZ, YZ, _END
	};


public:
	typedef struct tagInstanceDesc
	{
		_float3			vCenter;
		_float3			vRange;
		_float			fRangeOffset = 0.f;
		_float			fSpeedMin = 0.f, fSpeedMax = 0.f;
		_float3			vScaleMin, vScaleMax;
		_float			fLifeTimeMin, fLifeTimeMax;
		_uint			iNumInstance = 0;
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(class CGameObject* pOwner, const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(_float fTimeDelta) { return S_OK; }
	virtual HRESULT Render() override;

	const _bool&	Is_LifeOver() const { return m_IsAllLifeOver; }

	const _bool&	Is_Update() const { return m_IsUpdate; }
	void			Set_Update(_bool bUpdate)  { m_IsUpdate = bUpdate; }
	
protected:
	_uint					m_iStrideInstance = { 0 };
	_uint					m_iNumInstance = { 0 };
	_uint					m_iNumIndicesPerInstance = { 0 };
	ID3D11Buffer*			m_pVBInstance = { nullptr };
	VTXINSTANCE*			m_pVertices = { nullptr };

	_float*					m_pSpeeds = { nullptr };
	_float*					m_pLifeTimes = { nullptr };

	_float*					m_pTimeAccs = { nullptr };

	_uint					m_iLifeCnt = { 0 };
	_bool*					m_pIsLifeOver = { nullptr };

protected:
	_bool					m_IsAllLifeOver = { false };
	_bool					m_IsUpdate = { true };

public:
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) PURE;
	virtual void Free() override;
};

END