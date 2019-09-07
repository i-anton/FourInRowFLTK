#include "StrOperations.h"

inline std::string decrypt(const std::string & val)
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

std::string hash(const std::string & in) {
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

unsigned char getChecksum(const std::string & str) {
	unsigned char result = 0;
	for (size_t i = 0; i < str.length(); i++) {
		result ^= static_cast<unsigned char>(str[i]);
	}
	return result;
}

unsigned char getChecksum(const std::vector<unsigned char>& str) {
	unsigned char result = 0;
	for (size_t i = 0; i < str.size(); i++) {
		result ^= static_cast<unsigned char>(str[i]);
	}
	return result;
}

std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
