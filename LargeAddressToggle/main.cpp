#include <iostream>
#include <Windows.h>

#include "largeaddresstoggle.h"

WORD get_PE_ptr(HANDLE hFile) {
	DWORD dwPtr = 0;
	DWORD read_bytes = 0;
	WORD ptr = 0;

	dwPtr = SetFilePointer(hFile, 0x3C, NULL, FILE_BEGIN);
	if (dwPtr == INVALID_SET_FILE_POINTER) {
		std::cerr << "Can't find PE header pointer" << std::endl;
		return (-1);
	}
	ReadFile(hFile, (LPVOID)&ptr, 2, &read_bytes, NULL);
	if (read_bytes <= 0) {
		std::cerr << "Couldn't read" << std::endl;
		return (-1);
	}
	//ptr = ((ptr << 8) | (ptr >> 8)) & 0xFFFF;
	std::cout << "PE Header address: 0x" << std::hex << ptr << std::endl;
	return (ptr + 4);
}

IMAGE_FILE_HEADER get_PE_header(HANDLE hFile, WORD ptr) {
	DWORD dwPtr = 0;
	DWORD read_bytes = 0;
	IMAGE_FILE_HEADER header = { 0 };

	dwPtr = SetFilePointer(hFile, ptr, NULL, FILE_BEGIN);
	if (dwPtr == INVALID_SET_FILE_POINTER) {
		std::cerr << "Couldn't find the PE header" << std::endl;
		return (header);
	}
	std::cout << "Ptr: 0x" << std::hex << dwPtr << std::endl;
	ReadFile(hFile, (LPVOID)&header, sizeof(IMAGE_FILE_HEADER), &read_bytes, NULL);
	if (read_bytes != sizeof(IMAGE_FILE_HEADER)) {
		std::cerr << "Expected " << sizeof(IMAGE_FILE_HEADER) << " bytes, but " << read_bytes << " bytes were read" << std::endl;
	}
	return (header);
}

void show_header_info(PIMAGE_FILE_HEADER header) {
	std::cout << "Arch: ";
	switch (header->Machine)
	{
	case 0x014c:
		std::cout << "x86";
		break;
	case 0x0200:
		std::cout << "Intel Itanium";
		break;
	case 0x8664:
		std::cout << "x64";
		break;
	default:
		std::cout << "Unknown (0x" << std::hex << header->Machine << ")";
		break;
	}
	std::cout << std::dec << std::endl;
	if (header->Machine == 0x014c) {
		std::cout << "IMAGE_FILE_LARGE_ADDRESS_AWARE=" << (header->Characteristics & 0x0020) << std::endl;
	}
}

void make_large_address_aware(HANDLE hFile, WORD ptr, PIMAGE_FILE_HEADER header) {
	HANDLE out;

	out = CreateFile("./out.exe",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	CloseHandle(out);
}

int main(int ac, char** av) {
	HANDLE hFile;
	WORD PE_address;
	IMAGE_FILE_HEADER header;
	
	//hFile = CreateFile("C:\\Users\\senpaisilver\\Documents\\visual studio 2017\\Projects\\LargeAddressToggle\\arma3_x64.exe",
	hFile = CreateFile("C:\\Users\\senpaisilver\\Documents\\visual studio 2017\\Projects\\LargeAddressToggle\\AoK HD.exe",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "Can't open" << std::endl;
		return (EXIT_FAILURE);
	}
	PE_address = get_PE_ptr(hFile);
	header = get_PE_header(hFile, PE_address);
	show_header_info(&header);
	//
	if (header.Machine == 0x014c) {
		SetFilePointer(hFile, PE_address, NULL, FILE_BEGIN);
		if (header.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE) {
			std::cout << "Disabling large address awareness" << std::endl;
			header.Characteristics = header.Characteristics ^ IMAGE_FILE_LARGE_ADDRESS_AWARE;
		}
		else {
			std::cout << "Enabling large address awareness" << std::endl;
			header.Characteristics = header.Characteristics | IMAGE_FILE_LARGE_ADDRESS_AWARE;
		}
		DWORD bytes_written = 0;
		WriteFile(hFile, (LPCVOID)&header, sizeof(header), &bytes_written, NULL);
	}
	else {
		std::cout << "No need to make it large address aware" << std::endl;
	}
	//
	CloseHandle(hFile);
	std::cout << "Exiting" << std::endl;
	std::getchar();
	return (EXIT_SUCCESS);
}