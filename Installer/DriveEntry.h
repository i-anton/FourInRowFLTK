#pragma once
#include "stdafx.h"

struct DriveEntry
{
	char letter;
	unsigned int type;
	std::string strPath;
	std::string volName;
	std::string fsName;
	unsigned long serial;
};