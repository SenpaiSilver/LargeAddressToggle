#include <iomanip>
#include "File.hh"

File::File(char* path): _path(path), _opened(false), _ptr(NULL) {
	this->_file = CreateFile(path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (this->_file == INVALID_HANDLE_VALUE) {
		std::cerr << "Could not open file." << std::endl;
		return;
	}
	else
		this->_opened = true;
	this->_header = this->GetPEHeader();
}

File::~File() {
	if (this->_opened)
		CloseHandle(this->_file);
}

bool File::isOpen() {
	return (this->_opened);
}

void File::ShowInfo() {
	std::cout << "Arch: ";
	switch (this->_header.Machine)
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
		std::cout << "Unknown (0x" << std::hex << this->_header.Machine << ")";
		break;
	}
	std::cout << std::dec << std::endl;
	if (this->_header.Machine == 0x014c) {
		std::cout << "IMAGE_FILE_LARGE_ADDRESS_AWARE=0x" << std::setfill('0') << std::setw(4) << std::hex << (this->_header.Characteristics & 0x0020) << std::endl;
	}
}

void File::EnableLargeAddress() {
	DWORD written = 0;

	this->_header.Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE;
	if (SetFilePointer(this->_file, this->_ptr, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		std::cerr << "Could not seek." << std::endl;
	}
	WriteFile(this->_file, (LPCVOID)&this->_header, sizeof(this->_header), &written, NULL);
	if (written != sizeof(IMAGE_FILE_HEADER)) {
		std::cerr << "The PE header might be corrupted." << std::endl;
	}
	std::cout << "Enabling large address awareness." << std::endl;
}

void File::DisableLargeAddress() {
	DWORD written = 0;

	this->_header.Characteristics ^= IMAGE_FILE_LARGE_ADDRESS_AWARE;
	if (SetFilePointer(this->_file, this->_ptr, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		std::cerr << "Could not seek." << std::endl;
	}
	WriteFile(this->_file, (LPCVOID)&this->_header, sizeof(this->_header), &written, NULL);
	if (written != sizeof(IMAGE_FILE_HEADER)) {
		std::cerr << "The PE header might be corrupted." << std::endl;
	}
	std::cout << "Disabling large address awareness." << std::endl;
}

bool File::isLargeAddressAware() {
	return (this->_header.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE);
}

WORD File::GetPEPtr() {
	DWORD dwPtr = 0;
	DWORD read_bytes = 0;
	WORD ptr = 0;

	dwPtr = SetFilePointer(this->_file, 0x3C, NULL, FILE_BEGIN);
	if (dwPtr == INVALID_SET_FILE_POINTER) {
		std::cerr << "Can't find PE header pointer" << std::endl;
		return (-1);
	}
	ReadFile(this->_file, (LPVOID)&ptr, 2, &read_bytes, NULL);
	if (read_bytes <= 0) {
		std::cerr << "Couldn't read" << std::endl;
		return (-1);
	}
	this->_ptr = ptr + 4;
	return (ptr + 4);
}

IMAGE_FILE_HEADER File::GetPEHeader() {
	DWORD dwPtr = 0;
	DWORD read_bytes = 0;
	IMAGE_FILE_HEADER header = { 0 };

	if (this->_ptr == NULL)
		this->GetPEPtr();
	dwPtr = SetFilePointer(this->_file, this->_ptr, NULL, FILE_BEGIN);
	if (dwPtr == INVALID_SET_FILE_POINTER) {
		std::cerr << "Couldn't find the PE header" << std::endl;
		return (header);
	}
	std::cout << "Ptr: 0x" << std::hex << (dwPtr - 4) << "+4" << std::endl;
	ReadFile(this->_file, (LPVOID)&header, sizeof(IMAGE_FILE_HEADER), &read_bytes, NULL);
	if (read_bytes != sizeof(IMAGE_FILE_HEADER)) {
		std::cerr << "Expected " << sizeof(IMAGE_FILE_HEADER) << " bytes, but " << read_bytes << " bytes were read" << std::endl;
	}
	return (header);
}