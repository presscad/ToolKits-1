#pragma once

#include "Buffer.h"
#include "soapH.h"
#include "soapVersionServiceSoapProxy.h"
#include "XhCharString.h"
#include "list.h"
#include "HashTable.h"
#include "LicenseManager.h"

class CVersionServer
{
public:
	int m_idSession;
	VersionServiceSoapProxy* m_pServer;
	VersionServiceSoapProxy* GetValidServer();
public:
	CVersionServer(void);
	~CVersionServer(void);
public:
	bool QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff);
	bool QueryOrderPayState(int userId,int applyRandCode);
	bool QueryAuthByteArr(int userId,int applyRandCode,BUFFER_IO* authCardBuffer);
	bool QueryProductRentals(int productId,BUFFER_IO* rentalBuffer);
	bool DownloadObjects(const char* clsName, int* idarr, int countOfObj=1,UINT serialize=0,BUFFER_IO* pExterLocalBuff=NULL);
	int  SaveOrUpdateObject(const char* clsName,void* pLocalObject,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);	//serialize=0表示取默认值
	bool SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj=1,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);
	bool SaveOrUpdateObjects(const char* clsName,IXhEnumerator* pIter,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);
	bool CompareVersion(int userId, UINT productId, UINT curVersion, UINT updateVersion, bool loadOnHasNew, VersionRevision **ppCompVersionResultArr,int *pResultCount);
	bool GetVersionReleaseNotes(UINT product_id, UINT uCurVersion, VersionRevision **ppCompVersionResultArr, int *pResultCount);
	bool SetRevisionReadFlag(int userId, int* revisionIdArr, int count);
	bool GetUpdateVerByCurVer(UINT userId, UINT product_id, UINT uCurVersion, ProductVersion **ppProductVersionArr, int *pResultCount);


public:
	void SetServerUrl(const char* url);
	int LoginUser(void* pEndUser);
	bool DownloadEndUser(int id,void *pEndUser);
	bool QueryValidRentalOrder(int sessionId,int productId,void *pOrder);
};
extern CVersionServer VER;
//extern SOAP_NMAC struct Namespace namespaces[];
