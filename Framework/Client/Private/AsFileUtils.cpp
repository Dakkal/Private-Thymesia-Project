#include "pch.h"
#include "AsFileUtils.h"


CAsFileUtils::CAsFileUtils()
{
}

CAsFileUtils::~CAsFileUtils()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

void CAsFileUtils::Open(wstring filePath, FileMode mode)
{
	if (mode == FileMode::Write)
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{
		_handle = ::CreateFile
		(
			filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}

	assert(_handle != INVALID_HANDLE_VALUE);
}

void CAsFileUtils::Write(void* data, _uint dataSize)
{
	_uint numOfBytes = 0;
	assert(::WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void CAsFileUtils::Write(const string& data)
{
	_uint size = (_uint)data.size();
	Write(size);

	if (data.size() == 0)
		return;

	Write((void*)data.data(), size);
}

HRESULT CAsFileUtils::Read(void** data, _uint dataSize)
{
	_uint numOfBytes = 0;
	if (false == ::ReadFile(_handle, *data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr))
		return E_FAIL;

	return S_OK;
}

HRESULT CAsFileUtils::Read(OUT string& data)
{
	_uint size = Read<_uint>();

	if (size == 0)
		return E_FAIL;

	char* temp = new char[size + 1];
	temp[size] = 0;
	
	if(FAILED(Read((void**)&temp, size)))
		return E_FAIL;

	data = temp;
	delete[] temp;

	return S_OK;
}