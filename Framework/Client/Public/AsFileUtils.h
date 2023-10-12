#pragma once
#include "AsTypes.h"

enum FileMode : uint8
{
	Write,
	Read,
};

class CAsFileUtils
{
public:
	CAsFileUtils();
	~CAsFileUtils();

	void Open(wstring filePath, FileMode mode);

	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<>
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	void Write(void* data, _uint dataSize);
	void Write(const string& data);

	template<typename T>
	HRESULT Read(OUT T& data)
	{
		DWORD numOfBytes = 0;
		if (false == ::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr))
			return E_FAIL;

		return S_OK;
	}

	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	HRESULT Read(void** data, _uint dataSize);
	HRESULT Read(OUT string& data);

private:
	HANDLE _handle = INVALID_HANDLE_VALUE;
};

