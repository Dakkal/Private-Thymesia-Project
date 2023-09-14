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
	const _vector&		Get_State(STATE eState) const	{ return XMLoadFloat4x4(&m_WorldMatrix).r[eState];}
	const _float3&		Get_Scale() const;

	void				Set_State(STATE eState, _fvector vState);
	void				Set_Scale(const _float3& vScale);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT		Bind_ShaderReSources(class CShader* pShader, const char* pConstantName);

public:
	void Go_Forward(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

	void Fix_Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn_Invert(_fvector vAxis, _float fTimeDelta);
	void LookAt(_fvector vPoint);
	void Chase(_fvector vPoint, _float fTimeDelta, _float fDis = 0.1f);

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