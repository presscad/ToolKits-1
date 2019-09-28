#pragma once;
#include "SocketObject.h"

class NAMEDPIPE_LIFE{
	BYTE ciOpenMode,ciErrorStatus;
	DWORD dwErrorCode;
	HANDLE hPipe;
	SECURITY_ATTRIBUTES sa,*m_pSA;
	void InitSecurityAttributes()
	{
		PSECURITY_DESCRIPTOR    pSD;
		// create a security NULL security
		// descriptor, one that allows anyone
		// to write to the pipe... WARNING
		// entering NULL as the last attribute
		// of the CreateNamedPipe() will
		// indicate that you wish all
		// clients connecting to it to have
		// all of the same security attributes
		// as the user that started the
		// pipe server.

		pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
			SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
		{
			LocalFree((HLOCAL)pSD);
			return;
		}
		// add a NULL disc. ACL to the
		// security descriptor.
		if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
		{
			LocalFree((HLOCAL)pSD);
			return;
		}

		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle = TRUE;
	}
public:
	NAMEDPIPE_LIFE(){
		dwErrorCode=0;
		ciErrorStatus=0;
		sa.lpSecurityDescriptor=NULL;
		ciOpenMode=0;
		m_pSA=NULL;
		hPipe=NULL;
	}
	NAMEDPIPE_LIFE(HANDLE hNamedPipe,BYTE SERVER0_CLIENT1=0){
		sa.lpSecurityDescriptor=NULL;
		ciOpenMode=0;
		m_pSA=NULL;
		if(ConnectPipe(hNamedPipe))
			hPipe=hNamedPipe;
		else
			hPipe=NULL;
	}
	BOOL ConnectPipe(HANDLE hNamedPipe){
		dwErrorCode=0;
		ciErrorStatus=0;
		if(ciOpenMode==0&&ConnectNamedPipe(hNamedPipe,NULL))
			return TRUE;
		else
		{
			ciErrorStatus=4;
			dwErrorCode=GetLastError();
			return FALSE;
		}
	}
	NAMEDPIPE_LIFE(const char* pipeName,BYTE SERVER0_CLIENT1=0,SECURITY_ATTRIBUTES *pSA=NULL){
		ciErrorStatus=0;
		dwErrorCode=0;
		hPipe=NULL;
		ciOpenMode=0;
		sa.lpSecurityDescriptor=NULL;
		InitPipe(pipeName,SERVER0_CLIENT1,pSA);
	}
	void InitPipe(const char* pipeName,BYTE SERVER0_CLIENT1=0,SECURITY_ATTRIBUTES *pSA=NULL){
		DestroyPipe();	//重新初始化管道之前需要销毁之前的管道
		ciErrorStatus=0;
		dwErrorCode=0;
		hPipe=NULL;
		ciOpenMode=0;
		sa.lpSecurityDescriptor=NULL;
		m_pSA=pSA;
		if(pSA==NULL&&(ciOpenMode=SERVER0_CLIENT1)==0)
		{
			InitSecurityAttributes();
			m_pSA=&sa;
		}
		if(ciOpenMode==0)
		{
			hPipe=CreateNamedPipe(pipeName,PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE,32,0,0,2000,m_pSA);
			dwErrorCode=GetLastError();
		}
		else if(ciOpenMode==1)
		{
			if(!WaitNamedPipe(pipeName,NMPWAIT_USE_DEFAULT_WAIT))
			{
				int iError=GetLastError();
				if(iError>=1326||iError<=1331)
					ciErrorStatus=1;	//登录主机失败
				else //if(iError==53)	
					ciErrorStatus=2;	//主机无响应
			}
			else if((hPipe=CreateFile(pipeName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL))==INVALID_HANDLE_VALUE)
				ciErrorStatus=3;		//管道创建失败
			if(ciErrorStatus>0)
				dwErrorCode=GetLastError();
		}
	}
	HANDLE PipeHandle(){return hPipe;}
	~NAMEDPIPE_LIFE(){
		DestroyPipe();
	}
	void DestroyPipe(){
		if(ciOpenMode==0&&sa.lpSecurityDescriptor)
			LocalFree((HLOCAL)sa.lpSecurityDescriptor);
		if(hPipe)
		{
			if(ciOpenMode==0)
				DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
		}
	}
	CXhChar200 GetErrorStr()
	{
		CXhChar200 sError;
		if(dwErrorCode==2)
		{
			sError.Copy("系统找不到指定的文件");
			if(ciOpenMode==1)
				sError.Append(". 请核实主授权服务是否已启动，错误代码#2!");
		}
		else if(dwErrorCode==53)
		{
			sError.Copy("找不到网络路径");
			if(ciOpenMode==1)
				sError.Append(". 请核实主授权服务器名称及IP是否正确，错误代码#53!");
		}
		else if(dwErrorCode==71)
			sError.Copy("已达到XP计算机的连接数最大值，无法再同此远程计算机连接。 ");
		else if(dwErrorCode==1326)
			sError.Copy("登录失败: 未知的用户名或错误密码。确认是否可以正常访问授权服务器。");
		else if(ciErrorStatus==1)
			sError.Printf("登录服务器失败，错误代码#%d！",dwErrorCode);
		else if(ciErrorStatus==2)
			sError.Printf("服务器无响应，错误代码#%d！",dwErrorCode);
		else if(ciErrorStatus==3)
			sError.Printf("管道创建失败，错误代码#%d！",dwErrorCode);
		else if(ciErrorStatus==4)
			sError.Printf("连接管道失败，错误代码#%d！",dwErrorCode);
		return sError;
	}
};

class CCommunicationObject
{
	const static BYTE TYPE_NONE		= 0;	//无效连接
	const static BYTE TYPE_PIPE		= 1;	//管道
	const static BYTE TYPE_SOCKET	= 2;	//Socket
	BYTE m_cConnectType;
	CXhChar100 m_sIpAddr;
	USHORT m_siPortNumber;
	CXhChar500 m_sPipeName;
	BOOL m_bServer;
	CSocketObject socketObj;
	NAMEDPIPE_LIFE pipe;
public:
	CCommunicationObject();
	CCommunicationObject(const char *sIpAddr,u_short siPortNumber,bool bServer=true);
	CCommunicationObject(const char* pipeName,bool bServer=true,SECURITY_ATTRIBUTES *pSA=NULL);
	CCommunicationObject(const char *sIpAddr,u_short siPortNumber,
						 const char* pipeName,bool bServer=true,SECURITY_ATTRIBUTES *pSA=NULL);
	~CCommunicationObject();
	BOOL Write(LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten);
	BOOL Read(LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead);
	BOOL Connect();
	BOOL Disconnect();
	BOOL IsValidConnection();
	UINT GetConnId(int pipe0socket1); 
	BYTE GetCurrActConnType(){return m_cConnectType;}
	CXhChar200 GetErrorStr(char ciTypeAct0Pipe1Socket2=0);
	BOOL IsPipeConnect(){return m_cConnectType==TYPE_PIPE;}
	BOOL IsSocketConnect(){return m_cConnectType==TYPE_SOCKET;}
	BOOL InitSocketConnect(const char *sIpAddr,u_short siPortNumber,bool bServer=true);
	BOOL InitPipeConnect(const char* pipeName,bool bServer=TRUE,SECURITY_ATTRIBUTES *pSA=NULL);
};