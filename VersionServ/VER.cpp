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
VersionServiceSoapProxy* CVersionServer::GetValidServer()
{
	if(m_pServer!=NULL)
		return m_pServer;
	if(g_sServerURL[0]==0)
		strcpy(g_sServerURL,"http://www.xerofox.com/VersionService/VersionService.asmx");
		//strcpy(g_sServerURL,"http://192.168.2.6/VersionService/VersionService.asmx");	//www.xerofox.com
	m_pServer=new VersionServiceSoapProxy(g_sServerURL);
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
	if(m_pServer->LoginByByteArr(&inputPara,response)!=SOAP_OK||response.LoginByByteArrResult<=0)//Loginµ÷ÓÃÊ§°Ü±íÊ¾ÁªÍøÊ§°Ü!
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
		inputPara.clsName="TempAuthCard";		//ÏÂÔØÁÙÊ±ÊÚÈ¨¿¨
	else if(strcmp(clsName,"OfficialAuthCard")==0)
		inputPara.clsName="OfficialAuthCard";	//ÕýÊ½ÊÚÈ¨¿¨
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
		/* ·¶Àý 
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
	/* ·¶Àý 
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
	/* ·¶Àý 
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