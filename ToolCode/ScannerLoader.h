#pragma once
#ifdef __SCANNER_LOADER_
#include <objbase.h>
#include <iphlpapi.h>
#include <wlanapi.h> 
#include "HttpClient.h"
#include "Markup.h"
#include "HashTable.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

struct IMAGE_DOWNLOAD{
	char sName[100];
	char sFileType[10];
	DWORD dwImageSize;
	char sSdcardPath[MAX_PATH];
	char sLocalPath[MAX_PATH];
	IMAGE_DOWNLOAD(){memset(this,0,sizeof(IMAGE_DOWNLOAD));}
};
class CScannerLoader
{
	CHashStrList<IMAGE_DOWNLOAD> m_hashImageByName;
	static const char* XML_URL;//="http://s.oorrzz.com/:sda1/DCIM:.xml:Picture:Sub";
	static const char* FILE_ROOT_PATH;//="http://s.oorrzz.com";
	
public:
	CScannerLoader(){;}
	~CScannerLoader(){Empty();}
	
	bool ConnectTest(){return QueryAllImages(FALSE)>=0;}
	int QueryAllImages(BOOL bInitImageList=TRUE);     //查询xml更新m_hashImageByName
	int DownloadImageToBuffer(const char* image_name,BYTE* content);
	int DownloadImageToBuffer2(const char* image_name,BYTE* content);
	int DownloadImageToFile(const char* image_name,const char* image_path,bool include_name);
	void Empty(){m_hashImageByName.Empty();}
	IMAGE_DOWNLOAD *EnumFirst(BOOL bNew=TRUE);  //获取第一个没有本地路径的结构体
	IMAGE_DOWNLOAD *EnumNext(BOOL bNew=TRUE);   //获取下一个没有本地路径的结构体
public:
	static bool AutoConnectScannerWifi(char *sError);
private:
	CString RequestXML(CString url);
	static bool ValidFile(const char* image_path){return strlen(image_path)>0;}
};

extern CScannerLoader g_scannerLoader;
#endif