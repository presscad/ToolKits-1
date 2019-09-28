#pragma once

#ifdef __CONNECT_REMOTE_SERVER_
#include "Buffer.h"
#include "soapH.h"
#include "soapTMSServiceSoapProxy.h"
#include "XhCharString.h"
#include "list.h"
#include "HashTable.h"

class CTmsServer
{
public:
	int m_idSession;
	TMSServiceSoapProxy* m_pServer;
	struct VERSION_SERIAL{
		BYTE activePpi0Tid1;	//0.ppi为最新 1.Tid为最新 2.已同步
		UINT uiPpiVerSerial;
		UINT uiTidVerSerial;
		VERSION_SERIAL(){activePpi0Tid1=uiPpiVerSerial=uiPpiVerSerial=0;}
	};
	CHashStrList<VERSION_SERIAL> hashPartsVerSerial;
public:
	CTmsServer(void);
	~CTmsServer(void);
public:
	bool InitTowerPartsVersionSerial(int towerTypeId);
	//clsName有效值为:"TowerTypeFileGroup","TowerTypeFile","TowerAssembly","TowerPart","AssemblePart",
	bool QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff);
	bool DownloadObjects(const char* clsName, int* idarr, int countOfObj=1,UINT serialize=0,BUFFER_IO* pExterLocalBuff=NULL);
	bool DownloadFileById(const char* clsName,int fileId,BUFFER_IO* pExterLocalBuff);
 	bool DeleteObject(const char* clsName,int objId);
	int  SaveOrUpdateObject(const char* clsName,void* pLocalObject,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);	//serialize=0表示取默认值
	bool SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj=1,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);
	bool SaveOrUpdateObjects(const char* clsName,IXhEnumerator* pIter,DWORD serialize=0,BYTE fromClientTMA0TAP1=0);
	bool MatchTowerParts(IXhEnumerator* pIter,int towerTypeId,DWORD serialize=0);
public:	//文件上传下载
	/// <summary>
	/// 保存文件
	/// </summary>
	/// <param name="sessionId"></param>
	/// <param name="clsName"></param>
	/// <param name="byteArr"></param>
	/// <returns></returns>
	bool UploadFileObject(int idFileObj, BUFFER_IO* pFileDataBuff,bool overwrite=true, bool compressed=false,char* errorMsg=NULL);
	bool UploadFileObject(int idFileObj, int startposition,CBuffer* pFileDataBuff, bool compressed=false);
	bool FinalizeFileObjectData(int idFileObj, int file_length, bool isOverriteFile,char* errorMsg=NULL);
	//创建一个指定塔型Tap试组装模型的虚拟远程文件内容提供者对象，返回压缩前或后的虚拟文件数据流字节总长度
	UINT OpenTapModelDataProvider(int idTowerType,int* pVirtualFileObjId, bool compressed=false);
	//创建一个指定ServerObj的虚拟远程文件内容提供者对象，返回压缩前或后的虚拟文件数据流字节总长度
	UINT OpenServerObjectDataProvider(int idObject,const char* cls_name,int* pVirtualFileObjId, bool compressed=false);
	//创建一个远程文件内容提供者对象，返回压缩前或后的文件数据流字节总长度
	UINT OpenFileObjectDataProvider(int idFileObj, bool compressed);
	UINT DownloadFileObject(int idFileObj, int startposition, int download_size, bool compressed,BUFFER_IO* pFileDataBuff);
	UINT DownloadFileObject(int idFileObj, bool compressed,BUFFER_IO* pFileDataBuff);
	bool CloseFileObjectDataProvider(int idFileObj);
public:
	void SetServerUrl(const char* url);
	bool LoginUser(const char* userName, const char* password, const char* fingerprint=NULL);
	bool DownloadTowerTypeFile(void* pTowerTypeFile);
	bool QueryStateTowerTypes(BUFFER_IO* pOutLocalBuff,int state=1,int fromdate=0,int todate=0,DWORD dwSerialzeFlag=0,bool bFilterByUserId=true);
	int  QueryProcessParts(BUFFER_IO* pOutLocalBuff,int towerTypeId,CXhChar16* arrLabels,int labelCount=1);
	bool GetObjectsVersion(const char* clsName, int* idarr, int countOfObj=1,UINT* ver_arr=NULL);
	bool QueryStateManuTasks(BUFFER_IO* pOutLocalBuff,int state=1);
	//
	bool UploadServerFile(void* pServerFile,const char* file_cls_name,BUFFER_IO *pSrcBuff,bool overwrite=true);
	bool DownloadServerFile(void* pServerFile,BUFFER_IO* pDestBuff=NULL);
};
extern CTmsServer TMS;
//extern SOAP_NMAC struct Namespace namespaces[];
#endif
