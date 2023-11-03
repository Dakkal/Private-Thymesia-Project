#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CPlayerCamera final : public CCamera
{
public:
	typedef struct tagCamera_Player_Desc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensitive = { 0.0f };
	}CAMERA_PLAYER_DESC;
private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CPlayerCamera& rhs);
	virtual	~CPlayerCamera() = default;

public:
	_bool	IsMouseMove() const { return m_bIsMouseMove; }

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	void	MouseMove(_float fTimeDelta);

private:
	_float			m_fOffsetDis = 5.5f;
	_float			m_fTargetOffsetDis = 5.5f;
	_vector			m_vOffset;
	_vector			m_vAngle;

	_float			m_fMouseSensitive = { 0.0f };
	_bool			m_bIsMouseMove = { false };
	CTransform*		m_pPlayerTransform = { nullptr };

	_vector			m_vPlayerPos;

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CPlayerCamera* Clone(void* pArg) override;
	virtual void Free() override;
};

END