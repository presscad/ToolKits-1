#ifndef __SERVER_TOWERTYPE_H_
#define __SERVER_TOWERTYPE_H_
#include "XhCharString.h"
#include "Buffer.h"
#include "HashTable.h"
#include "f_ent_list.h"
#include "list.h"
#include "MemberProperty.h"
#include "Markup.h"
#include "ServerObject.h"

class CServerTowerFile;
class CServerTowerFileGroup;
typedef void* (*DefCreateLocalTowerFileFunc)(CServerTowerFile *pServerTowerFile);
typedef void (*DefDestroyLocalTowerFileFunc)(void *pLocalTowerFile);
typedef void* (*DefCreateLocalFileGroupFunc)(CServerTowerFileGroup *pServerFileGroup);
typedef void (*DefDestroyLocalFileGroupFunc)(void *pLocalFileGroup);
class CServerTowerFileGroup;

#include "ProcessPart.h"
class CServerTowerPart :  public CServerObject
{
protected:
	BYTE _ciPartType;				//构件类型
	CProcessAngle angle;
	CProcessPlate plate;
	char* ppifiledata;
	UINT _uiPpiFileLength;
public://序列化标识掩码
	static const UINT SERIALIZE_LABELnID	= 0x01;	    //塔型工件标识信息
	static const UINT SERIALIZE_BASICNONEID	= 0x02;	    //塔型工件基本信息（Id及编号除外）
	static const UINT SERIALIZE_BASIC	 = 0x03;	    //塔型工件基本信息
	static const UINT SERIALIZE_PROCESSDATA = 0x04;	    //塔型数据分组文件信息
	static const UINT SERIALIZE_PROCESSFILEDATA = 0x08;	    //塔型数据分组文件信息
	static const UINT SERIALIZE_TMA = 0x0B;				//塔型数据分组文件信息
public:
	DECLARE_READONLY_PROPERTY(CProcessPart&,ppi);
	int m_idTowerType;
	int m_idSrvFileObj;	//PPI文件在文件服务器存储的实际物理文件Id;
	static const BYTE SYNC_NONE		= 0x00;
	static const BYTE SYNC_UPLOAD	= 0x01;
	static const BYTE SYNC_DOWNLOAD = 0x02;
	static const BYTE SYNC_JUSTIFY	= 0x04;
	BYTE m_cbSyncState;	//0x00不需要同步；0x01需要上传新版本至服务器；0x02服务器有更新版本需要下载到本地；0x04服务器有更新版本需要确认本地是否修改后决定下载或上传
	struct VERSION_SERIAL{
		UINT uiPpiVerSerial;
		UINT uiTidVerSerial;
		VERSION_SERIAL(){uiPpiVerSerial=uiPpiVerSerial=0;}
	}VerSerial;
	UINT m_uiCurrPpiVerSerial;
	UINT m_uiCurrTidVerSerial;
	CServerTowerPart(UINT key);
	CServerTowerPart(CProcessPart* pPPIPart=NULL);
	~CServerTowerPart();
	const char* get_PpiFileData(){return ppifiledata;};
	UINT get_PpiFileLength(){return _uiPpiFileLength;};
	__declspec(property(get=get_PpiFileLength)) UINT PpiFileLength;
	__declspec(property(get=get_PpiFileData)) const char* PpiFileData;
	bool ClonePart(CServerTowerPart *pNewPart);
	void InitByPPIPart(CProcessPart* pPPIPart);
	virtual void FromPPIBuffer(CBuffer &buffer);
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
typedef CServerTowerPart* CServerTowerPartPtr;
class CServerFile :  public CServerObject
{
public://序列化标识掩码
	static const UINT SERIALIZE_BASIC = 0x01;	//共享使用的塔型基本信息
	static const UINT SERIALIZE_FILEDATA = 0x02;//共享使用的塔型基本信息
	static const UINT SERIALIZE_ALL = 0x03;		//共享使用的塔型基本信息
public:
	CBuffer m_dataBuff;
	CXhChar200 m_sFileName;
	int m_idSrvFileObj;	//文件在文件服务器存储的实际物理文件Id;
	bool m_bHasPhysicalFile;
	DWORD dwVersion;
	CXhChar50 m_sDescription;
	CServerFile();
	virtual bool ExtractFromXmlStr(CMarkup& xml);
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};

class CTidFile : public CServerFile
{
public:
	int m_idTowerType;
	CTidFile();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};	
class CTapShellFile : public CServerFile
{
public:
	int m_idTowerType;
	CTapShellFile();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
class CServerTowerFile : public CServerFile
{
	friend CServerTowerFileGroup;
	void *m_pLocalTowerFile;
public:
	static DefCreateLocalTowerFileFunc CreateLocalTowerFileFunc;
	static DefDestroyLocalTowerFileFunc DestroyLocalTowerFileFunc;
public:
	CServerTowerFile();
	~CServerTowerFile();
	int m_idTowerType;
	int m_idFileGroup;
	CServerTowerFileGroup *m_pBelongFileGroup;
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void* GetLocalTowerFile();
	void* BelongModel();
	BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
typedef CServerTowerFile* CServerTowerFilePtr;
class CServerTowerType;
class CServerTowerFileGroup : public CServerObject
{
	void *m_pLocalFileGroup;
	CServerTowerFile* m_pActiveFile;
public:
	static DefCreateLocalFileGroupFunc CreateLocalFileGroupFunc;
	static DefDestroyLocalFileGroupFunc DestroyLocalFileGroupFunc;
public://序列化标识掩码
	static const UINT SERIALIZE_BASIC = 0x01;	        //共享使用的塔型基本信息
	static const UINT OPER_USER = 0x02;					//塔型数据分组文件信息
	static const UINT OPER_LOCALTAPID = 0x04;					//塔型数据分组文件信息
	static const UINT SERIALIZE_ATTACHFILES = 0x08;	    //塔型数据分组文件信息
	static const UINT SERIALIZE_TMA = 0x0B;	            //共享使用的塔型基本信息
	static const UINT SERIALIZE_TAP = 0x0D;	            //共享使用的塔型基本信息
public:
	CServerTowerFileGroup();
	~CServerTowerFileGroup();
	CXhChar200 m_sName;
	CXhChar200 m_sDescription;
	CXhChar200 m_sSegStr;		//段号范围
	CXhChar200 shareSegIncStr;		//多个文件共同有效的段号(常用于不同呼高塔脚编在同一段号情况)
	CXhChar200 priorPartLabelStr;	//文件中不受有效段号范围的特定件号清单字符串
	UINT m_idActiveFile;		//激活文件的远程服务器数据库Id
	UINT m_uiActFileVerSerial;	//当前激活文件的版本流水号
	UINT m_localIdInTapModel;
	DECLARE_READONLY_VIRTUAL_PROPERTY(CServerTowerFile* ,pActiveFile);
	
	int m_idTowerType;
	CServerTowerType *m_pBelongTowerType;
	//
	CXhChar50 segIncStr;		//配基字符串
	//CHashList<PRIOR_PART> hashPriorPartByPartId;
	CHashListEx<CServerTowerFile> hashTowerFiles;//键值:File.Id;

	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void ToBufferForPrjTowerType(CBuffer &buffer);
	void FromBufferForAssemblyTowerType(CBuffer &buffer);
	void* GetLocalFileGroup();
	void* BelongModel();
	BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
class CAssemblyHeight : public CServerFile
{
public://序列化标识掩码
	static const UINT SERIALIZE_BASIC	 = 0x01;	    //塔型工件基本信息
	static const UINT SERIALIZE_INFOFILEDATA = 0x02;	    //塔型数据分组文件信息
	static const UINT SERIALIZE_ALL = 0x03;	    //塔型数据分组文件信息
public:
	int m_idTowerType;
	int m_idTidFile;
	UINT localIdInTapModel;
	CServerTowerType *m_pBelongTowerType;
	void *m_pTowerInstance;	//关联塔例
	CXhChar16 placeName;
	CXhChar50 heightName;
	CXhChar100 segIncStr;
	double m_fAntiTheftHeight;
	int m_nManuCount;	//加工基数
	int m_iFoundationStyle;
	CAssemblyHeight();
	~CAssemblyHeight();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
class CTapModel : public CServerObject
{
	CServerTowerType* m_pMapSrvTowerType;
public://序列化标识掩码
	static const UINT SERIALIZE_BASIC = 0x01;		    //共享使用的塔型基本信息
	static const UINT SERIALIZE_TIDFILE = 0x02;			//共享使用的塔型基本信息
	static const UINT SERIALIZE_TAPSHELLFILE = 0x04;    //共享使用的塔型基本信息
	static const UINT SERIALIZE_TOWERFILES = 0x08;	    //塔型数据分组文件信息
	static const UINT SERIALIZE_ASSEMBLYHEIGHT = 0x10;	//塔型装配塔位呼高实例信息
	static const UINT SERIALIZE_TAP_PREFER = 0x1F;		//共享使用的塔型基本信息
	static const UINT SERIALIZE_TIDFILE_DATA = 0x20;			//共享使用的塔型基本信息
	static const UINT SERIALIZE_TAPSHELLFILE_DATA = 0x40;		//共享使用的塔型基本信息
	static const UINT SERIALIZE_TOWERFILES_DATA = 0x80;			//塔型数据分组文件信息
	static const UINT SERIALIZE_ASSEMBLYHEIGHT_DATA = 0x0100;	//塔型装配塔位呼高实例信息
public:
	CTapModel(CServerTowerType* pMapSrvTowerType);
	CXhChar50 GetServerClassName(){return m_sServerClassName;}
	DECLARE_READONLY_VIRTUAL_PROPERTY(CServerTowerType* ,pSrvTowerType);
	//
	CTidFile tidfile;
	CTapShellFile shellfile;
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
class CServerTowerType :  public CServerObject
{
public://序列化标识掩码
	static const UINT SERIALIZE_BASIC = 0x01;	        //共享使用的塔型基本信息
	static const UINT SERIALIZE_FILEGROUP = 0x02;	    //塔型数据分组文件信息
	static const UINT SERIALIZE_ASSEMBLYHEIGHT = 0x04;	//塔型装配塔位呼高实例信息
	static const UINT SERIALIZE_TMA = 0x03;	            //共享使用的塔型基本信息
	static const UINT SERIALIZE_TAP = 0x07;	            //共享使用的塔型基本信息
public:
	int state;	//1.在放样塔型;2.在试组塔型;3.已存档塔型
	int m_idBelongProject;	//归属工程
	CServerTowerType();
	CXhChar200 m_sName;
	CXhChar200 m_sProjName;
	CTidFile m_tidFile;
	ATOM_LIST<CServerTowerFileGroup> m_towerFileGroupList;
	//CAssemblyHeight
	//CXhChar16 placeName;
	//CXhChar50 heightName;
	//CXhChar100 segIncStr;
	CXhSimpleList<CAssemblyHeight> listAssemblyHeights;
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true, DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void* BelongModel(){return NULL;}
	virtual bool ExtractFromXmlStr(CMarkup& xml);
};
class CServerManuTask : public CServerObject
{
public:
	CServerManuTask();
	CXhChar200 m_sName;
	CHashListEx<CServerTowerType> hashTowerTypes;
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true, DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
class CServerContents
{
public:
	CServerTowerFile* m_pActiveTowerFile;
	CServerTowerType* m_pActiveTowerType;
public:
	CHashListEx<CServerTowerType> hashTowerTypes;
	CHashListEx<CServerManuTask> hashManuTasks;
	CServerContents();
	void ParseAndUpdateTowerTypesFromBuffer(CBuffer &buffer,int iStateTowerType=0,DWORD dwSerialzeFlag=0);
	void ActivateTowerFile(CServerTowerFile *pTowerFile);
	void ActivateTowerType(CServerTowerType *pTowerType);
	void ParseProcessPartsFromBuffer(CBuffer &buffer,CHashListEx<CProcessPart>* pHashProcessParts);
	void ParseManuTasksFromBuffer(CBuffer &buffer,CHashListEx<CServerManuTask>* pHashTasks);
	CServerTowerFile *GetActiveTowerFile();
	CServerTowerType *GetActiveTowerType();
};
extern CServerContents AgentServer;
template <class TYPE,class TYPE2> class CServerContent
{
	TYPE2 *m_pActiveObj;
public:
	CServerContent(){m_pActiveObj=NULL;}
	~CServerContent(){m_pActiveObj=NULL;}

	ATOM_LIST<TYPE> m_objList;
	TYPE2 *GetActiveObj(){return m_pActiveObj;}
	TYPE2 *ActivateObj(TYPE2 *pObj)
	{
		m_pActiveObj=pObj;
		return m_pActiveObj;
	}
	void FromBuffer(CBuffer &buffer)
	{
		m_pActiveObj=NULL;
		m_objList.Empty();
		int nCount=0;
		buffer.ReadInteger(&nCount);
		for(int i=0;i<nCount;i++)
		{
			TYPE *pObj=m_objList.append();
			pObj->FromBuffer(buffer);
		}
	}
};

class CServerAssemblePart :  public CServerObject
{
public:
	CServerAssemblePart(){m_sServerClassName="AssemblePart";};
	~CServerAssemblePart();
	void ToBuffer(CBuffer &buffer, bool bIncludeId=true, DWORD dwSerialzeFlag=0);
	void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};

#endif