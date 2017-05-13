#pragma once

#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "largeaddresstoggle.h"

class File
{
private:
	char* _path;
	bool _opened;
	HANDLE _file;
	IMAGE_FILE_HEADER _header;
	DWORD _ptr;
public:
	File(char*);
	~File();
	bool isOpen();
	void ShowInfo();
	void EnableLargeAddress();
	void DisableLargeAddress();
	bool isLargeAddressAware();
	IMAGE_FILE_HEADER GetPEHeader();
private:
	WORD File::GetPEPtr();
};

