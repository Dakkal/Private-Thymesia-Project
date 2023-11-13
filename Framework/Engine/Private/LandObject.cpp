#include "LandObject.h"
#include "Navigation.h"
#include "Transform.h"
#include "Cell.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLandObject::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CLandObject::PriorityTick(_float fTimeDelta)
{
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::LateTick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::Set_CurNaviCom(CNavigation* pNavi)
{
	if (nullptr == pNavi)
		return E_FAIL;

	m_pCurNavigationCom = pNavi;

	return S_OK;
}

_bool CLandObject::Find_NaviMesh(_vector vPos)
{
 	_int iIndex = -1;

	for (auto& pNavi : m_NavigationComs)
	{
		if (m_pCurNavigationCom == pNavi)
			continue;

		iIndex = pNavi->IsIn(vPos);
		if (-1 != iIndex)
		{
			m_pCurNavigationCom = pNavi;
			return true;
		}
			
	}
	return false;
}

HRESULT CLandObject::Set_On_NaviMesh(CTransform* pTransform)
{
	CCell* pCell = m_pCurNavigationCom->Get_CurCell();

	_vector vWorldPos = pTransform->Get_State(CTransform::STATE_POS);
	_matrix matNavi = m_pCurNavigationCom->Get_Navi_Matrix();

	_vector		vLocalPos = XMVector3TransformCoord(vWorldPos, matNavi.Invert());
	_vector		vPlane;

	vPlane = XMPlaneFromPoints(*pCell->Get_Point(CCell::POINT_A), *pCell->Get_Point(CCell::POINT_B), *pCell->Get_Point(CCell::POINT_C));

	_float	fY = ((-vPlane.x * vLocalPos.x - vPlane.z * vLocalPos.z - vPlane.w)) / vPlane.y;

	vLocalPos.y = fY;

	vWorldPos = XMVector3TransformCoord(vLocalPos, matNavi);
	vWorldPos.w = 1;

	pTransform->Set_State(CTransform::STATE_POS, vWorldPos);

	return S_OK;
}

CGameObject* CLandObject::Clone(void* pArg)
{
	__super::Clone(pArg);

	return nullptr;
}

void CLandObject::Free()
{
	__super::Free();

	if (1 < m_NavigationComs.size())
	{
		for (auto& pNavi : m_NavigationComs)
		{
			Safe_Release(pNavi);
		}
	}
	else
	{
		Safe_Release(m_pCurNavigationCom);
	}
	m_NavigationComs.clear();

}
