#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	enum class DATA { SAVE, LOAD, _END };
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
	void	DeleteMakeShift_Data();
	HRESULT	Save_Data(LEVEL eLevel);
	HRESULT	Load_Data(LEVEL eLevel);
	

	/* 여기에 사용할 함수 생성 */
	HRESULT Menu(_float fTimeDelta);
	HRESULT ToolBox(_float fTimeDelta);

	HRESULT Setting_Terrain();
	HRESULT Setting_Object();
	HRESULT	Mouse_Pos();

	HRESULT List_Object();
	void	ChangeListToSelectObj();


	void ImGuiStyles();


private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	_bool			m_bShowMessage[(_uint)DATA::_END] = { false };

	/* 터레인용 변수 */
	CGameObject*	m_pSelectTerrain = { nullptr };
	_int			m_iNumVerticesX[(_uint)LEVEL::_END] = { 0 };
	_int			m_iNumVerticesZ[(_uint)LEVEL::_END] = { 0 };
	_bool			m_bIsWireFrame[(_uint)LEVEL::_END] = { true };
	_bool			m_bIsCreateTerrain[(_uint)LEVEL::_END] = { false };

	_vector			m_vTerrainPos[(_uint)LEVEL::_END];
	_vector			m_vObjectPos[(_uint)LEVEL::_END];
	_bool			m_bisTerrainPick = { true };
	_bool			m_bisObjectPick = { false };

	/* GUI용 변수 */
	_bool			m_bFrameReady = { false };
	_bool			m_bWireCheckBox = { true };

	/* 레벨 설정 변수 */
	string			m_strCurLevel = { "Level : " };
	_int			m_iCurLevel = { 0 };
	_int			m_SelectLevel = { 0 };

	/* 리스트 박스 변수 */
	wstring			m_strCurMonsterProtoObject[(_uint)LEVEL::_END] = { TEXT("") };
	wstring			m_strCurPropProtoObject[(_uint)LEVEL::_END] = { TEXT("") };
	vector<string>	m_vecMonsters;
	vector<string>	m_vecProps;
	_int			m_iSelectMonster[(_uint)LEVEL::_END] = { -1 };
	_int			m_iSelectProp[(_uint)LEVEL::_END] = { -1 };

	/* 오브젝트 리스트 변수 */
	CGameObject* m_pSelectObject = { nullptr };

	vector<string>	m_vecObjectList;
	_int			m_iSelectObject[(_uint)LEVEL::_END] = { -1 };

	/* 트랜스폼 설정 변수 */
	_bool			m_IsMonTransformOpen[(_uint)LEVEL::_END] = { false };
	_bool			m_IsPropTransformOpen[(_uint)LEVEL::_END] = { false };
	_matrix			m_matStore[(_uint)LEVEL::_END];
	_vector			m_vMonsterScale[(_uint)LEVEL::_END];
	_vector			m_vMonsterRot[(_uint)LEVEL::_END];
	_vector			m_vMonsterPos[(_uint)LEVEL::_END];
	_vector			m_vPropScale[(_uint)LEVEL::_END];
	_vector			m_vPropRot[(_uint)LEVEL::_END];
	_vector			m_vPropPos[(_uint)LEVEL::_END];

	/* 네비메쉬용 변수*/
	vector<_uint>	m_Passages;
	vector<_float3>	m_NaviMeshes;
	vector<_float3>	m_SortMeshes;


public:
	virtual void Free() override;

};

END