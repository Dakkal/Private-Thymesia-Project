#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Camera.h"

BEGIN(Client)

class CSeq_Camera_Player final : public CPartObject
{
protected:
	CSeq_Camera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSeq_Camera_Player(const CSeq_Camera_Player& rhs);
	virtual ~CSeq_Camera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void	Set_Camera_Fov(_float fFov) { m_fFov = fFov; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_vector			m_vEye, m_vAt;
	_float			m_fFov = { 80.f };


public:
	static CSeq_Camera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END