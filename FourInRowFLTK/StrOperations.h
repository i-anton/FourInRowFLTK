#pragma once
#include "stdafx.h"

std::string decrypt(const std::string &val);
std::string hash(const std::string& in);
unsigned char getChecksum(const std::string& str);
unsigned char getChecksum(const std::vector<unsigned char>& str);
template< typename T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}

std::string ExePath();