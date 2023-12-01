#include "..\Public\EffectObject.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Rect_Instance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectObject::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	EFFECT_DESC* pPartDesc = (EFFECT_DESC*)pArg;

	m_vTargetPos = pPartDesc->vTargetPos;

	return S_OK;
}

void CEffectObject::PriorityTick(_float fTimeDelta)
{
}

void CEffectObject::Tick(_float fTimeDelta)
{
}

void CEffectObject::LateTick(_float fTimeDelta)
{
}

HRESULT CEffectObject::Render()
{
	return S_OK;
}

HRESULT CEffectObject::Use_Effect()
{
	m_pVIBufferCom->Set_Update(true);

	return S_OK;
}

const _bool& CEffectObject::Is_Update(TYPE eType)
{
	if (TYPE::POINT == eType)
	{
		return m_pVIBufferCom->Is_Update();
	}
	else
	{
		return m_pVIRectBufferCom->Is_Update();
	}

	
}

void CEffectObject::Free()
{
	__super::Free();
}
