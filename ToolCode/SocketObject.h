#pragma once;
#include <winsock2.h>
#include "XhCharString.h"

class CSocketObject
{
	UINT m_hSocket;
	SOCKET m_hAcceptSocket;
	int m_nFunType;
public:
	struct SOCKETINIT{
	public:
		SOCKETINIT(WORD wVersionRequested=0x0202){
			WSADATA wsdata;
			WSAStartup(wVersionRequested,&wsdata);
		}
		~SOCKETINIT(){
			WSACleanup();
		}
	};
	CSocketObject(const char* strBindIp=NULL,u_short bindPort=0,BOOL bServer=TRUE);
	~CSocketObject();
	UINT GetSocketId(){return m_hSocket;}
	__declspec( property(get=GetSocketId)) UINT SocketId;
	UINT GetAcceptSocketId(){return (UINT)m_hAcceptSocket;}
	__declspec( property(get=GetAcceptSocketId)) UINT AcceptSocketId;
	static CXhChar100 GetMachineIpStr(const char* hostname=NULL);
	static int GetMachineIp(const char* hostname=NULL,in_addr* pAddrs=NULL,int maxReadIpCount=1);
	static bool GetMachineName(char* hostname);
	static const int SOCKET_LOWEVER        = 2;
	static const int SOCKER_HIGVER         = 2;
	//
	
	static const int FUN_SEND_ERR       = -2;
	static const int FUN_RECV_ERR       = -3;
	static const int FUN_CONNECT_ERR    = -4;
	static const int FUN_BIND_ERR       = -5;
	static const int FUN_LISTEN_ERR     = -6;
	static const int FUN_LIB_ERR		= -8;
	static const int FUN_ACCEPT_ERR		= -9;
	static const int FUN_SCOKET_ERR		= -10;
	int InitSocket(const char* strBindIp=NULL,u_short bindPort=0,BOOL bServer=TRUE);
	int NetRecv(SOCKET sock, char *buf, int len);
	int NetSend(SOCKET sock, const char *buf, int len);
	SOCKET NetAccept(sockaddr* paddr=NULL,int addrLen=0);
	BOOL CloseAcceptSocket();
	void DestroySocketObject();
	//
	CXhChar200 GetErrorStr();
private:
	SOCKETINIT socketLife;
};