#include "stdafx.h"
#include "TMS.h"
#include "LifeObj.h"
#include "ArrayList.h"
#include "ServerTowerType.h"

#ifdef __CONNECT_REMOTE_SERVER_
/////////////////////////////////////////////////////////////////////////////
CTmsServer TMS;
CTmsServer::CTmsServer(void)
{
	m_pServer=NULL;
	m_idSession=0;
}


CTmsServer::~CTmsServer(void)
{
	if(m_pServer)
	{
		m_pServer->destroy();
		delete m_pServer;
		m_pServer=NULL;
	}
}
static char g_sServerURL[MAX_PATH]="";
void CTmsServer::SetServerUrl(const char* url)
{
	sprintf(g_sServerURL,"http://%s/TMSService.asmx",url);
}
bool CTmsServer::LoginUser(const char* userName, const char* password, const char* fingerprint/*=NULL*/)
{
	if(g_sServerURL[0]==0)
		strcpy(g_sServerURL,"http://127.0.0.1/MESServer/TMSService.asmx");
	m_pServer=new TMSServiceSoapProxy(g_sServerURL);
	_TMSServ__loginUserByByteArr inputPara;
	CBuffer buffer;
	buffer.WriteStringToUTF8(userName);
	buffer.WriteStringToUTF8(password);
	buffer.WriteStringToUTF8(fingerprint);
	inputPara.userInfo=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> selflife(inputPara.userInfo);
	inputPara.userInfo->__ptr=(BYTE*)buffer.GetBufferPtr();
	inputPara.userInfo->__size=buffer.GetLength();
	_TMSServ__loginUserByByteArrResponse response;
	bool bRetCode=false;
	if(m_pServer->loginUserByByteArr(&inputPara,response)!=SOAP_OK||response.loginUserByByteArrResult<=0)//Login调用失败表示联网失败!
	{
		delete m_pServer;
		m_pServer=NULL;
	}
	else 
	{
		m_idSession=response.loginUserByByteArrResult;
		bRetCode=true;
	}
	return bRetCode;
}
bool CTmsServer::QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__QueryObjects inputPara;
	inputPara.sessionId=m_idSession;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)	//文件组,每个文件组内所有文件表示同一个模型相同分段的不同历史版本
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerType")==0)	//当前用户权限范围内的塔型
		inputPara.clsName="TowerType";
	else if(strcmp(clsName,"TowerFile")==0)	//指定模型分段的某一特定上传历史版本
		inputPara.clsName="TowerFile";
	else if(strcmp(clsName,"TowerAssembly")==0)	//指定塔位的试组塔例模型
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)		//塔型工件(钢板与角钢)的三维工艺数据交换信息
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)	//塔型工件在塔例模型中的装配记录对象
		inputPara.clsName="AssemblePart";
	else if(strcmp(clsName,"ManuElemTask")==0)	//生产任务
		inputPara.clsName="ManuElemTask";
	else
		return false;
	inputPara.xmlScope=xmlscope;
	_TMSServ__QueryObjectsResponse response;
	if(m_pServer->QueryObjects(&inputPara,response)!=SOAP_OK)
		return false;
	if( response.QueryObjectsResult!=NULL&&response.QueryObjectsResult->__size>0&&response.QueryObjectsResult->__ptr!=NULL)
		pExterLocalBuff->Write(response.QueryObjectsResult->__ptr,response.QueryObjectsResult->__size);
	return true;
}
bool CTmsServer::DownloadObjects(const char* clsName, int* idarr, int countOfObj/*=1*/,UINT serialize/*=0*/,BUFFER_IO* pExterLocalBuff/*=NULL*/)
{
	if(m_pServer==NULL)
		return false;
	TMSServ__ArrayOfInt arrayOfInt;
	arrayOfInt.int_=idarr;
	arrayOfInt.__sizeint_=countOfObj;
	_TMSServ__DownloadObjects inputPara;
	inputPara.idarr=&arrayOfInt;
	inputPara.serialize=serialize;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)	//文件组,每个文件组内所有文件表示同一个模型相同分段的不同历史版本
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerType")==0)	//当前用户权限范围内的塔型
		inputPara.clsName="TowerType";
	else if(strcmp(clsName,"TowerFile")==0)	//指定模型分段的某一特定上传历史版本
		inputPara.clsName="TowerFile";
	else if(strcmp(clsName,"TowerAssembly")==0)	//指定塔位的试组塔例模型
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)		//塔型工件(钢板与角钢)的三维工艺数据交换信息
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)	//塔型工件在塔例模型中的装配记录对象
		inputPara.clsName="AssemblePart";
	else if(strcmp(clsName,"PpiPart")==0)
		inputPara.clsName="PpiPart";
	else
		return false;
	_TMSServ__DownloadObjectsResponse response;
	if(m_pServer->DownloadObjects(&inputPara,response)!=SOAP_OK)
		return false;
	if( pExterLocalBuff!=NULL&&response.DownloadObjectsResult!=NULL&&
		response.DownloadObjectsResult->__size>0&&response.DownloadObjectsResult->__ptr!=NULL)
		pExterLocalBuff->Write(response.DownloadObjectsResult->__ptr,response.DownloadObjectsResult->__size);
	return true;
}
bool CTmsServer::GetObjectsVersion(const char* clsName, int* idarr, int countOfObj/*=1*/,UINT* ver_arr/*=NULL*/)
{
	if(m_pServer==NULL)
		return false;
	TMSServ__ArrayOfInt arrayOfInt;
	arrayOfInt.int_=idarr;
	arrayOfInt.__sizeint_=countOfObj;
	_TMSServ__GetObjectsVersion inputPara;
	inputPara.idarr=&arrayOfInt;
	if(strcmp(clsName,"TowerType")==0)	//当前用户权限范围内的塔型
		inputPara.clsName="TowerType";
	else if(strcmp(clsName,"TowerFile")==0)	//指定模型分段的某一特定上传历史版本
		inputPara.clsName="TowerFile";
	else if(strcmp(clsName,"TowerPart")==0)		//塔型工件(钢板与角钢)的三维工艺数据交换信息
		inputPara.clsName="TowerPart";
	else
		return false;
	_TMSServ__GetObjectsVersionResponse response;
	if(m_pServer->GetObjectsVersion(&inputPara,response)!=SOAP_OK)
		return false;
	if(ver_arr!=NULL&&response.GetObjectsVersionResult!=NULL)
		memcpy(ver_arr,response.GetObjectsVersionResult->unsignedInt,4*response.GetObjectsVersionResult->__sizeunsignedInt);
	return true;
}
bool CTmsServer::DownloadFileById(const char* clsName,int fileId,BUFFER_IO* pExterLocalBuff)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__DownloadFileById inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.fileId=fileId;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerFile")==0)
		inputPara.clsName="TowerFile";
	else if(strcmp(clsName,"TowerAssembly")==0)
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)
		inputPara.clsName="AssemblePart";
	else
		return false;
	_TMSServ__DownloadFileByIdResponse response;
	if(m_pServer->DownloadFileById(&inputPara,response)!=SOAP_OK)
		return false;
	if(response.DownloadFileByIdResult!=NULL&&response.DownloadFileByIdResult->__size>0&&response.DownloadFileByIdResult->__ptr!=NULL)
		pExterLocalBuff->Write(response.DownloadFileByIdResult->__ptr,response.DownloadFileByIdResult->__size);
	return true;
}
bool CTmsServer::DownloadTowerTypeFile(void* pTowerTypeFile)
{
	if(m_pServer==NULL)
		return false;
	CServerTowerFile* pFile=(CServerTowerFile*)pTowerTypeFile;
	CBuffer buffer;
	if(!DownloadFileById("TowerFile",pFile->m_idSrvFileObj,&buffer))
		return false;
	if(buffer.GetLength()>0)
		pFile->FromBuffer(buffer);
	return true;
}
bool CTmsServer::UploadFileObject(int idFileObj, BUFFER_IO* pFileDataBuff,bool overwrite/*=true*/, bool compressed/*=false*/,char* errorMsg/*=NULL*/)
{
	if(m_pServer==NULL||pFileDataBuff==NULL)
		return false;
	UINT uiFileSize=(UINT)pFileDataBuff->Length;
	UINT uiLeastSize=uiFileSize;
	int indexpos=0;
	pFileDataBuff->SeekToBegin();
	const int FRAG_MAXSIZE=0x100000;	//每次上传最大不超过1M的文件，大文件分片上传T
	CHAR_ARRAY pool(FRAG_MAXSIZE);
	CBuffer buff(pool,FRAG_MAXSIZE);	//每次上传最大不超过5M的文件，大文件分片上传T
	while(uiLeastSize>0)
	{
		buff.ClearContents();
		UINT uiReadSize=min(uiLeastSize,FRAG_MAXSIZE);
		buff.Write(NULL,uiReadSize);
		pFileDataBuff->Read(pool,uiReadSize);
		if(!TMS.UploadFileObject(idFileObj,indexpos,&buff,false))
			break;	//上传失败
		indexpos+=uiReadSize;
		uiLeastSize-=uiReadSize;
	}
	if(uiLeastSize==0)
	{
		return TMS.FinalizeFileObjectData(idFileObj,uiFileSize,overwrite,errorMsg);
	}
	return false;
}
/*
上传文件时，应先
1.调用UploadFileObject，根据用户要求分片上传文件数据内容
2.文件全部内容上传成功后，调用FinalizeFileObjectData结束服务器端文件传输过程，并合并为一个完整的物理文件存到相应路径
*/
bool CTmsServer::UploadFileObject(int idFileObj, int startposition,CBuffer* pFileDataBuff, bool compressed)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__UploadFileObject inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idFileObj=idFileObj;
	inputPara.startposition=startposition;
	inputPara.byteArr=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> selflife(inputPara.byteArr);
	inputPara.byteArr->__ptr=(BYTE*)pFileDataBuff->GetBufferPtr();
	inputPara.byteArr->__size=pFileDataBuff->GetLength();
	inputPara.compressed=compressed;
	_TMSServ__UploadFileObjectResponse response;
	if(m_pServer->UploadFileObject(&inputPara,response)!=SOAP_OK)
		response.UploadFileObjectResult=false;
	return response.UploadFileObjectResult;
}
bool CTmsServer::FinalizeFileObjectData(int idFileObj, int file_length, bool isOverriteFile,char* errorMsg/*=NULL*/)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__FinalizeFileObjectData inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idFileObj=idFileObj;
	inputPara.file_USCORElength=file_length;
	inputPara.isOverriteFile=isOverriteFile;
	_TMSServ__FinalizeFileObjectDataResponse response;
	if(m_pServer->FinalizeFileObjectData(&inputPara,response)!=SOAP_OK||response.FinalizeFileObjectDataResult==NULL)
		return false;
	CString retCode;
	CMarkup xml(response.FinalizeFileObjectDataResult);
	xml.ResetMainPos();
	while(xml.FindChildElem())   
	{
		xml.IntoElem();
		CString elemname=xml.GetTagName();
		if(elemname.CompareNoCase("Return")==0)
		{
			retCode=xml.GetAttrib("retcode");
			CString attribstr=xml.GetAttrib("msg");
			if(errorMsg!=NULL&&attribstr.GetLength()>0)
				strcpy(errorMsg,attribstr);
			break;
		}
		xml.OutOfElem();
	}
	if(retCode.CompareNoCase("true")==0)
		return true;
	else
		return false;
}

UINT CTmsServer::OpenServerObjectDataProvider(int idObject,const char* cls_name,int* pVirtualFileObjId, bool compressed/*=false*/)
{
	if(m_pServer==NULL)
		return 0;
	_TMSServ__OpenServerObjectDataProvider inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idObject=idObject;
	if(cls_name!=NULL&&strcmp(cls_name,"TowerType")==0)
		inputPara.cls_USCOREname="TowerType";
	else
		return 0;
	inputPara.compressed=compressed;
	_TMSServ__OpenServerObjectDataProviderResponse response;
	if(m_pServer->OpenServerObjectDataProvider(&inputPara,response)!=SOAP_OK||response.OpenServerObjectDataProviderResult==NULL)
		return false;
	UINT datasize=0;
	CMarkup xml(response.OpenServerObjectDataProviderResult);
	xml.ResetMainPos();
	CXhChar100 objname="PpiModelProvider";
	while(xml.FindChildElem())   
	{
		xml.IntoElem();
		CString elemname=xml.GetTagName();
		if(elemname.CompareNoCase("PpiModelProvider")==0)
		{
			CString attribstr=xml.GetAttrib("fileObjId");
			if(attribstr.GetLength()==0)
				return false;
			if(pVirtualFileObjId)
				*pVirtualFileObjId=atoi(attribstr);
			attribstr=xml.GetAttrib("size");
			if(attribstr.GetLength()>0)
				datasize=atoi(attribstr);
			break;
		}
		xml.OutOfElem();
	}
	return datasize;
}

UINT CTmsServer::OpenTapModelDataProvider(int idTowerType,int* pVirtualFileObjId, bool compressed)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__OpenTapModelDataProvider inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idTowerType=idTowerType;
	inputPara.compressed=compressed;
	_TMSServ__OpenTapModelDataProviderResponse response;
	if(m_pServer->OpenTapModelDataProvider(&inputPara,response)!=SOAP_OK||response.OpenTapModelDataProviderResult==NULL)
		return false;
	UINT datasize=0;
	CMarkup xml(response.OpenTapModelDataProviderResult);
	xml.ResetMainPos();
	CXhChar100 objname="TapModelProvider";
	while(xml.FindChildElem())   
	{
		xml.IntoElem();
		CString elemname=xml.GetTagName();
		if(elemname.CompareNoCase("TapModelProvider")==0)
		{
			CString attribstr=xml.GetAttrib("fileObjId");
			if(attribstr.GetLength()==0)
				return false;
			if(pVirtualFileObjId)
				*pVirtualFileObjId=atoi(attribstr);
			attribstr=xml.GetAttrib("size");
			if(attribstr.GetLength()>0)
				datasize=atoi(attribstr);
			break;
		}
		xml.OutOfElem();
	}
	return datasize;
}
/*
下载文件时，应先
1.调用OpenFileObjectDataProvider为当前会话在服务器端创建数据提供者
2.调用DownloadFileObject，分片下载文件的相应数据
3.关才服务器端相关文件的数据提供服务，释放缓存
*/
//创建一个远程文件内容提供者对象，返回压缩前或后的文件数据流字节总长度
UINT CTmsServer::OpenFileObjectDataProvider(int idFileObj, bool compressed)
{
	if(m_pServer==NULL)
		return 0;
	_TMSServ__OpenFileObjectDataProvider inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idFileObj=idFileObj;
	inputPara.compressed=compressed;
	_TMSServ__OpenFileObjectDataProviderResponse response;
	if(m_pServer->OpenFileObjectDataProvider(&inputPara,response)!=SOAP_OK||response.OpenFileObjectDataProviderResult==NULL)
		return 0;
	return response.OpenFileObjectDataProviderResult;
}
UINT CTmsServer::DownloadFileObject(int idFileObj, int startposition, int download_size, bool compressed,
									BUFFER_IO* pFileDataBuff)
{
	if(m_pServer==NULL)
		return 0;
	_TMSServ__DownloadFileObject inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idFileObj=idFileObj;
	inputPara.startposition=startposition;
	inputPara.download_USCOREsize=download_size;
	inputPara.compressed=compressed;
	_TMSServ__DownloadFileObjectResponse response;
	if(m_pServer->DownloadFileObject(&inputPara,response)!=SOAP_OK||response.DownloadFileObjectResult==NULL
		||response.DownloadFileObjectResult->__size<=0||response.DownloadFileObjectResult->__ptr==NULL)
		return 0;
	return pFileDataBuff->Write(response.DownloadFileObjectResult->__ptr,response.DownloadFileObjectResult->__size);
}
bool CTmsServer::CloseFileObjectDataProvider(int idFileObj)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__CloseFileObjectDataProvider inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.idFileObj=idFileObj;
	_TMSServ__CloseFileObjectDataProviderResponse response;
	if(m_pServer->CloseFileObjectDataProvider(&inputPara,response)!=SOAP_OK||response.CloseFileObjectDataProviderResult==NULL)
		return false;
	return response.CloseFileObjectDataProviderResult;
}
UINT CTmsServer::DownloadFileObject(int idFileObj, bool compressed,BUFFER_IO* pFileDataBuff)
{
	if(m_pServer==NULL)
		return 0;
	UINT uiFileDataLength=OpenFileObjectDataProvider(idFileObj,false);
	if( uiFileDataLength<=0)
	{	//创建一个远程文件内容提供者对象，返回压缩前或后的文件数据流字节总长度
		AfxMessageBox("缺少服务器端的指向数据文件！");
		return 0;
	}
	int indexpos=0;
	UINT uiLeastSize=uiFileDataLength;
	const int FRAG_MAXSIZE=0x100000;	//每次上传最大不超过1M的文件，大文件分片上传T
	CHAR_ARRAY pool(FRAG_MAXSIZE);
	CBuffer buff(pool,FRAG_MAXSIZE);	//每次上传最大不超过1M的文件，大文件分片上传T
	CBuffer fileBuff(FRAG_MAXSIZE);
	BUFFER_IO* pToBuff=pFileDataBuff!=NULL?pFileDataBuff:&fileBuff;
	while(uiLeastSize>0)
	{
		buff.ClearContents();
		UINT uiDownloadSize=min(uiLeastSize,FRAG_MAXSIZE);
		if(DownloadFileObject(idFileObj,indexpos,uiDownloadSize,false,&buff)<uiDownloadSize)
			break;	//下载失败
		pToBuff->Write(buff.GetBufferPtr(),uiDownloadSize);
		indexpos+=uiDownloadSize;
		uiLeastSize-=uiDownloadSize;
	}
	return CloseFileObjectDataProvider(idFileObj);
}
int  CTmsServer::SaveOrUpdateObject(const char* clsName,void* pLocalObject,DWORD serialize/*=0*/,BYTE fromClientTMA0TAP1/*=0*/)
{
	if(SaveOrUpdateObjects(clsName,&pLocalObject,1,serialize,fromClientTMA0TAP1))
		return ((CServerObject*)pLocalObject)->Id;
	else
		return 0;
}

bool CTmsServer::SaveOrUpdateObjects(const char* clsName,IXhEnumerator* pIter,DWORD serialize/*=0*/,BYTE fromClientTMA0TAP1/*=0*/)
{
	if(m_pServer==NULL)
		return false;
	int id=0;
	int i=0,countOfObj=pIter->Count;
	CBuffer buffer(0x100000),filebuf(0x100000);
	_TMSServ__SaveOrUpdateObjects inputPara;
	inputPara.byteArr=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> selflife(inputPara.byteArr);
	inputPara.sessionId=m_idSession;
	_TMSServ__SaveOrUpdateObjectsResponse response;
	DYN_ARRAY<CServerObject*> localObjPtrArr(countOfObj);
	for(void* pObj=pIter->MoveFirst();pObj;pObj=pIter->MoveNext(),i++)
		localObjPtrArr[i]=(CServerObject*)pObj;
	if(strcmp(clsName,"TowerFile")==0)
	{
		IXhTemplEnumerator<CServerTowerFile>* pEnum=(IXhTemplEnumerator<CServerTowerFile>*)pIter;
		CServerTowerFile *pTowerFile;
		CServerTowerFile** ppLocalObjArr=(CServerTowerFile**)(CServerObject**)localObjPtrArr;
		for(i=0;i<countOfObj;i++)
		{
			pTowerFile=ppLocalObjArr[i];
			filebuf.ClearContents();
			pTowerFile->ToBuffer(filebuf);
			filebuf.SeekToBegin();
			buffer.Write(filebuf,filebuf.Length);
		}
		if(countOfObj==1)
			AgentServer.m_pActiveTowerFile=pTowerFile;
		inputPara.clsName="TowerFile";
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
		xmlformat.Append("     <ClassName value=\"TowerFile\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(" </SaveOrUpdate>");
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,localObjPtrArr,countOfObj);
	}
	else if(strcmp(clsName,"TowerType")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CServerTowerType::SERIALIZE_TMA:serialize;
		CServerTowerType** ppLocalObjArr=(CServerTowerType**)(CServerObject**)localObjPtrArr;
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerType *pTowerType=(CServerTowerType*)ppLocalObjArr[i];
			pTowerType->ToBuffer(buffer,true,dwSerialzeFlag);
		}
		inputPara.clsName="TowerType";
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
		xmlformat.Append("     <ClassName value=\"TowerType\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		//xmlformat.Append(" </SaveOrUpdate>");
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
		//{
		//ParseSaveOrUpdateServerTowerAssemblyRetStr(pTowerAssembly,response.SaveOrUpdateObjectsResult);
		//pTowerAssembly->m_id=atoi(response.SaveOrUpdateObjectsResult);
		//}
	}
	else if(strcmp(clsName,"TowerTypeFileGroup")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CServerTowerType::SERIALIZE_TMA:serialize;
		CServerTowerFileGroup** ppLocalObjArr=(CServerTowerFileGroup**)(CServerObject**)localObjPtrArr;
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerFileGroup *pTowerFileGroup=(CServerTowerFileGroup*)ppLocalObjArr[i];
			pTowerFileGroup->ToBuffer(buffer,true,dwSerialzeFlag);
		}
		inputPara.clsName="TowerTypeFileGroup";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		/* 范例 
		"<?xml version="1.0" encoding="utf-8"?>"
		" <SaveOrUpdate>"
		"     <ClassName value="TowerTypeFileGroup" />"
		"     <ObjectCount value="1" />"
		"     <serialize   value="1" />"
		" </SaveOrUpdate>");
		*/
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
			" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TowerTypeFileGroup\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		//xmlformat.Append(" </SaveOrUpdate>");
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	else if(strcmp(clsName,"TowerAssembly")==0)
	{
		buffer.WriteInteger(countOfObj);
		CAssemblyHeight** ppLocalObjArr=(CAssemblyHeight**)(CServerObject**)localObjPtrArr;
		char pool[2048]={0};
		CBuffer single(pool,2048);
		for(int i=0;i<countOfObj;i++)
		{
			CAssemblyHeight *pTowerAssembly=(CAssemblyHeight*)ppLocalObjArr[i];
			pTowerAssembly->ToBuffer(single);
			if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
				buffer.WriteInteger(single.GetLength());
			buffer.Write(single.GetBufferPtr(),single.GetLength());
		}
		inputPara.clsName="TowerAssembly";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
		//{
			//ParseSaveOrUpdateServerTowerAssemblyRetStr(pTowerAssembly,response.SaveOrUpdateObjectsResult);
			//pTowerAssembly->m_id=atoi(response.SaveOrUpdateObjectsResult);
		//}
	}
	else if(strcmp(clsName,"TowerPart")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CServerTowerPart::SERIALIZE_BASIC:serialize;
		CServerTowerPart** ppLocalObjArr=(CServerTowerPart**)(CServerObject**)localObjPtrArr;
		char OnePartPool[2048]={0};
		CBuffer single(OnePartPool,2048);
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerPart *pTowerPart=(CServerTowerPart*)ppLocalObjArr[i];
			single.ClearContents();
			pTowerPart->ToBuffer(single,true,dwSerialzeFlag);
			if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
				buffer.WriteInteger(single.GetLength());
			buffer.Write(single.GetBufferPtr(),single.GetLength());
		}
		inputPara.clsName="TowerPart";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
						" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TowerPart\" />");
		int currTowerTypeId=AgentServer.m_pActiveTowerType==NULL?0:AgentServer.m_pActiveTowerType->Id;
		if(currTowerTypeId<=0)
			return false;
		xmlformat.Append(CXhChar50("     <towerTypeId value=\"%d\" />",currTowerTypeId));
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(CXhChar50("     <srcFrom value=\"%d\" />",fromClientTMA0TAP1));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	else if(strcmp(clsName,"TapModel")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CTapModel::SERIALIZE_BASIC:serialize;
		CTapModel** ppLocalObjArr=(CTapModel**)(CServerObject**)localObjPtrArr;
		char OnePartPool[2048]={0};
		CBuffer single(OnePartPool,2048);
		for(int i=0;i<countOfObj;i++)
		{
			CTapModel *pTapModel=(CTapModel*)ppLocalObjArr[i];
			single.ClearContents();
			pTapModel->ToBuffer(single,true,dwSerialzeFlag);
			if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
				buffer.WriteInteger(single.GetLength());
			buffer.Write(single.GetBufferPtr(),single.GetLength());
		}
		inputPara.clsName="TapModel";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
			" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TapModel\" />");
		int currTowerTypeId=AgentServer.m_pActiveTowerType==NULL?0:AgentServer.m_pActiveTowerType->Id;
		if(currTowerTypeId<=0)
			return false;
		xmlformat.Append(CXhChar50("     <towerTypeId value=\"%d\" />",currTowerTypeId));
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	return id>0;
}
bool  CTmsServer::SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj/*=1*/,DWORD serialize/*=0*/,BYTE fromClientTMA0TAP1/*=0*/)
{
	if(m_pServer==NULL)
		return false;
	int id=0;
	CBuffer buffer(0x100000),filebuf(0x100000);
	_TMSServ__SaveOrUpdateObjects inputPara;
	inputPara.byteArr=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> selflife(inputPara.byteArr);
	inputPara.sessionId=m_idSession;
	_TMSServ__SaveOrUpdateObjectsResponse response;
	if(strcmp(clsName,"TowerFile")==0)
	{
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerFile *pTowerFile=(CServerTowerFile*)((CServerTowerFile**)ppLocalObjArr)[i];
			filebuf.ClearContents();
			pTowerFile->ToBuffer(filebuf);
			filebuf.SeekToBegin();
			buffer.Write(filebuf,filebuf.Length);
		}
		if(countOfObj==1)
			AgentServer.m_pActiveTowerFile=(CServerTowerFile*)((CServerTowerFile**)ppLocalObjArr)[0];
		inputPara.clsName="TowerFile";
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
		xmlformat.Append("     <ClassName value=\"TowerFile\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(" </SaveOrUpdate>");
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	else if(strcmp(clsName,"TowerType")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CServerTowerType::SERIALIZE_TMA:serialize;
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerType *pTowerType=(CServerTowerType*)((CServerTowerType**)ppLocalObjArr)[i];
			pTowerType->ToBuffer(buffer,true,dwSerialzeFlag);
		}
		inputPara.clsName="TowerType";
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
		xmlformat.Append("     <ClassName value=\"TowerType\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		//xmlformat.Append(" </SaveOrUpdate>");
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
		//{
		//ParseSaveOrUpdateServerTowerAssemblyRetStr(pTowerAssembly,response.SaveOrUpdateObjectsResult);
		//pTowerAssembly->m_id=atoi(response.SaveOrUpdateObjectsResult);
		//}
	}
	else if(strcmp(clsName,"TowerTypeFileGroup")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CServerTowerType::SERIALIZE_TMA:serialize;
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerFileGroup *pTowerFileGroup=(CServerTowerFileGroup*)(((CServerTowerFileGroup**)ppLocalObjArr)[i]);
			pTowerFileGroup->ToBuffer(buffer,true,dwSerialzeFlag);
		}
		inputPara.clsName="TowerTypeFileGroup";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		/* 范例 
		"<?xml version="1.0" encoding="utf-8"?>"
		" <SaveOrUpdate>"
		"     <ClassName value="TowerTypeFileGroup" />"
		"     <ObjectCount value="1" />"
		"     <serialize   value="1" />"
		" </SaveOrUpdate>");
		*/
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
			" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TowerTypeFileGroup\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		//xmlformat.Append(" </SaveOrUpdate>");
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	else if(strcmp(clsName,"TowerAssembly")==0)
	{
		buffer.WriteInteger(countOfObj);
		char pool[2048]={0};
		CBuffer single(pool,2048);
		for(int i=0;i<countOfObj;i++)
		{
			CAssemblyHeight *pTowerAssembly=(CAssemblyHeight*)((CAssemblyHeight**)ppLocalObjArr)[i];
			pTowerAssembly->ToBuffer(single);
			if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
				buffer.WriteInteger(single.GetLength());
			buffer.Write(single.GetBufferPtr(),single.GetLength());
		}
		inputPara.clsName="TowerAssembly";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
		//{
			//ParseSaveOrUpdateServerTowerAssemblyRetStr(pTowerAssembly,response.SaveOrUpdateObjectsResult);
			//pTowerAssembly->m_id=atoi(response.SaveOrUpdateObjectsResult);
		//}
	}
	else if(strcmp(clsName,"TowerPart")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CServerTowerPart::SERIALIZE_BASIC:serialize;
		char OnePartPool[2048]={0};
		CBuffer single(OnePartPool,2048);
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerPart *pTowerPart=(CServerTowerPart*)((CServerTowerPart**)ppLocalObjArr)[i];
			single.ClearContents();
			pTowerPart->ToBuffer(single,true,dwSerialzeFlag);
			if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
				buffer.WriteInteger(single.GetLength());
			buffer.Write(single.GetBufferPtr(),single.GetLength());
		}
		inputPara.clsName="TowerPart";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
						" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TowerPart\" />");
		int currTowerTypeId=AgentServer.m_pActiveTowerType==NULL?0:AgentServer.m_pActiveTowerType->Id;
		if(currTowerTypeId<=0)
			return false;
		xmlformat.Append(CXhChar50("     <towerTypeId value=\"%d\" />",currTowerTypeId));
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(CXhChar50("     <srcFrom value=\"%d\" />",fromClientTMA0TAP1));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			return CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	else if(strcmp(clsName,"TapModel")==0)
	{
		DWORD dwSerialzeFlag=serialize==0?CTapModel::SERIALIZE_TAP_PREFER:serialize;
		char OnePartPool[2048]={0};
		CBuffer single(OnePartPool,2048);
		for(int i=0;i<countOfObj;i++)
		{
			CTapModel *pTapModel=(CTapModel*)((CServerObject**)ppLocalObjArr)[i];
			single.ClearContents();
			pTapModel->ToBuffer(single,true,dwSerialzeFlag);
			if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
				buffer.WriteInteger(single.GetLength());
			buffer.Write(single.GetBufferPtr(),single.GetLength());
		}
		inputPara.clsName="TapModel";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
						" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TapModel\" />");
		int currTowerTypeId=AgentServer.m_pActiveTowerType==NULL?0:AgentServer.m_pActiveTowerType->Id;
		if(currTowerTypeId<=0)
			return false;
		xmlformat.Append(CXhChar50("     <towerTypeId value=\"%d\" />",currTowerTypeId));
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
			" </SaveOrUpdate>",dwSerialzeFlag);
		xmlformat.Append(xmlfinal);
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)!=SOAP_OK)
			return false;
		if(!CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj))
			return false;
		for(int i=0;i<countOfObj;i++)
		{
			CTapModel *pTapModel=(CTapModel*)((CServerObject**)ppLocalObjArr)[i];
			//上传Tid文件
			if(pTapModel->tidfile.m_idSrvFileObj>0)
				TMS.UploadFileObject(pTapModel->tidfile.m_idSrvFileObj,&pTapModel->tidfile.m_dataBuff,true,false);
			//上传TapShell文件
			if(pTapModel->shellfile.m_idSrvFileObj>0)
				TMS.UploadFileObject(pTapModel->shellfile.m_idSrvFileObj,&pTapModel->shellfile.m_dataBuff,true,false);
			//上传塔位实例文件
			if(pTapModel->pSrvTowerType!=NULL&&pTapModel->pSrvTowerType->listAssemblyHeights.Count>0)
			{
				for(CAssemblyHeight* pHeight=pTapModel->pSrvTowerType->listAssemblyHeights.EnumObjectFirst();pHeight;
					pHeight=pTapModel->pSrvTowerType->listAssemblyHeights.EnumObjectNext())
				{
					if(pHeight->m_idSrvFileObj<=0)
						continue;
					if(!TMS.UploadFileObject(pHeight->m_idSrvFileObj,&pHeight->m_dataBuff,true,false))
						logerr.Log("error");
				}
			}
		}
	}
	return true;
}
bool CTmsServer::InitTowerPartsVersionSerial(int towerTypeId)
{
	if(m_pServer==NULL)
		return false;
	//GetTowerPartsLabelAndVersionSerial(AgentServer.m_pActiveTowerType->Id);
	_TMSServ__GetTowerPartsLabelAndVersionSerial inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.towerTypeId=towerTypeId;
	_TMSServ__GetTowerPartsLabelAndVersionSerialResponse response;
	if(m_pServer->GetTowerPartsLabelAndVersionSerial(&inputPara,response)!=SOAP_OK||response.GetTowerPartsLabelAndVersionSerialResult==NULL
		||response.GetTowerPartsLabelAndVersionSerialResult->__size<=0||response.GetTowerPartsLabelAndVersionSerialResult->__ptr==NULL)
		return false;
	CBuffer rsltbuf(response.GetTowerPartsLabelAndVersionSerialResult->__ptr,response.GetTowerPartsLabelAndVersionSerialResult->__size);
	hashPartsVerSerial.Empty();
	rsltbuf.SeekToBegin();
	int count=rsltbuf.ReadInteger();
	VERSION_SERIAL verserial;
	for(int i=0;i<count;i++)
	{
		CXhChar16 label;
		rsltbuf.ReadString(label,17);
		rsltbuf.ReadByte(&verserial.activePpi0Tid1);
		rsltbuf.ReadInteger(&verserial.uiPpiVerSerial);
		rsltbuf.ReadInteger(&verserial.uiTidVerSerial);
		hashPartsVerSerial.SetValue(label,verserial);
	}
	return count>0;
}
bool CTmsServer::MatchTowerParts(IXhEnumerator* pIter,int towerTypeId,DWORD serialize/*=0*/)
{
	if(m_pServer==NULL)
		return false;
	int id=0;
	int i=0,countOfObj=pIter->Count;
	CBuffer buffer(0x100000),filebuf(0x100000);
	_TMSServ__MatchTowerParts inputPara;
	inputPara.byteArr=new xsd__base64Binary();
	CSelfDestoryObject<xsd__base64Binary> byteslife(inputPara.byteArr);	//跳出函数时自动销毁对象
	inputPara.sessionId=m_idSession;
	_TMSServ__MatchTowerPartsResponse response;
	DYN_ARRAY<CServerObject*> localObjPtrArr(countOfObj);
	for(void* pObj=pIter->MoveFirst();pObj;pObj=pIter->MoveNext(),i++)
		localObjPtrArr[i]=(CServerObject*)pObj;
	DWORD dwSerialzeFlag=serialize==0?CServerTowerPart::SERIALIZE_BASIC:serialize;
	CServerTowerPart** ppLocalObjArr=(CServerTowerPart**)(CServerObject**)localObjPtrArr;
	char OnePartPool[2048]={0};
	CBuffer single(OnePartPool,2048);
	for(int i=0;i<countOfObj;i++)
	{
		CServerTowerPart *pTowerPart=(CServerTowerPart*)ppLocalObjArr[i];
		single.ClearContents();
		pTowerPart->ToBuffer(single,true,dwSerialzeFlag);
		if(countOfObj>1)	//单构件缓存与多构件缓存结构不同
			buffer.WriteInteger(single.GetLength());
		buffer.Write(single.GetBufferPtr(),single.GetLength());
	}
	inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
	inputPara.byteArr->__size=buffer.GetLength();
	CXhChar500 xmlformat;
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <MatchTowerPart>";
	xmlformat.Copy(xmlheader);
	xmlformat.Append("     <ClassName value=\"TowerPart\" />");
	int currTowerTypeId=AgentServer.m_pActiveTowerType==NULL?0:AgentServer.m_pActiveTowerType->Id;
	if(currTowerTypeId<=0)
		return false;
	xmlformat.Append(CXhChar50("     <towerTypeId value=\"%d\" />",currTowerTypeId));
	xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
	CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
		" </MatchTowerPart>",dwSerialzeFlag);
	xmlformat.Append(xmlfinal);
	inputPara.xmlformat=xmlformat;

	if(m_pServer->MatchTowerParts(&inputPara,response)==SOAP_OK)
		return CServerObject::ExtractObjArrFromXmlStr(response.MatchTowerPartsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	return false;
}
bool CTmsServer::DeleteObject(const char* clsName,int objId)
{
	if(m_pServer==NULL)
		return false;
	_TMSServ__DeleteObject inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.objId=objId;
	_TMSServ__DeleteObjectResponse response;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerType")==0)	//当前用户权限范围内的塔型
		inputPara.clsName="TowerType";
	else if(strcmp(clsName,"TowerFile")==0)
		inputPara.clsName="TowerFile";
	else if(strcmp(clsName,"TowerAssembly")==0)
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)
		inputPara.clsName="AssemblePart";
	else 
		return false;
	if(m_pServer->DeleteObject(&inputPara,response)!=SOAP_OK||!response.DeleteObjectResult)
		return false;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CTmsServer::QueryStateTowerTypes(BUFFER_IO* pOutLocalBuff,int state,int fromdate/*=0*/,
									  int todate/*=0*/,DWORD dwSerialzeFlag/*=0*/,bool bFilterByUserId/*=true*/)
{
	if(m_pServer==NULL)
		return false;
	CXhChar500 xmlscope;
    char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <query>";
	xmlscope.Copy(xmlheader);
	if(fromdate>0)
		xmlscope.Append(CXhChar50("     <fromdate value=\"%4d-%2d-%2d\" />",fromdate/10000,(fromdate%10000)/100,fromdate%100));
	if(todate>0)
		xmlscope.Append(CXhChar50("     <todate value=\"%4d-%2d-%2d\" />",todate/10000,(todate%10000)/100,todate%100));
	xmlscope.Append(CXhChar50("     <state value=\"%d\" />",state));
	if(!bFilterByUserId)
		xmlscope.Append(CXhChar50("     <filter value=\"0\" />"));
	CXhChar100 xmlfinal("     <serialize value=\"0x%x\" />"\
		" </query>",dwSerialzeFlag);
	xmlscope.Append(xmlfinal);
	return TMS.QueryObjects("TowerType",xmlscope,pOutLocalBuff);
}
bool CTmsServer::QueryStateManuTasks(BUFFER_IO* pOutLocalBuff,int state/*=1*/)
{
	if(m_pServer==NULL)
		return false;
	CXhChar500 xmlscope;
	char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
		" <query>";
	xmlscope.Copy(xmlheader);
	xmlscope.Append(CXhChar50("     <state value=\"%d\" />",state));
	CXhChar100 xmlfinal("     <IncTowerType value=\"1\" />"\
		" </query>");
	xmlscope.Append(xmlfinal);
	return TMS.QueryObjects("ManuElemTask",xmlscope,pOutLocalBuff);
}
int CTmsServer::QueryProcessParts(BUFFER_IO* pOutLocalBuff,int towerTypeId,CXhChar16* arrLabels,int labelCount/*=1*/)
{
	if(m_pServer==NULL)
		return 0;
	CXhChar500 xmlscope;
    char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <query>";
	xmlscope.Copy(xmlheader);
	xmlscope.Append(CXhChar50("     <TowerTypeId value=\"%d\" />",towerTypeId));
	if(arrLabels==NULL||labelCount<=0)
		return 0;
	xmlscope.Append(CXhChar50("     <QueryLabels count=\"%d\">",labelCount));
	for(int i=0;i<labelCount;i++)
		xmlscope.Append(CXhChar50("     <Label value=\"%s\" />",(char*)arrLabels[i]));
	xmlscope.Append("     </QueryLabels\">");
    char* xmlfinal="     <serialize value=\"0xffffffff\" />"\
		" </query>";
	xmlscope.Append(xmlfinal);
	return TMS.QueryObjects("TowerPart",xmlscope,pOutLocalBuff);
}

bool CTmsServer::UploadServerFile(void* pFile,const char* file_cls_name,
	BUFFER_IO *pSrcBuff,bool overwrite/*=true*/)
{
	if(m_pServer==NULL)
		return false;
	CServerFile *pServerFile=(CServerFile*)pFile;
	int result=TMS.SaveOrUpdateObject(file_cls_name,pFile);
	bool upload_ok=false;
	if(result>0)
	{
		UINT uiFileSize=(UINT)pSrcBuff->Length;
		UINT uiLeastSize=uiFileSize;
		int indexpos=0;
		pSrcBuff->SeekToBegin();
		CHAR_ARRAY pool(0x500000);
		CBuffer buff(pool,0x500000);	//每次上传最大不超过5M的文件，大文件分片上传T
		while(uiLeastSize>0)
		{
			buff.ClearContents();
			UINT uiReadSize=min(uiLeastSize,0x500000);
			buff.Write(NULL,uiReadSize);
			pSrcBuff->Read(pool,uiReadSize);
			if(!TMS.UploadFileObject(pServerFile->m_idSrvFileObj,indexpos,&buff,false))
				break;	//上传失败
			indexpos+=uiReadSize;
			uiLeastSize-=uiReadSize;
		}
		if(uiLeastSize==0)
			upload_ok=TMS.FinalizeFileObjectData(pServerFile->m_idSrvFileObj,uiFileSize,overwrite);
	}
	return upload_ok;
}
bool CTmsServer::DownloadServerFile(void* pFile,BUFFER_IO* pDestBuff/*=NULL*/)
{
	if(m_pServer==NULL)
		return false;
	UINT uiFileDataLength=0;
	CServerFile *pServerFile=(CServerFile*)pFile;
	if( pServerFile->m_idSrvFileObj<=0||	
		//创建一个远程文件内容提供者对象，返回压缩前或后的文件数据流字节总长度
		((uiFileDataLength=TMS.OpenFileObjectDataProvider(pServerFile->m_idSrvFileObj, false))==0))
	{
		AfxMessageBox("缺少服务器端的指向数据文件！");
		return false;
	}
	int indexpos=0;
	UINT uiLeastSize=uiFileDataLength;
	CHAR_ARRAY pool(0x500000);
	CBuffer buff(pool,0x500000);	//每次上传最大不超过5M的文件，大文件分片上传T
	CBuffer fileBuff(0x100000);
	BUFFER_IO* pToBuff=pDestBuff!=NULL?pDestBuff:&fileBuff;
	//pToBuff->ClearBuffer();
	while(uiLeastSize>0)
	{
		buff.ClearContents();
		UINT uiDownloadSize=min(uiLeastSize,0x500000);
		if(TMS.DownloadFileObject(pServerFile->m_idSrvFileObj,indexpos,uiDownloadSize,false,&buff)<uiDownloadSize)
			break;	//下载失败
		pToBuff->Write(buff.GetBufferPtr(),uiDownloadSize);
		indexpos+=uiDownloadSize;
		uiLeastSize-=uiDownloadSize;
	}
	TMS.CloseFileObjectDataProvider(pServerFile->m_idSrvFileObj);
	if(uiLeastSize==0)
		return true;
	else
		AfxMessageBox("数据文件下载失败!");
	return false;
}
#endif
