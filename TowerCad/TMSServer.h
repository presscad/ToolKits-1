#include "soapTAPServiceSoapProxy.h"
#include "Buffer.h"
#include "ServerTowerType.h"

class CRemoteServer
{
	TAPServiceSoapProxy *m_pServer;
	int m_idSession;
	char m_sServerURL[MAX_PATH];
public:
	static  const   char*  CLASS_SERVER_OBJJECT ;
	static  const   char*  CLASS_SERVER_FILE;
	static  const   char*  CLASS_TOWER_ASSMBLY;
	static  const   char*  CLASS_TOWER_PART;
	static  const   char*  CLASS_ASSMBLY_PART;
	//
	CRemoteServer(const char* serverIp=NULL );
	~CRemoteServer();
	void InitServer(const char* serverIp);
	bool ValidServer();
	bool LoginServer(const char *userName,const char *password);
	bool QueryAllObjects(const char* clsName,BUFFER_IO* pExterLocalBuff);
	bool DownloadObjById(const char* clsName,int id,BUFFER_IO* pExterLocalBuff);
	int SaveOrUpdateObject(CServerObject* pLocalObj);
	bool DeleteObject(const char* clsName,int objId);
	bool DeleteObject(CServerObject*  pLocalObj);
	static bool IsSupportType(const char *clsName);
};
const   char*  CRemoteServer::CLASS_SERVER_OBJJECT			   = "CServerObject";
const   char*  CRemoteServer::CLASS_SERVER_FILE			       = "ServerFile";
const   char*  CRemoteServer::CLASS_TOWER_ASSMBLY              =  "TowerAssembly";
const   char*  CRemoteServer::CLASS_TOWER_PART                 =  "TowerAssembly";
const   char*  CRemoteServer::CLASS_ASSMBLY_PART               =  "AssemblePart";