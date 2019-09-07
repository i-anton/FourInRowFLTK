#include "InstallThread.h"
#include <Shlobj.h>
#include <Lmcons.h>
#include <stdio.h>

InstallThread::InstallThread(InstallationWindow * wnd) :
	parent(wnd), i(0.f)
{}
void InstallThread::operator()() {
	Fl_Progress& progress = *(parent->progress);
	srand(static_cast<unsigned int>(time(0)));
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	createConfFiles();
	incStep(progress);
	if (parent->isCanceled) return;
	Fl::lock();
	parent->finish();
	Fl::unlock();
}

void InstallThread::incStep(Fl_Progress& progress)
{
	i += 0.1f;
	Fl::lock();
	progress.value(i);
	Fl::unlock();
}

void InstallThread::createConfFiles()
{
	DriveEntry ent = *(parent->drive);
	std::string uname = ent.fsName;
	uname += ent.volName;
	uname += int_to_hex(ent.serial);
	uname += ent.letter;

	std::string fullhash = hash(uname);
	static std::string keys[] =
	{
		"j2109as23p",
		"89u21njaw0",
		"oui1012kls",
		"maSzx239al",
		"o21mJWEs39"
	};
	std::string pathS(parent->install_path+"\\");
	if (CreateDirectory(pathS.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		createSingleConfFile(pathS, "JALDI14.bin", randomhash(), randomhash());
		createSingleConfFile(pathS, "AKJ2180.bin", randomhash(), randomhash());
		createSingleConfFile(pathS, "U673YIV.bin", randomhash(), randomhash());
		createSingleConfFile(pathS, "K710QA6.bin", randomhash(), randomhash());
		createSingleConfFile(pathS, "ZTY853Z.bin", fullhash.substr(0, 64), hash(keys[4]).substr(0, 64));
		createSingleConfFile(pathS, "YHIQP12.bin", fullhash.substr(64, 64), hash(keys[4]).substr(64, 64));
		ExtractResource(101, parent->install_path + "\\FourInRowFLTK.exe", "BINARY");
	}
	else
	{
		fl_alert(u8"Can't create folder!");
	}
}

void InstallThread::createSingleConfFile(const std::string& path,
	const std::string& filenameS,
	const std::string& content,
	const std::string& some_hash)
{
	std::ofstream file(path + filenameS,
		std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	file << content;
	file << some_hash;
	auto chksum = getChecksum(content + some_hash);
	file << chksum;
	file.close();
}

std::string InstallThread::hash(const std::string& in) {
	size_t i = 0;
	uint32_t hash = 0;
	std::string result;
	while (result.length() != 128) {
		hash += in[(i++) % in.length()];
		hash += hash << 10;
		hash ^= hash >> 6;
		result += static_cast<unsigned char>(hash);
		hash += hash << 3;
		hash ^= hash >> 11;
		hash += hash << 15;
	}
	return result;
}


std::string InstallThread::randomhash() {
	std::string result;
	while (result.length() != 64) {
		result += static_cast<unsigned char>(rand());
	}
	return result;
}

unsigned char InstallThread::getChecksum(const std::string& str) {
	unsigned char result = 0;
	for (size_t i = 0; i < str.length(); i++) {
		result ^= static_cast<unsigned char>(str[i]);
	}
	return result;
}

std::string InstallThread::encrypt(const std::string &val)
{
	std::string result(val);
	int shift = 52;
	for (size_t i = 0; i < result.length(); i++)
	{
		int value = result[i];
		value = (value - 32 + (95 - shift)) % 95 + 32;
		result[i] = value;
	}
	return result;
}

std::string InstallThread::decrypt(const std::string &val)
{
	std::string result(val);
	int shift = 52;
	for (size_t i = 0; i < result.length(); i++)
	{
		int value = result[i];
		value = (value - 32 - (95 - shift)) % 95 + 32;
		result[i] = value;
	}
	return result;
}

bool InstallThread::ExtractResource(std::uint16_t ResourceID,const std::string& OutputFileName, const char* ResType)
{
	try
	{
		HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(ResourceID), ResType);
		if (hResource == nullptr)
			return false;
		HGLOBAL hFileResource = LoadResource(nullptr, hResource);
		if (hFileResource == nullptr)
			return false;
		void* lpFile = LockResource(hFileResource);
		if (lpFile == nullptr)
			return false;
		std::uint32_t dwSize = SizeofResource(nullptr, hResource);
		if (dwSize == 0)
			return false;
		HANDLE hFile = CreateFile(OutputFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		HANDLE hFilemap = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, dwSize, nullptr);
		if (hFilemap == nullptr)
			return false;
		void* lpBaseAddress = MapViewOfFile(hFilemap, FILE_MAP_WRITE, 0, 0, 0);
		CopyMemory(lpBaseAddress, lpFile, dwSize);
		UnmapViewOfFile(lpBaseAddress);
		CloseHandle(hFilemap);
		CloseHandle(hFile);
		return true;
	}
	catch (...) {}
	return false;
}