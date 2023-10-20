#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_1 final : public CLevel
{
private:
	CLevel_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_1() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

private:
	HRESULT Load_Level(LEVELID eLevel);
	HRESULT Ready_Layer_Camera(const _uint& iLayerIndex);
	HRESULT Ready_Light();

private:
	LEVELID	m_eLevel;

public:
	static class CLevel_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END

