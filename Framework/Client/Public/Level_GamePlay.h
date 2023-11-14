#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

private:
	HRESULT Load_Level(LEVELID eLevel);
	HRESULT Ready_Layer_BackGround(const _uint& iLayerIndex);
	HRESULT Ready_Layer_SkyDome(const _uint& iLayerIndex);
	HRESULT Ready_Layer_Camera(const _uint& iLayerIndex);
	HRESULT Ready_Layer_Props(const _uint& iLayerIndex);
	HRESULT Ready_Layer_Player(const _uint& iLayerIndex);
	HRESULT Ready_Layer_Boss(const _uint& iLayerIndex);
	HRESULT Ready_Layer_Monster(const _uint& iLayerIndex);
	HRESULT Ready_Light();

public:
	static class CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END