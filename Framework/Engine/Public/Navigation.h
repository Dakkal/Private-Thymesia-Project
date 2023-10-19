#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int	iCurIndex = { -1 };
	}NAVI_DESC;

private:
	CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CNavigation(class CGameObject* pOwner, const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring & strNavigationDataFiles);
	virtual HRESULT Initialize(void* pArg) override;
	void	Update(_matrix WorldMatrix);

	_bool	IsMove(_vector vPoint);

#ifndef NDEBUG
public:
	HRESULT Render();

private:
	class CShader* m_pShader = { nullptr };
#endif // !NDEBUG

private:
	static	_matrix			m_NaviWorldMatrix;
	_int					m_iCurrentIndex = { -1 };
	vector<class CCell*>	m_Cells;

private:
	HRESULT	Set_Neighbors();


public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavigationDataFiles);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END