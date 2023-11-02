#pragma once
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef	struct tagCameraDesc : public CTransform::TRANSFORM_DESC
	{
		_vector			vEye, vAt;
		_float			fFovy, fAspect, fNear, fFar;
	}CAMERA_DESC;


protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;

public:
	virtual void Set_TargetTransform(class CTransform* pTransform) { m_pTargetTransform = pTransform; }

protected:
	_vector			m_vEye, m_vAt;
	_float			m_fFovy, m_fAspect, m_fNear, m_fFar;

protected:
	class CTransform*	m_pTransform = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };

	class CTransform* m_pTargetTransform = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END