#pragma once
#include "stdafx.h"
#include "InstallationWindow.h"
class InstallThread
{
public:
	InstallThread(InstallationWindow* wnd);
	void operator()();
private:
	void incStep(Fl_Progress& progress);
	void createConfFiles();
	void createSingleConfFile(const std::string& path,
		const std::string& filenameS,
		const std::string& content,
		const std::string& some_hash);
	bool ExtractResource(std::uint16_t ResourceID, const std::string& OutputFileName, const char* ResType);
	static std::string hash(const std::string& in);
	static std::string randomhash();
	static unsigned char getChecksum(const std::string& str);
	static std::string encrypt(const std::string&);
	static std::string decrypt(const std::string&);
	template< typename T >
	std::string int_to_hex(T i)
	{
		std::stringstream stream;
		stream << "0x"
			<< std::setfill('0') << std::setw(sizeof(T) * 2)
			<< std::hex << i;
		return stream.str();
	}
private:
	InstallationWindow* parent;
	float i;
};