#include "stdafx.h"
#include "Communication.h"
#include "XhCharString.h"

CCommunicationObject::CCommunicationObject()
{
	m_cConnectType=TYPE_NONE;
	m_bServer=FALSE;
	m_siPortNumber=0;
}
CCommunicationObject::CCommunicationObject(const char *sIpAddr,u_short siPortNumber,bool bServer/*=true*/)
{
	InitSocketConnect(sIpAddr,siPortNumber,bServer);
}
CCommunicationObject::CCommunicationObject(const char* pipeName,bool bServer/*=true*/,SECURITY_ATTRIBUTES *pSA/*=NULL*/)
{
	InitPipeConnect(pipeName,bServer,pSA);
}
CCommunicationObject::CCommunicationObject(const char *sIpAddr,u_short siPortNumber,
										   const char* pipeName,bool bServer/*=true*/,SECURITY_ATTRIBUTES *pSA/*=NULL*/)
{	
	if(InitPipeConnect(pipeName,bServer,pSA))
		m_cConnectType=TYPE_PIPE;
	else if(InitSocketConnect(sIpAddr,siPortNumber,bServer))
		m_cConnectType=TYPE_SOCKET;
	else
		m_cConnectType=TYPE_NONE;
}

BOOL CCommunicationObject::InitPipeConnect(const char* pipeName,bool bServer/*=true*/,SECURITY_ATTRIBUTES *pSA/*=NULL*/)
{
	m_cConnectType=TYPE_PIPE;
	m_bServer=bServer;
	m_sPipeName.Copy(pipeName);
	pipe.InitPipe(pipeName,bServer?0:1,pSA);
	if(pipe.PipeHandle()!=NULL)
		return TRUE;
	else
		return FALSE;
}
BOOL CCommunicationObject::InitSocketConnect(const char *sIpAddr,u_short siPortNumber,bool bServer/*=true*/)
{
	m_cConnectType=TYPE_SOCKET;
	m_sIpAddr.Copy(sIpAddr);
	m_siPortNumber=siPortNumber;
	m_bServer=bServer;
	if(strlen(sIpAddr)>0&&siPortNumber>0)
	{
		socketObj.InitSocket(sIpAddr,siPortNumber,m_bServer);
		return TRUE;
	}
	else
		return FALSE;
}
CCommunicationObject::~CCommunicationObject()
{
	
}
BOOL CCommunicationObject::Write(LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten)
{
	if(m_cConnectType==TYPE_PIPE)
		return WriteFile(pipe.PipeHandle(),lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,NULL);
	else if(m_cConnectType==TYPE_SOCKET)
	{
		if(m_bServer)
			return socketObj.NetSend(socketObj.AcceptSocketId,(char*)lpBuffer,nNumberOfBytesToWrite);
		else
			return socketObj.NetSend(socketObj.SocketId,(char*)lpBuffer,nNumberOfBytesToWrite);
	}
	else
		return FALSE;
}

BOOL CCommunicationObject::Read(LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead)
{
	if(m_cConnectType==TYPE_PIPE)
		return ReadFile(pipe.PipeHandle(),lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead,NULL);
	else if(m_cConnectType==TYPE_SOCKET)
	{
		if(m_bServer)
			return socketObj.NetRecv(socketObj.AcceptSocketId,(char*)lpBuffer,nNumberOfBytesToRead);
		else
			return socketObj.NetRecv(socketObj.SocketId,(char*)lpBuffer,nNumberOfBytesToRead);
	}
	else
		return FALSE;
}

BOOL CCommunicationObject::Connect()
{
	if(m_cConnectType==TYPE_PIPE)
		return pipe.ConnectPipe(pipe.PipeHandle());
	else if(m_cConnectType==TYPE_SOCKET)
		return (socketObj.NetAccept()!=INVALID_SOCKET);
	else
		return FALSE;
}

BOOL CCommunicationObject::Disconnect()
{
	if(m_cConnectType==TYPE_PIPE)
		return DisconnectNamedPipe(pipe.PipeHandle());
	else if(m_cConnectType==TYPE_SOCKET)
		return socketObj.CloseAcceptSocket();
	else
		return FALSE;
}

UINT CCommunicationObject::GetConnId(int pipe0socket1)
{
	if(pipe0socket1==0)
		return (UINT)pipe.PipeHandle();
	else
		return socketObj.SocketId;
}
BOOL CCommunicationObject::IsValidConnection()
{
	if(m_cConnectType==TYPE_PIPE)
		return pipe.PipeHandle()!=NULL;
	else if(m_cConnectType==TYPE_SOCKET)
		return socketObj.SocketId!=NULL;
	else
		return FALSE;
}

CXhChar200 CCommunicationObject::GetErrorStr(char ciTypeAct0Pipe1Socket2/*=0*/)
{
	if(ciTypeAct0Pipe1Socket2==1)
		return pipe.GetErrorStr();
	else if(ciTypeAct0Pipe1Socket2==2)
		return socketObj.GetErrorStr();
	else if(m_cConnectType==TYPE_PIPE)
		return pipe.GetErrorStr();
	else if(m_cConnectType==TYPE_SOCKET)
		return socketObj.GetErrorStr();
	else
		return CXhChar200("无效连接");
}