//#include "stdafx.h"
#include "afxwin.h"
#include "VER.h"
#include "LifeObj.h"
#include "ArrayList.h"
#include "LicenseManager.h"
#include "Buffer.h"

/////////////////////////////////////////////////////////////////////////////
CVersionServer VER;
CVersionServer::CVersionServer(void)
{
	m_pServer=NULL;
	m_idSession=0;
}

CVersionServer::~CVersionServer(void)
{
	if(m_pServer)
	{
		m_pServer->destroy();
		delete m_pServer;
		m_pServer=NULL;
	}
}
static char g_sServerURL[MAX_PATH]="";
void CVersionServer::SetServerUrl(const char* url)
{
	sprintf(g_sServerURL,"http://%s/VersionService.asmx",url);
	if(m_pServer)
	{
		m_pServer->destroy();
		delete m_pServer;
		m_pServer=NULL;
	}
}

static BOOL IsWindows7OrGreater()
{
	//2000	0X0500   
	//XP	0X0501
	//2003	0X0502
	//VISTA 0X0600
	//Win7	0X0601
#ifdef _WIN64
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!::GetVersionEx(&osVer))
	{
		return FALSE;
	}
	if (osVer.dwMajorVersion > 6 || ((osVer.dwMajorVersion == 6) && osVer.dwMinorVersion >= 1))
		return TRUE;
	else
		return FALSE;
#else
	int i = 0, j = 0;
	_asm
	{
		pushad
		mov ebx, fs:[0x18]; get self pointer from TEB
		mov eax, fs:[0x30]; get pointer to PEB / database
		mov ebx, [eax + 0A8h]; get OSMinorVersion
		mov eax, [eax + 0A4h]; get OSMajorVersion
		mov j, ebx
		mov i, eax
		popad
	}
	if (i > 6 || ((i == 6) && j >= 1))
		return TRUE;
	else
		return FALSE;
#endif
}

VersionServiceSoapProxy* CVersionServer::GetValidServer()
{
	if(m_pServer!=NULL)
		return m_pServer;
	if(g_sServerURL[0]==0)
		strcpy(g_sServerURL,"http://www.xerofox.com/VersionService/VersionService.asmx");
		//strcpy(g_sServerURL,"http://192.168.2.6/VersionService/VersionService.asmx");	//www.xerofox.com
	m_pServer=new VersionServiceSoapProxy(g_sServerURL);
	//设置本地编码格式 wht 19-08-17
	if(IsWindows7OrGreater())
		setlocale(LC_ALL, "Chinese");
	else
		setlocale(LC_ALL, "chs");
	soap_set_mode(m_pServer, SOAP_C_MBSTRING);
	return m_pServer;
}
#ifdef _SOAP_DEBUG_LOG_FILE_
#include "LogFile.h"
#endif
int CVersionServer::LoginUser(void* pEndUser)
{
	if(pEndUser==NULL)
		return 0;
	m_pServer=GetValidServer();
	if(m_pServer==NULL)
		return 0;
#ifdef _SOAP_DEBUG_LOG_FILE_
	CLogErrorLife life(&logerr);
#endif
	_ns1__LoginByByteArr inputPara;
	CBuffer buffer(0x100000);
	CEndUser *pEndUser1=(CEndUser*)pEndUser;
	buffer.ClearContents();
	pEndUser1->ToBuffer(buffer);
	buffer.SeekToBegin();
	inputPara.userInfo=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> selflife(inputPara.userInfo);
	inputPara.userInfo->__ptr=(BYTE*)buffer.GetBufferPtr();
	inputPara.userInfo->__size=buffer.GetLength();
	_ns1__LoginByByteArrResponse response;
#ifdef _SOAP_DEBUG_LOG_FILE_
	//AfxMessageBox(CXhChar100("m_pServer->soap_endpoint=%s",m_pServer->soap_endpoint));
#endif
	if(m_pServer->LoginByByteArr(&inputPara,response)!=SOAP_OK||response.LoginByByteArrResult<=0)//Login调用失败表示联网失败!
	{
#ifdef _SOAP_DEBUG_LOG_FILE_
		//AfxMessageBox("ok01");
#endif
		delete m_pServer;
		m_pServer=NULL;
		m_idSession=0;
	}
	else 
		m_idSession=response.LoginByByteArrResult;
	return m_idSession;
}
bool CVersionServer::QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff)
{
	m_pServer=GetValidServer();
	if(m_pServer==NULL)
		return false;
	_ns1__QueryObjects inputPara;
	inputPara.sessionId=m_idSession;
	if(strcmp(clsName,"Order")==0)
		inputPara.clsName="Order";
	else if(strcmp(clsName,"EndUser")==0)
		inputPara.clsName="EndUser";
	else if(strcmp(clsName,"VirtualDog")==0)
		inputPara.clsName="VirtualDog";
	else if(strcmp(clsName,"ProductRental")==0)
		inputPara.clsName="ProductRental";
	else if(strcmp(clsName,"AuthCard")==0)
		inputPara.clsName="AuthCard";
	else if(strcmp(clsName,"TempAuthCard")==0)
		inputPara.clsName="TempAuthCard";
	else if(strcmp(clsName,"OfficialAuthCard")==0)
		inputPara.clsName="OfficialAuthCard";
	else
		return false;
	inputPara.xmlScope=xmlscope;
	_ns1__QueryObjectsResponse response;
	if(m_pServer->QueryObjects(&inputPara,response)!=SOAP_OK)
		return false;
	if( response.QueryObjectsResult!=NULL&&response.QueryObjectsResult->__size>0&&response.QueryObjectsResult->__ptr!=NULL)
		pExterLocalBuff->Write(response.QueryObjectsResult->__ptr,response.QueryObjectsResult->__size);
	return true;
}
bool CVersionServer::DownloadObjects(const char* clsName, int* idarr, int countOfObj/*=1*/,UINT serialize/*=0*/,BUFFER_IO* pExterLocalBuff/*=NULL*/)
{
	m_pServer=GetValidServer();
	if(m_pServer==NULL)
		return false;
	ns1__ArrayOfInt arrayOfInt;
	arrayOfInt.int_=idarr;
	arrayOfInt.__sizeint_=countOfObj;
	_ns1__DownloadObjects inputPara;
	inputPara.idarr=&arrayOfInt;
	inputPara.serialize=serialize;
	if(strcmp(clsName,"TempAuthCard")==0)
		inputPara.clsName="TempAuthCard";		//下载临时授权卡
	else if(strcmp(clsName,"OfficialAuthCard")==0)
		inputPara.clsName="OfficialAuthCard";	//正式授权卡
	else
		return false;
	_ns1__DownloadObjectsResponse response;
	if(m_pServer->DownloadObjects(&inputPara,response)!=SOAP_OK)
		return false;
	if( pExterLocalBuff!=NULL&&response.DownloadObjectsResult!=NULL&&
		response.DownloadObjectsResult->__size>0&&response.DownloadObjectsResult->__ptr!=NULL)
		pExterLocalBuff->Write(response.DownloadObjectsResult->__ptr,response.DownloadObjectsResult->__size);
	return true;
}

int  CVersionServer::SaveOrUpdateObject(const char* clsName,void* pLocalObject,DWORD serialize/*=0*/,BYTE fromClientTMA0TAP1/*=0*/)
{
	if(SaveOrUpdateObjects(clsName,&pLocalObject,1,serialize,fromClientTMA0TAP1))
		return ((CServerObject*)pLocalObject)->Id;
	else
		return 0;
}

bool  CVersionServer::SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj/*=1*/,DWORD serialize/*=0*/,BYTE fromClientTMA0TAP1/*=0*/)
{
	m_pServer=GetValidServer();
	if(m_pServer==NULL)
		return false;
	int id=0;
	CBuffer buffer(0x100000),filebuf(0x100000);
	_ns1__SaveOrUpdateObjects inputPara;
	inputPara.byteArr=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> selflife(inputPara.byteArr);
	inputPara.sessionId=m_idSession;
	_ns1__SaveOrUpdateObjectsResponse response;
	if(strcmp(clsName,"Order")==0)
	{
		for(int i=0;i<countOfObj;i++)
		{
			COrder *pOrder=(COrder*)((COrder**)ppLocalObjArr)[i];
			filebuf.ClearContents();
			pOrder->ToBuffer(filebuf);
			filebuf.SeekToBegin();
			buffer.Write(filebuf,filebuf.Length);
		}
		inputPara.clsName="Order";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		/* 范例 
		"<?xml version="1.0" encoding="utf-8"?>"
		" <SaveOrUpdate>"
		"     <ClassName value="TowerFile" />"
		"     <ObjectCount value="1" />"
		" </SaveOrUpdate>");
		*/
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
						" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"Order\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(" </SaveOrUpdate>");
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	return false;
}

bool CVersionServer::QueryOrderPayState(int userId,int applyRandCode)
{
	CXhChar500 xmlformat;
	/* 范例 
	"<?xml version="1.0" encoding="utf-8"?>"
	" <SaveOrUpdate>"
	"     <ClassName value="TowerFile" />"
	"     <ObjectCount value="1" />"
	" </SaveOrUpdate>");
	*/
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <QueryObjects>";
	xmlformat.Copy(xmlheader);
	xmlformat.Append("     <ClassName value=\"Order\" />");
	xmlformat.Append(CXhChar50("     <EndUserId value=\"%d\" />",userId));
	xmlformat.Append(CXhChar50("     <ApplyRandCode value=\"%d\" />",applyRandCode));
	xmlformat.Append("     <ClientType value=\"C++\" />");
	xmlformat.Append(" </QueryObjects>");
	CBuffer buffer;
	if(QueryObjects("Order",xmlformat,&buffer))
	{
		COrder order;
		buffer.SeekToBegin();
		order.FromBuffer(buffer);
		if(order.wxOrderSerial.GetLength()>0)
			return true;
		else
			return false;
	}
	else
		return false;
}
bool CVersionServer::QueryAuthByteArr(int userId,int applyRandCode,BUFFER_IO* authCardBuffer)
{
	CXhChar500 xmlformat;
	/* 范例 
	"<?xml version="1.0" encoding="utf-8"?>"
	" <SaveOrUpdate>"
	"     <ClassName value="TowerFile" />"
	"     <ObjectCount value="1" />"
	" </SaveOrUpdate>");
	*/
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <QueryObjects>";
	xmlformat.Copy(xmlheader);
	xmlformat.Append("     <ClassName value=\"AuthCard\" />");
	xmlformat.Append(CXhChar50("     <EndUserId value=\"%d\" />",userId));
	xmlformat.Append(CXhChar50("     <ApplyRandCode value=\"%d\" />",applyRandCode));
	xmlformat.Append("     <ClientType value=\"C++\" />");
	xmlformat.Append(" </QueryObjects>");
	if(QueryObjects("AuthCard",xmlformat,authCardBuffer))
		return true;
	else
		return false;
}

bool CVersionServer::QueryProductRentals(int productId,BUFFER_IO* rentalBuffer)
{
	CXhChar500 xmlformat;
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <QueryObjects>";
	xmlformat.Copy(xmlheader);
	xmlformat.Append("     <ClassName value=\"ProductRental\" />");
	xmlformat.Append(CXhChar50("     <ProductId value=\"%d\" />",productId));
	xmlformat.Append("     <ClientType value=\"C++\" />");
	xmlformat.Append(" </QueryObjects>");
	if(QueryObjects("ProductRental",xmlformat,rentalBuffer))
		return true;
	else
		return false;
}

bool CVersionServer::DownloadEndUser(int id,void *pEndUser)
{
	CXhChar500 xmlformat;
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
		" <QueryObjects>";
	xmlformat.Copy(xmlheader);
	xmlformat.Append("     <ClassName value=\"EndUser\" />");
	xmlformat.Append(CXhChar50("     <Id value=\"%d\" />",id));
	xmlformat.Append("     <ClientType value=\"C++\" />");
	xmlformat.Append(" </QueryObjects>");
	CBuffer buffer;
	if(QueryObjects("EndUser",xmlformat,&buffer))
	{
		if(pEndUser!=NULL)
		{
			CEndUser *pEndUser1=(CEndUser*)pEndUser;
			buffer.SeekToBegin();
			int nCount=buffer.ReadInteger();
			if(nCount==1)
				pEndUser1->FromBuffer(buffer);
		}
		return true;
	}
	else
		return false;
}

bool CVersionServer::QueryValidRentalOrder(int sessionId,int productId,void *pOrder)
{
	CXhChar500 xmlformat;
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
		" <QueryObjects>";
	xmlformat.Copy(xmlheader);
	xmlformat.Append("     <ClassName value=\"Order\" />");
	xmlformat.Append(CXhChar50("     <EndUserId value=\"%d\" />", sessionId));
	xmlformat.Append(CXhChar50("     <ProductId value=\"%d\" />", productId));
	xmlformat.Append(CXhChar50("     <LastValid value=\"true\" />"));
	xmlformat.Append("     <ClientType value=\"C++\" />");
	xmlformat.Append(" </QueryObjects>");
	CBuffer buffer;
	if(QueryObjects("Order",xmlformat,&buffer))
	{
		if(pOrder!=NULL)
		{
			COrder *pOrder1=(COrder*)pOrder;
			buffer.SeekToBegin();
			int nCount=buffer.ReadInteger();
			if(nCount==1)
			{	
				pOrder1->FromBuffer(buffer);
				return true;
			}
		}
		return false;
	}
	else
		return false;
}

bool CVersionServer::CompareVersion(int userId, UINT productId, UINT curVersion, UINT updateVersion, bool loadOnHasNew, VersionRevision **ppCompVersionResultArr, int *pResultCount)
{
	m_pServer = GetValidServer();
	if (m_pServer == NULL || ppCompVersionResultArr==NULL || pResultCount==NULL)
		return false;
	_ns1__CompareVersion inputPara;
	inputPara.userId = userId;
	inputPara.product_USCOREid = productId;
	inputPara.uCurVersion = curVersion;
	inputPara.uUpdateVersion = updateVersion;
	inputPara.bLoadOnHasNew = loadOnHasNew;
	_ns1__CompareVersionResponse response;
	if (m_pServer->CompareVersion(&inputPara, response) != SOAP_OK)
		return false;
	typedef VersionRevision* RevisionPtr;
	if (response.CompareVersionResult != NULL &&
		response.CompareVersionResult->__sizeVersionRevision > 0 &&
		response.CompareVersionResult->VersionRevision != NULL)
	{
		int n = response.CompareVersionResult->__sizeVersionRevision;
		ns1__VersionRevision **versionRevisionArr = response.CompareVersionResult->VersionRevision;
		*ppCompVersionResultArr = new VersionRevision[n];
		for (int i = 0; i < n; i++)
		{
			(*ppCompVersionResultArr)[i].Id = versionRevisionArr[i]->Id;
			(*ppCompVersionResultArr)[i].important = versionRevisionArr[i]->important;
			(*ppCompVersionResultArr)[i].productId = versionRevisionArr[i]->productId;
			(*ppCompVersionResultArr)[i].readed = versionRevisionArr[i]->readed;
			(*ppCompVersionResultArr)[i].revisionType = versionRevisionArr[i]->revisionType;
			if (versionRevisionArr[i]->title != NULL)
			{
				strcpy((*ppCompVersionResultArr)[i].title, versionRevisionArr[i]->title);
				//UTF2Gb2312((*ppCompVersionResultArr)[i].title, (*ppCompVersionResultArr)[i].title);
			}
			if (versionRevisionArr[i]->contents != NULL)
			{
				strcpy((*ppCompVersionResultArr)[i].contents, versionRevisionArr[i]->contents);
				//UTF2Gb2312((*ppCompVersionResultArr)[i].contents, (*ppCompVersionResultArr)[i].contents);
			}
		}
		if (pResultCount)
			*pResultCount = n;
		return true;
	}
	else
		return false;
}

bool CVersionServer::GetVersionReleaseNotes(UINT product_id, UINT uCurVersion, VersionRevision **ppCompVersionResultArr, int *pResultCount)
{
	m_pServer = GetValidServer();
	if (m_pServer == NULL)
		return false;
	if (ppCompVersionResultArr == NULL || pResultCount == NULL)
		return false;
	_ns1__GetVersionReleaseNotes inputPara;
	inputPara.product_USCOREid = product_id;
	inputPara.uCurVersion = uCurVersion;
	_ns1__GetVersionReleaseNotesResponse response;
	if (m_pServer->GetVersionReleaseNotes(&inputPara, response) != SOAP_OK)
		return false;

	typedef VersionRevision* RevisionPtr;
	if (response.GetVersionReleaseNotesResult != NULL &&
		response.GetVersionReleaseNotesResult->VersionRevision != NULL &&
		response.GetVersionReleaseNotesResult->__sizeVersionRevision > 0)
	{
		int n = response.GetVersionReleaseNotesResult->__sizeVersionRevision;
		ns1__VersionRevision **versionRevisionArr = response.GetVersionReleaseNotesResult->VersionRevision;
		*ppCompVersionResultArr = new VersionRevision[n];
		for (int i = 0; i < n; i++)
		{
			(*ppCompVersionResultArr)[i].Id = versionRevisionArr[i]->Id;
			(*ppCompVersionResultArr)[i].important = versionRevisionArr[i]->important;
			(*ppCompVersionResultArr)[i].productId = versionRevisionArr[i]->productId;
			(*ppCompVersionResultArr)[i].readed = versionRevisionArr[i]->readed;
			(*ppCompVersionResultArr)[i].revisionType = versionRevisionArr[i]->revisionType;
			if (versionRevisionArr[i]->title != NULL)
			{
				strcpy((*ppCompVersionResultArr)[i].title, versionRevisionArr[i]->title);
				//UTF2Gb2312((*ppCompVersionResultArr)[i].title, (*ppCompVersionResultArr)[i].title);
			}
			if (versionRevisionArr[i]->contents != NULL)
			{
				strcpy((*ppCompVersionResultArr)[i].contents, versionRevisionArr[i]->contents);
				//UTF2Gb2312((*ppCompVersionResultArr)[i].contents, (*ppCompVersionResultArr)[i].contents);
			}
		}
		if (pResultCount)
			*pResultCount = n;
		return true;
	}
	else
		return false;
}

bool CVersionServer::SetRevisionReadFlag(int userId, int* revisionIdArr,int count)
{
	m_pServer = GetValidServer();
	if (m_pServer == NULL)
		return false;
	if (userId == 0 || revisionIdArr == NULL || count == 0)
		return false;
	_ns1__SetRevisionReadFlag inputPara;
	inputPara.userId = userId;
	inputPara.revisionIdArr->__sizeint_ = count;
	inputPara.revisionIdArr->int_ = new int[count];
	for (int i = 0; i < count; i++)
		inputPara.revisionIdArr->int_[i] = revisionIdArr[i];
	_ns1__SetRevisionReadFlagResponse response;
	bool bRetCode = false;
	if (m_pServer->SetRevisionReadFlag(&inputPara, response) == SOAP_OK)
		bRetCode = true;
	if (inputPara.revisionIdArr != NULL)
		delete inputPara.revisionIdArr;
	return bRetCode;
}
bool CVersionServer::GetUpdateVerByCurVer(UINT userId, UINT product_id, UINT uCurVersion, ProductVersion **ppProductVersionArr, int *pResultCount)
{
	m_pServer = GetValidServer();
	if (m_pServer == NULL)
		return false;
	if (ppProductVersionArr == NULL || pResultCount == NULL)
		return false;
	_ns1__GetUpdateVerByCurVer inputPara;
	inputPara.userId = userId;
	inputPara.product_USCOREid = product_id;
	inputPara.uCurVersion = uCurVersion;
	_ns1__GetUpdateVerByCurVerResponse response;
	if (m_pServer->GetUpdateVerByCurVer(&inputPara, response) != SOAP_OK)
		return false;
	typedef ProductVersion* VersionPtr;
	if (response.GetUpdateVerByCurVerResult != NULL &&
		response.GetUpdateVerByCurVerResult->ProductVersion != NULL &&
		response.GetUpdateVerByCurVerResult->__sizeProductVersion > 0)
	{
		int n = response.GetUpdateVerByCurVerResult->__sizeProductVersion;
		ns1__ProductVersion **ProductVersionArr = response.GetUpdateVerByCurVerResult->ProductVersion;
		*ppProductVersionArr = new ProductVersion[n];
		for (int i = 0; i < n; i++)
		{
			(*ppProductVersionArr)[i].productId = ProductVersionArr[i]->productId;
			(*ppProductVersionArr)[i].version = ProductVersionArr[i]->version;
			(*ppProductVersionArr)[i].SetReleaseData(ProductVersionArr[i]->releaseDate);
		}
		if (pResultCount)
			*pResultCount = n;
		return true;
	}
	else
		return false;
}