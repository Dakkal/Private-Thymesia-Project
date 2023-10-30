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
	virtual HRESULT Initialize_Alone(const _matrix& NaviMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	void			Update(_matrix WorldMatrix);

	_int	IsMove(_vector vPoint);
	_int	IsIn(_vector vPoint);
	_int    CheckIn(_vector vPoint);

public:
	const _matrix&	Get_Navi_Matrix() { return m_NaviWorldMatrix; }
	class CCell*	Get_CurCell() { return m_Cells[m_iCurrentIndex]; }
	_float3			Get_Closet_Cell_Point(_vector vPick);

public:
	void			Set_CurIndex(const _uint& iIndex) {  m_iCurrentIndex = iIndex; }
	HRESULT			Set_toCell(_uint iIndex, class CTransform* pTransform);
	HRESULT			Set_CelltoPassage(_uint iIndex);
	HRESULT			Set_All_CelltoPassage();

public:
	_vector			Get_Cell_SliderVec(_vector vLook);
	HRESULT			Add_Cell(_float3* vPoints);
	HRESULT			Delete_Last_Cell();
	
	HRESULT	Save_Navi(const wstring& Path);

#ifdef _DEBUG
public:
	HRESULT Render();

private:
	class CShader* m_pShader = { nullptr };
#endif // !NDEBUG

private:
	static	_matrix			m_NaviWorldMatrix;
	_int					m_iCurrentIndex = { -1 };
	vector<class CCell*>	m_Cells;
	vector<_uint>			m_Passages;

private:
	HRESULT	Set_Neighbors();


public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavigationDataFiles);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _matrix& NaviMatrix);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END