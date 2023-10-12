#pragma once
#include "Client_Defines.h"
#include "AsFileUtils.h"
#include "FbxStruct.h"

class CFbxExporter
{
public:
	CFbxExporter();
	~CFbxExporter();

public:
	HRESULT Initialize_Export(const wstring& FbxImportFolderPath);

private:
	HRESULT Start_Export(const wstring& FbxPath);

private:
	wstring fileExtension = { TEXT(".fbx") };

};