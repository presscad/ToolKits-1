#pragma once
#ifndef __PATH_MANAGER_INC_
#define __PATH_MANAGER_INC_
#include "direct.h"
#include "XhCharString.h"

//Â·¾¶²Ù×÷º¯Êý
void MakeDirectory(char *path);
BOOL GetCurWorkPath(CString& file_path, BOOL bEscapeChar = TRUE, const char* subFolderName = NULL, BOOL bSingleFolder=FALSE);
BOOL SetPathToReg(const char* path, char* path_tag);
BOOL GetPathFromReg(char* path, char* path_tag, BOOL setup0_setting1);
BOOL GetSysPath(char* sys_path);
BOOL GetAppPath(char* app_path);
BOOL GetLicFile(char* lic_path);
BOOL GetLicFile2(char* lic_path);
BOOL GetJgCardPath(char* card_path);
BOOL SetJgCardPath(const char* card_path);

#endif
