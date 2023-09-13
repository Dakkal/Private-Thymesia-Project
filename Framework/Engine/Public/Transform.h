#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POS, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = { 0.f };
		_float		fRotRadianPerSec = { 0.f };
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_float4x4	Get_WorldMatrix() { return m_WorldMatrix; }
	_vector		Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState];}

	void		Set_State(STATE eState, _fvector vState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Go_Forward(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

	void Fix_Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);

private:
	_float4x4			m_WorldMatrix;

	_float				m_fSpeedPerSec = { 0.0f };
	_float				m_fRotRadianPerSec = { 0.0f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END