#include "StdAfx.h"
#include "PathManager.h"


//////////////////////////////////////////////////////////////////////////
//文件路径操作函数
//////////////////////////////////////////////////////////////////////////
void MakeDirectory(char *path)
{
	char bak_path[MAX_PATH], drive[MAX_PATH];
	strcpy(bak_path, path);
	char *dir = strtok(bak_path, "/\\");
	if (strlen(dir) == 2 && dir[1] == ':')
	{
		strcpy(drive, dir);
		strcat(drive, "\\");
		_chdir(drive);
		dir = strtok(NULL, "/\\");
	}
	while (dir)
	{
		_mkdir(dir);
		_chdir(dir);
		dir = strtok(NULL, "/\\");
	}
}
BOOL GetCurWorkPath(CString& file_path, BOOL bEscapeChar/*=TRUE*/, const char* subFolderName/*=NULL*/)
{
	AcApDocument* pDoc = acDocManager->curDocument();
	if (pDoc == NULL)
		return FALSE;
	CXhChar500 sWorkDir, sName;
	file_path = pDoc->fileName();
	if (file_path.CompareNoCase("Drawing1.dwg") == 0)	//默认的DWG文件
		return FALSE;
	_splitpath(file_path, NULL, NULL, sName, NULL);
	int index = file_path.ReverseFind('\\');	//反向查找'\\'
	file_path = file_path.Left(index);		//移除文件名
	//
	if (subFolderName)
		sWorkDir.Printf("%s\\%s", file_path, subFolderName);
	else
		sWorkDir.Printf("%s\\%s", file_path, (char*)sName);
	MakeDirectory(sWorkDir);
	if (bEscapeChar)
		sWorkDir.Append("\\");
	file_path.Format("%s", (char*)sWorkDir);
	return TRUE;
}
BOOL SetPathToReg(const char* path, char* path_tag)
{
	char sSubKey[MAX_PATH] = "";
#ifdef __CNC_
	strcpy(sSubKey, "Software\\Xerofox\\CNC\\Settings");
#elif __PNC_
	strcpy(sSubKey, "Software\\Xerofox\\PNC\\Settings");
#else
	strcpy(sSubKey, "Software\\Xerofox\\TMA\\Settings");
#endif
	HKEY hKey;
	LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_WRITE, &hKey);
	if (retval == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, _T(path_tag), NULL, REG_SZ, (BYTE*)(char*)path, strlen(path));
		RegCloseKey(hKey);
		return TRUE;
	}
	else
		return FALSE;
}
BOOL GetPathFromReg(char* path, char* path_tag, BOOL setup0_setting1)
{
	char sPath[MAX_PATH * 10] = "", sSubKey[MAX_PATH] = "";
#ifdef __CNC_
	if (setup0_setting1 == 0)
		strcpy(sSubKey, "Software\\Xerofox\\CNC\\SETUP");
	else
		strcpy(sSubKey, "Software\\Xerofox\\CNC\\Settings");
#elif __PNC_
	if (setup0_setting1 == 0)
		strcpy(sSubKey, "Software\\Xerofox\\PNC\\SETUP");
	else
		strcpy(sSubKey, "Software\\Xerofox\\PNC\\Settings");
#else
	if (setup0_setting1 == 0)
		strcpy(sSubKey, "Software\\Xerofox\\TMA\\SETUP");
	else
		strcpy(sSubKey, "Software\\Xerofox\\TMA\\Settings");
#endif
	//
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_READ, &hKey);
	DWORD dwDataType, dwLength = MAX_PATH * 10;
	if (RegQueryValueEx(hKey, _T(path_tag), NULL, &dwDataType, (BYTE*)&sPath[0], &dwLength) == ERROR_SUCCESS)
		strcpy(path, sPath);
	RegCloseKey(hKey);
	if (strlen(path) > 1)
		return TRUE;
	return FALSE;
}
BOOL GetSysPath(char* sys_path) { return GetPathFromReg(sys_path, "SYS_PATH", 0); }
BOOL GetAppPath(char* app_path) { return GetPathFromReg(app_path, "APP_PATH", 0); }
BOOL GetLicFile(char* lic_path) { return GetPathFromReg(lic_path, "lic_file", 1); }
BOOL GetLicFile2(char* lic_path) { return GetPathFromReg(lic_path, "lic_file2", 1); }
BOOL GetJgCardPath(char* card_path) { return GetPathFromReg(card_path, "JG_CARD", 1); }
BOOL SetJgCardPath(const char* card_path) { return SetPathToReg(card_path, "JG_CARD"); }
