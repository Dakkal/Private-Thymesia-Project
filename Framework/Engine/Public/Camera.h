#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;

public:
	const _matrix& Get_View() { return m_ViewMatrix; }
	const _matrix& Get_Proj() { return m_ProjMatrix; }

protected:
	_matrix	m_ViewMatrix;
	_matrix	m_ProjMatrix;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END