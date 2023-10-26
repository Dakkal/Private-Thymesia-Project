#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POS, STATE_END };
	enum DIR { DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT, DIR_UPRIGHT, DIR_UPLEFT, DIR_DOWNRIGHT, DIR_DOWNLEFT };
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = { 0.f };
		_float		fRotRadianPerSec = { 0.f };
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(class CGameObject* pOwner, const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector			Get_State(STATE eState);
	_vector			Get_Scale();
	_matrix			Get_WorldMatrix() const { return m_WorldMatrix;}
	_matrix			Get_WorldMatrix_Inverse() const { return m_WorldMatrix.Invert();}
	TRANSFORM_DESC	Get_TransformDesc() const { return m_TrasformDesc; }

	void		Set_State(STATE eState, _vector vState);
	void		Set_Look(_vector vChangeLook);
	void		Set_Scale(const _float3& vScale);
	void		Set_WorldMatrix(_matrix matWorld);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT		Bind_ShaderResources(class CShader* pShader, const char* pConstantName);

public:
	void Go_Dir(_vector vDir, _float fTimeDelta, class CNavigation* pNavi = nullptr);
	void Go_Forward(_float fTimeDelta, class CNavigation* pNavi = nullptr);
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavi = nullptr);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_Left(_float fTimeDelta, class CNavigation* pNavi = nullptr);
	void Go_Right(_float fTimeDelta, class CNavigation* pNavi = nullptr);

	void Fix_Rotation(_vector vAxis, _float fRadian);
	void Rotation(_vector vAxis, _float fRadian);
	void Turn(_vector vAxis, _float fTimeDelta);
	void LookAt(_vector vPoint);
	void Chase(_vector vPoint, _float fTimeDelta, _float fDis = 0.1f);

private:
	_matrix				m_WorldMatrix;

	TRANSFORM_DESC		m_TrasformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END