#include "DriveOperations.h"

std::vector<DriveEntry> systemDrivesInfo()
{
	std::vector<DriveEntry> drives;
	DWORD drivesByte = GetLogicalDrives();
	for (short i = 0; i < 26; i++)
	{
		if (!(drivesByte&(1 << i)))continue;

		char letter = 'A' + i;
		std::cout << letter;
		std::string strLetterPath;
		strLetterPath += letter;
		strLetterPath.append(":\\");
		UINT typeMask = GetDriveType(strLetterPath.c_str());
		std::cout << typeMask;
		if (typeMask == DRIVE_FIXED || typeMask == DRIVE_REMOVABLE)
		{
			drives.push_back(DriveEntry{ letter,typeMask,strLetterPath });
		}
		std::cout << std::endl;
	}
	for (size_t i = 0; i < drives.size(); i++)
	{
		DriveEntry drv = drives[i];
		TCHAR volumeName[MAX_PATH + 1] = { 0 };
		TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
		DWORD serialNumber = 0;
		DWORD maxComponentLen = 0;
		DWORD fileSystemFlags = 0;

		if (!GetVolumeInformationA(drv.strPath.c_str(),
			volumeName, MAX_PATH, &serialNumber, &maxComponentLen,
			&fileSystemFlags, fileSystemName, MAX_PATH))
		{
			drives.clear();
			return drives;
		}

		drives[i].volName = std::string(volumeName);
		drives[i].fsName = std::string(fileSystemName);
		drives[i].serial = serialNumber;
	}
	return drives;
}
