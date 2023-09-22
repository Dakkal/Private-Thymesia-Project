#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT	Ready_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);
	HRESULT Render();

	void ImGuiStyles();

public:
	/* 여기에 사용할 함수 생성 */
	void Menu();
	void ToolBox();



	_bool	Is_MouseClickedGUI();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	/* 터레인용 변수 */
	_int			m_iNumVerticesX = { 0 };
	_int			m_iNumVerticesZ = { 0 };
	_bool			m_bIsCreateTerrain = { false };

	/* GUI용 변수 */
	_bool			m_bFrameReady = { false };

	string			m_strCurLevel = { "Level : " };
	_int			m_iCurLevel = {0};


private:
	/* Terrain용 변수*/

public:
	virtual void Free() override;

};

END