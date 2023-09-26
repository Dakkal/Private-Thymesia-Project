#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	enum class LEVEL {ONE, TWO, THREE, FOUR, _END};

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT	Ready_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);
	HRESULT Render();

public:
	_bool	Is_MouseClickedGUI();

private:
	/* 세이브, 로드 함수 */
	HRESULT	Save_MakeShift_Data(LEVEL eLevel);
	HRESULT	Load_MakeShift_Data(LEVEL eLevel);
	HRESULT	Save_Data(LEVEL eLevel);
	HRESULT	Load_Data(LEVEL eLevel);
	

	/* 여기에 사용할 함수 생성 */
	void Menu();
	void ToolBox();
	void ImGuiStyles();
	


private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	/* 터레인용 변수 */
	_int			m_iNumVerticesX[(_uint)LEVEL::_END] = { 0 };
	_int			m_iNumVerticesZ[(_uint)LEVEL::_END] = { 0 };
	_bool			m_bIsCreateTerrain = { false };

	/* GUI용 변수 */
	_bool			m_bFrameReady = { false };

	string			m_strCurLevel = { "Level : " };
	_int			m_iCurLevel = { 0 };
	_int			m_SelectLevel = { 0 };


private:
	/* Terrain용 변수*/

public:
	virtual void Free() override;

};

END