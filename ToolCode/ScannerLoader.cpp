#include "StdAfx.h"
#include "ScannerLoader.h"
#include "comutil.h"
#include "WlanConnect.h"

CScannerLoader g_scannerLoader;
const char* CScannerLoader::XML_URL="http://s.oorrzz.com/:sda1/DCIM:.xml:Picture:Sub";
const char* CScannerLoader::FILE_ROOT_PATH="http://s.oorrzz.com";
//char* CWlanConnect::IP_ADDRESS="10.10.100.1";//默认网关
//char* CWlanConnect::SIF_DESCRIPTION="802.11n USB Wireless LAN Card";//描述
//char* CWlanConnect::SSID_WIFI="Avision-715b";//wifi SSID

int CScannerLoader::QueryAllImages(BOOL bInitImageList/*=TRUE*/)
{
	CString strxml=RequestXML(XML_URL);
	if (strxml.GetLength()==0)
		return -1;
	int nNewFile=0;
	if (bInitImageList)
	{
		CMarkup xmlDoc(strxml); 
		xmlDoc.ResetMainPos();
		while(xmlDoc.FindChildElem())   //打开父节点
		{
			xmlDoc.IntoElem();
			CString sTag=xmlDoc.GetTagName();
			CString sData=xmlDoc.GetData();
			if(sTag=="ALLFile")
			{
				IMAGE_DOWNLOAD image;
				xmlDoc.FindChildElem();  //进入ALLFile节点内
				xmlDoc.IntoElem();
				while(xmlDoc.FindChildElem())  //打开Picture节点
				{
					xmlDoc.IntoElem();
					sTag=xmlDoc.GetTagName();
					sData=xmlDoc.GetData();
					if (sTag=="NAME")
						strcpy(image.sName,sData);
					else if(sTag=="FPATH")
						strcpy(image.sSdcardPath,sData);
					else if(sTag=="EXT")
						strcpy(image.sFileType,sData);
					else if(sTag=="SIZE")
						image.dwImageSize=_ttoi(sData);
					xmlDoc.OutOfElem();
				}
				IMAGE_DOWNLOAD *pImage=m_hashImageByName.GetValue(image.sName);
				if(pImage==NULL)
				{
					pImage=m_hashImageByName.Add(image.sName);
					strcpy(pImage->sName,image.sName);
					strcpy(pImage->sSdcardPath,image.sSdcardPath);
					strcpy(pImage->sFileType,image.sFileType);
					pImage->dwImageSize =image.dwImageSize;
					nNewFile++;
				}
				xmlDoc.OutOfElem();
			}
			xmlDoc.OutOfElem();
		}
	}
	return nNewFile;
}

int CScannerLoader:: DownloadImageToBuffer(const char* image_name,BYTE* content)
{
	CString sFileURL;
	sFileURL.Format("%s%s",FILE_ROOT_PATH,image_name);
	CString sImageFile = CHttpClient::Get("",sFileURL,NULL,NULL);
	char*cword = NULL; //ANSI指针
	DWORD  num = WideCharToMultiByte(CP_UTF8, 0, _bstr_t(sImageFile), -1, NULL, 0, NULL, NULL);//計算這個UNICODE实际由几个UTF-8字組成
	if(content!=NULL)
	{
		cword = (char*)calloc(num, sizeof(char));   //申请空间
		if (cword == NULL)                          //是否申请
		{
			free(cword);
		}
		memset(cword, 0, num*sizeof(char));     //初始化
		WideCharToMultiByte(CP_UTF8, 0,_bstr_t(sImageFile), -1, cword, num, NULL, NULL);


		DWORD  dwnum= WideCharToMultiByte(CP_ACP, 0,_bstr_t(sImageFile), -1, NULL, 0, NULL, NULL);

		cword = (char*)calloc(dwnum, sizeof(char));   //申请空间
		memset(cword, 0, dwnum*sizeof(char));     //初始化
		WideCharToMultiByte(CP_ACP, 0, _bstr_t(sImageFile), -1, cword, dwnum, NULL, NULL);
		memcpy(content,cword,dwnum);
		free(cword);
	}
	return  num;
}

int CScannerLoader::DownloadImageToBuffer2(const char* image_name,BYTE* content)
{
	if(image_name==NULL)
		return 0;
	IMAGE_DOWNLOAD *pImage=m_hashImageByName.GetValue(image_name);
	if(pImage==NULL)
	{
		CScannerLoader::QueryAllImages(FALSE);
		pImage=m_hashImageByName.GetValue(image_name);
	}
	if(pImage==NULL)
		return 0;
	if(content!=NULL)
	{
		CString sFilePath="D:\\_addssskkkk_ddd.jpg";
		CScannerLoader::DownloadImageToFile(image_name,sFilePath,true);
		FILE *fp=fopen(sFilePath ,"rb");
		long sFileSize;
		if(fp)
		{
			fseek(fp,0,SEEK_END);
			sFileSize=ftell(fp);
			rewind(fp);
			char *bytearray=(char *)malloc(sFileSize*sizeof(char));
			memset(bytearray,0,sFileSize*sizeof(char));
			fread(bytearray,sFileSize,1,fp);
			memcpy(content,bytearray,sFileSize);
			free(bytearray);
			bytearray=NULL;
		}
		fclose(fp);
		return pImage->dwImageSize;
	}
	return pImage->dwImageSize;
}

int CScannerLoader::DownloadImageToFile(const char* image_name,const char* image_path,bool include_name)
{
	DeleteUrlCacheEntry (FILE_ROOT_PATH); // 清理缓存
	IMAGE_DOWNLOAD *pImage=m_hashImageByName.GetValue(image_name);
	if (image_name!=NULL&& image_path!=NULL&&pImage!=NULL)
	{
		CString sFilePath=image_path;
		if(!include_name)
			sFilePath+=image_name;
		CString sFileURL;
		sFileURL.Format("%s/%s",FILE_ROOT_PATH,pImage->sSdcardPath);
		URLDownloadToFile(0,sFileURL,sFilePath,0,NULL);	//pImage->sSdcardPath
		strcpy(pImage->sLocalPath,sFilePath);
	}
	if(pImage)
		return pImage->dwImageSize;
	else
		return 0;
}
IMAGE_DOWNLOAD* CScannerLoader::EnumFirst(BOOL bNew)
{
	IMAGE_DOWNLOAD *pImage=m_hashImageByName.GetFirst();
	while(bNew&&pImage&&ValidFile(pImage->sLocalPath))
		pImage=m_hashImageByName.GetNext();
	return pImage;
}

IMAGE_DOWNLOAD *CScannerLoader::EnumNext(BOOL bNew)
{
	IMAGE_DOWNLOAD *pImage=m_hashImageByName.GetNext();
	while(bNew&&pImage&&ValidFile(pImage->sLocalPath))
		pImage=m_hashImageByName.GetNext();
	return pImage;
}

CString CScannerLoader::RequestXML(CString url)
{
	CString strxml=CHttpClient::Get("",url,NULL,NULL);
	return strxml;
}

bool CScannerLoader::AutoConnectScannerWifi(char *sError)
{
	char IP_ADDRESS[50]="10.10.100.1";//默认网关
	char SIF_DESCRIPTION[50]="802.11n USB Wireless LAN Card";//描述
	char SSID_WIFI[50]="Avision-715b";//wifi SSID
	CWlanConnect waln(SSID_WIFI,SIF_DESCRIPTION,IP_ADDRESS);
	if(waln.ConnectNetwork()==ERROR_SUCCESS)
		return true;
	else
	{
		CString ss=waln.GetLastError();
		if(sError)
			strcpy(sError,ss);
		return false;
	}
		
}