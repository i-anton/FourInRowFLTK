# FourInRowFLTK
Demo project of DRM with custom disk drive binding.
Written with C++/FLTK. 
Can be compiled with Visual Studio.
Uses WinAPI to get disk drive list.

## Structure

Repository consist of 3 projects:
- FourInRowFLTKNoDRM
- FourInRowFLTK
- Installer

### FourInRowFLTKNoDRM

Placeholder of game (there is no AI, only hotseat gameplay).

### FourInRowFLTK

Game with DRM. Protection code added in MainWindow.cpp

Checks:
- Files count (not all files contain userful information)
- Key (hashed)
- Username (hashed)
- Volume info (hashed)

### Installer

Installation steps could be seen in file InstallThread.cpp.
Compiled .exe file from FourInRowFLTK should be manually placed in Installer project.
File will be extracted from resources of Installer.exe after creating files for DRM.
Installer allows user to choose binding device for installation (app will work only if device is detected in system).

## Build

For building these projects you should install and configure FLTK.
Building Installer could be done only after building FourInRowFLTK and making copy of resulted exe in Installer.rc
