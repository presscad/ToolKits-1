
#include "stdafx.h"
#include "TMSServer.h"
#include "ServerTowerType.h"

#ifdef __CONNECT_REMOTE_SERVER_

CRemoteServer::CRemoteServer(const char* serverIp/*=NULL*/)
{
	if(serverIp==NULL)
		sprintf(m_sServerURL,"http://127.0.0.1/TMSServer/TMSService.asmx");
	else
		sprintf(m_sServerURL,"http://%s/TMSService.asmx",serverIp);
	m_pServer=new TAPServiceSoapProxy(m_sServerURL);
}

CRemoteServer::~CRemoteServer()
{
	if(m_pServer)
	{
		m_pServer->destroy();
		delete m_pServer;
		m_pServer=NULL;
	}
}

bool CRemoteServer::ValidServer()
{
	return (m_pServer)?true:false;
}

void CRemoteServer::InitServer(const char* server_url)
{
	if(ValidServer())
	{
		m_pServer->destroy();
		delete m_pServer;
		m_pServer=NULL;
	}
	sprintf(m_sServerURL,"http://%s/TMSService.asmx",server_url);
	m_pServer=new TAPServiceSoapProxy(m_sServerURL);
}

bool CRemoteServer::LoginServer(const char *userName,const char *password)
{								   
	_ns1__loginUser inputPara;
	inputPara.userName=userName;
	inputPara.fingerprint="";
	inputPara.password=password;
	inputPara.version=0;
	_ns1__loginUserResponse response;
	if(m_pServer->loginUser(&inputPara,response)!=SOAP_OK||response.loginUserResult<=0)//Login调用失败表示联网失败!
		return false;
	else 
	{
		m_idSession=response.loginUserResult;
		return true;
	}
	return false;
}



bool CRemoteServer::IsSupportType(const char *clsName)
{
	if(	strcmp(clsName,"TowerTypeFileGroup")==0||
		strcmp(clsName,"TowerTypeFile")==0||
		strcmp(clsName,"TowerAssembly")==0||
		strcmp(clsName,"TowerPart")==0||
		strcmp(clsName,"AssemblePart")==0)
		return true;
	else 
		return false;
}
bool CRemoteServer::DeleteObject(const char* clsName,int objId)
{
	if(!CRemoteServer::IsSupportType(clsName))
		return false;
	_ns1__DeleteObject inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.objId=objId;
	_ns1__DeleteObjectResponse response;
	inputPara.clsName=clsName;
	if(m_pServer->DeleteObject(&inputPara,response)!=SOAP_OK||!response.DeleteObjectResult)
		return false;
	return true;
}
bool CRemoteServer::DeleteObject(CServerObject* pLocalObj)
{
	if(!IsSupportType(pLocalObj->GetServerClassName()))
		return false;
	_ns1__DeleteObject inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.objId=pLocalObj->m_id;
	_ns1__DeleteObjectResponse response;
	inputPara.clsName=pLocalObj->GetServerClassName();
	if(m_pServer->DeleteObject(&inputPara,response)!=SOAP_OK||!response.DeleteObjectResult)
		return false;
	return true;
}
bool CRemoteServer::QueryAllObjects(const char* clsName,BUFFER_IO* pExterLocalBuff)
{
	if (CRemoteServer::IsSupportType(clsName))
		return false;
	_ns1__QueryAllObjcts inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.clsName=clsName;
	_ns1__QueryAllObjctsResponse response;
	if(m_pServer->QueryAllObjcts(&inputPara,response)!=SOAP_OK||response.QueryAllObjctsResult==NULL||
		response.QueryAllObjctsResult->__size<=0||response.QueryAllObjctsResult->__ptr==NULL)
		return false;
	pExterLocalBuff->Write(response.QueryAllObjctsResult->__ptr,response.QueryAllObjctsResult->__size);
	return true;
}
bool CRemoteServer::DownloadObjById(const char* clsName,int id,BUFFER_IO* pExterLocalBuff)
{
	if (!CRemoteServer::IsSupportType(clsName))
		return false;
	_ns1__DownloadObjById inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.fileId=id;
	_ns1__DownloadObjByIdResponse response;
	if(m_pServer->DownloadObjById(&inputPara,response)!=SOAP_OK||response.DownloadObjByIdResult==NULL||
		response.DownloadObjByIdResult->__size<=0||response.DownloadObjByIdResult->__ptr==NULL)
		return false;
	pExterLocalBuff->Write(response.DownloadObjByIdResult->__ptr,response.DownloadObjByIdResult->__size);
	return false;
}
int  CRemoteServer::SaveOrUpdateObject(CServerObject* pLocalObj)
{
	if(!IsSupportType(pLocalObj->GetServerClassName()))
		return false;
	CBuffer buffer(0x100000);
	_ns1__SaveOrUpdateObject inputPara;
	inputPara.byteArr=new xsd__base64Binary();
	inputPara.sessionId=m_idSession;
	_ns1__SaveOrUpdateObjectResponse response;
	CServerObject *pServerObj=(CServerObject*)pLocalObj;
	pServerObj->ToBuffer(buffer);
	inputPara.clsName=pServerObj->GetServerClassName();;
	inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
	inputPara.byteArr->__size=buffer.GetLength();
	if(m_pServer->SaveOrUpdateObject(&inputPara,response)==SOAP_OK)
		pServerObj->IsEqual(pServerObj->m_id);
	delete inputPara.byteArr;
	return pServerObj->m_id;
}
#endif