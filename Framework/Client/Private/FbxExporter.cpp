#include "..\Public\FbxExporter.h"

namespace fs = std::filesystem;

CFbxExporter::CFbxExporter()
{

}

CFbxExporter::~CFbxExporter()
{
}

HRESULT CFbxExporter::Initialize_Export(const wstring& FbxImportFolderPath)
{
    for (const auto& entry : fs::directory_iterator(FbxImportFolderPath)) {
        if (fs::is_directory(entry)) {
            Initialize_Export(entry.path());
        }
        else if (fs::is_regular_file(entry)) {
           
            if (fileExtension == entry.path().extension())
            {
                Start_Export(entry.path());
            }
        }
    }

	return S_OK;
}

HRESULT CFbxExporter::Start_Export(const wstring& FbxPath)
{
    return S_OK;
}
