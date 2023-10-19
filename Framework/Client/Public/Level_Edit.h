#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CLevel_Edit final : public CLevel
{
private:
	CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Edit() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

	HRESULT Ready_Light();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);


private:
	class CImgui_Manager* m_pImgui_Manager = { nullptr };


public:
	static class CLevel_Edit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END