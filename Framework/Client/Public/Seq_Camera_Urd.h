#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Camera.h"

BEGIN(Client)

class CSeq_Camera_Urd final : public CPartObject
{
protected:
	CSeq_Camera_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSeq_Camera_Urd(const CSeq_Camera_Urd& rhs);
	virtual ~CSeq_Camera_Urd() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_vector			m_vEye, m_vAt;


public:
	static CSeq_Camera_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END