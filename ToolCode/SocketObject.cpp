#include "StdAfx.h"
#include "SocketObject.h"

CSocketObject::CSocketObject(const char* strBindIp/*=NULL*/,u_short bindPort/*=0*/,BOOL bServer/*=TRUE*/)
{
	m_hSocket = (UINT)INVALID_SOCKET;
	m_hAcceptSocket=(UINT)INVALID_SOCKET;
	m_nFunType=0;
	InitSocket(strBindIp,bindPort,bServer);
}
CSocketObject::~CSocketObject()
{
	DestroySocketObject();
}
void CSocketObject::DestroySocketObject()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=(UINT)INVALID_SOCKET;
	}
	if(m_hAcceptSocket!=INVALID_SOCKET)
	{
		closesocket(m_hAcceptSocket);
		m_hAcceptSocket=INVALID_SOCKET;
	}
}
/* 使用TCP/IP协议
 * strBindIp 默认监听网络所有的通信
 */
int CSocketObject::InitSocket(const char* strBindIp/*=NULL*/,u_short bindPort/*=0*/,BOOL bServer/*=TRUE*/)
{
	//DestroySocketObject();	此处不能调用DestroySocketObject(),需进一步确认原因? wht 17-02-10
	if(bindPort==0)
	{
		m_hSocket=(UINT)INVALID_SOCKET;
		return m_hSocket;
	}
	SOCKET socketId = INVALID_SOCKET;
	//创建一个能进行网络通信的套接字
	socketId = socket(AF_INET,SOCK_STREAM,0);
	if(socketId==SOCKET_ERROR)
	{
		m_nFunType=FUN_SCOKET_ERR;
		return m_nFunType;
	}
	SOCKADDR_IN sockStruct;
	sockStruct.sin_family = AF_INET; //使用TCP/IP协议
	if(strBindIp==NULL||strBindIp[0]=='\0')
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	else
		sockStruct.sin_addr.S_un.S_addr = inet_addr(strBindIp);
	sockStruct.sin_port = htons(bindPort);
	if(!bServer) //客户端
	{
		if(connect(socketId,(LPSOCKADDR)&sockStruct,sizeof(sockStruct)) == SOCKET_ERROR)
		{
			m_nFunType=FUN_CONNECT_ERR;
			return m_nFunType;
		}
		else
			m_hSocket = (UINT)socketId;
	}
	else //if( bServer)  //服务端
	{
		if(bind(socketId,(sockaddr*)&sockStruct,sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			m_nFunType=FUN_BIND_ERR;
			return m_nFunType;
		}
		else if( listen(socketId,SOMAXCONN) == SOCKET_ERROR )
		{
			m_nFunType=FUN_LISTEN_ERR;
			return m_nFunType;
		}
		else
			return(m_hSocket=(UINT)socketId);
	}
	return (UINT)socketId;
}

bool CSocketObject::GetMachineName(char* hostname)
{
	SOCKETINIT life;
	//获取本地主机名称
	char host_name[255]="";
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		return false;
	StrCopy(hostname,host_name,255);
	return strlen(host_name)>0;
}

CXhChar100 CSocketObject::GetMachineIpStr(const char* hostname/*=NULL*/)
{
	in_addr addr;
	GetMachineIp(hostname,&addr);
	CXhChar100 ipstr;
	ipstr.Printf("%d.%d.%d.%d",addr.S_un.S_un_b.s_b1,addr.S_un.S_un_b.s_b2,addr.S_un.S_un_b.s_b3,addr.S_un.S_un_b.s_b4);
	return ipstr;
}

int CSocketObject::GetMachineIp(const char* hostname/*=NULL*/,in_addr* pAddrs/*=NULL*/,int maxReadIpCount/*=1*/)
{
	SOCKETINIT life;
	//获取本地主机名称
	char host_name[255]="";
	if(hostname!=NULL)
		StrCopy(host_name,hostname,255);
	else if(gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		return 0;
	struct hostent *pHostent = gethostbyname(host_name);
	if (pHostent == 0)
		return 0;
	//循环得出本地机器所有IP地址
	if(pHostent==0)
		return 0;
	int i=0;
	for(i=0;i<pHostent->h_length&&pHostent->h_addr_list&&pHostent->h_addr_list[i]!=0;i++)
	{
		if(pAddrs!=NULL&&i<maxReadIpCount)
			memcpy(&pAddrs[i], pHostent->h_addr_list[i], sizeof(struct in_addr));
		else if(pAddrs!=NULL&&i==maxReadIpCount)
			return maxReadIpCount;
		//computer_ip[0]=addr.S_un.S_un_b.s_b1;
		//computer_ip[1]=addr.S_un.S_un_b.s_b2;
		//computer_ip[2]=addr.S_un.S_un_b.s_b3;
		//computer_ip[3]=addr.S_un.S_un_b.s_b4;
	}
	return i;
}
SOCKET CSocketObject::NetAccept(sockaddr* paddr/*=NULL*/,int addrLen/*=0*/)
{
	CloseAcceptSocket();
	m_hAcceptSocket = INVALID_SOCKET;
	sockaddr addr;
	if(paddr==NULL)
	{
		paddr=&addr;
		addrLen=sizeof(addr);
	}
	m_hAcceptSocket=accept(m_hSocket,paddr,&addrLen);
	if (m_hAcceptSocket==INVALID_SOCKET)
	{
		m_nFunType=FUN_ACCEPT_ERR;
		return m_nFunType;
	}
	else
		return m_hAcceptSocket;
}
BOOL CSocketObject::CloseAcceptSocket()
{
	if(m_hAcceptSocket!=INVALID_SOCKET)
	{
		closesocket(m_hAcceptSocket);
		m_hAcceptSocket=INVALID_SOCKET;
	}
	return TRUE;
}
int CSocketObject::NetRecv(SOCKET sock, char *buf, int len)
{
	int res=recv(sock, buf, len, 0);
	if(res==SOCKET_ERROR||res<0)
	{
		m_nFunType=FUN_RECV_ERR;
		return m_nFunType;
	}
	return res;
}
int CSocketObject::NetSend(SOCKET sock, const char *buf, int len)
{
	int res=send( sock, buf, len,0);
	if(res==SOCKET_ERROR||res<0)
	{
		m_nFunType=FUN_SEND_ERR;
		return m_nFunType;
	}
	return  res;
}
CXhChar200  CSocketObject::GetErrorStr()
{
	CXhChar200 sErr;
	DWORD dwErrCode=WSAGetLastError();
	if(m_nFunType==CSocketObject::FUN_LIB_ERR)
		sErr.Printf("winsocket库版本低");
	else if(m_nFunType==CSocketObject::FUN_SCOKET_ERR)
	{
		if(dwErrCode==WSAEAFNOSUPPORT)
			sErr.Printf("socket()错误码：%d,不支持指定的地址族！",dwErrCode);
		else if(dwErrCode==WSAEINPROGRESS)
			sErr.Printf("socket()错误码：%d,一个阻塞的套接口调用正在运行中！",dwErrCode);
		else if(dwErrCode==WSAEMFILE)
			sErr.Printf("socket()错误码：%d,无可用文件描述字！",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("socket()错误码：%d,无可用缓冲区，无法创建套接口！",dwErrCode);
		else if(dwErrCode==WSAEPROTONOSUPPORT)
			sErr.Printf("socket()错误码：%d,不支持指定的协议！",dwErrCode);
		else if(dwErrCode==WSAEPROTOTYPE)
			sErr.Printf("socket()错误码：%d,指定的协议不适用于本套接口！",dwErrCode);
		else if(dwErrCode==WSAESOCKTNOSUPPORT)
			sErr.Printf("socket()错误码：%d,本地址族中不支持该类型套接口！",dwErrCode);
		else 
			sErr.Printf("socket()错误码：%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_SEND_ERR)
	{
		if(dwErrCode==WSAEINTR)
			sErr.Printf("send()错误码：%d，通过一个WSACancelBlockingCall()来取消一个（阻塞的）调用！",dwErrCode);
		else if(dwErrCode==WSAEFAULT)
			sErr.Printf("send()错误码：%d，buf参数不在用户地址空间中的有效位置！",dwErrCode);
		else if(dwErrCode==WSAENETRESET)
			sErr.Printf("send()错误码：%d，由于WINDOWS套接口实现放弃了连接，故该连接必需被复位！",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("send()错误码：%d，WINDOWS套接口实现报告一个缓冲区死锁！",dwErrCode);
		else if(dwErrCode==WSAENOTCONN)
			sErr.Printf("send()错误码：%d，套接口未被连接！",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("send()错误码：%d，套接口未用bind()捆绑！",dwErrCode);
		else if(dwErrCode==WSAECONNABORTED)
			sErr.Printf("send()错误码：%d，由于超时或其他原因引起虚电路的中断！",dwErrCode);
		else 
			sErr.Printf("send()错误码：%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_RECV_ERR)
	{
		if(dwErrCode==WSAESHUTDOWN)
			sErr.Printf("recv()错误码：%d，套接口已被关闭！",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("recv()错误码：%d，套接口未用bind()进行捆绑！",dwErrCode);
		//else if(dwErrCode==WSAENOTCONN)
			//sErr.Printf("套接口未连接！",dwErrCode);
		else if(dwErrCode==WSANOTINITIALISED)
			sErr.Printf("recv()错误码：%d，没有调用WSAStartup()或者失败！",dwErrCode);
		//else if(dwErrCode==WSAENOBUFS)
			//sErr.Printf("WINDOWS套接口实现报告一个缓冲区死锁！";
		else if(dwErrCode==WSAENOTCONN)
			sErr.Printf("recv()错误码：%d，套接口未被连接！",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("recv()错误码：%d，套接口未用bind()捆绑！",dwErrCode);
		else if(dwErrCode==WSAECONNABORTED)
			sErr.Printf("recv()错误码：%d，由于超时或其他原因引起虚电路的中断！",dwErrCode);
		else 
			sErr.Printf("recv()错误码：%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_CONNECT_ERR)
	{
		if (dwErrCode==WSAEADDRINUSE)
			sErr.Printf("connect()错误码：%d，地址已经使用",dwErrCode);
		else if (dwErrCode==WSAEISCONN)
			sErr.Printf("connect()错误码：%d，套接口早已连接",dwErrCode);
		else if (dwErrCode==WSAEFAULT)
			sErr.Printf("connect()错误码：%d，namelen参数不正确",dwErrCode);
		else if (dwErrCode==WSAEINVAL)
			sErr.Printf("connect()错误码：%d，套接口没有准备好与一地址捆绑",dwErrCode);
		else if (dwErrCode==WSAENETUNREACH)
			sErr.Printf("connect()错误码：%d，当前无法从本主机访问网络",dwErrCode);
		else if (dwErrCode==WSAECONNREFUSED)
			sErr.Printf("connect()错误码：%d，连接尝试被强制拒绝",dwErrCode);
		else if (dwErrCode==WSAEINPROGRESS)
			sErr.Printf("connect()错误码：%d，一个阻塞的套接口调用正在运行中",dwErrCode);
		else if (dwErrCode==WSAEADDRNOTAVAIL)
			sErr.Printf("connect()错误码：%d，在本地机器上找不到所指的地址",dwErrCode);
		else 
			sErr.Printf("connect()错误码：%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_LISTEN_ERR)
	{
		if(dwErrCode==WSAEADDRINUSE)
			sErr.Printf("listen()错误码：%d,试图用listen()去监听一个正在使用中的地址！",dwErrCode);
		else if(dwErrCode==WSAEINPROGRESS)
			sErr.Printf("listen()错误码：%d,一个阻塞的套接口调用正在运行中！",dwErrCode);
		else if(dwErrCode==WSAEINVAL)
			sErr.Printf("listen()错误码：%d,该套接口未用bind()进行捆绑，或已被连接！",dwErrCode);
		else if(dwErrCode==WSAEISCONN)
			sErr.Printf("listen()错误码：%d,套接口已被连接！",dwErrCode);
		else if(dwErrCode==WSAEMFILE)
			sErr.Printf("listen()错误码：%d,无可用文件描述字！",dwErrCode);
		else if(dwErrCode==WSAENOTSOCK)
			sErr.Printf("listen()错误码：%d,描述字不是一个套接口！",dwErrCode);
		else if(dwErrCode==WSAENOBUFS)
			sErr.Printf("listen()错误码：%d,无可用缓冲区空间！",dwErrCode);
		else if(dwErrCode==WSAEOPNOTSUPP)
			sErr.Printf("listen()错误码：%d,该套接口不正常listen()调用！",dwErrCode);
		else
			sErr.Printf("listen()错误码：%d",dwErrCode);
	}
	else if(m_nFunType==CSocketObject::FUN_ACCEPT_ERR)
	{
		if (dwErrCode==WSAEFAULT)
			sErr.Printf("accept()错误码：%d,addrlen参数太小（小于socket结构的大小）",dwErrCode);
		else if (dwErrCode==WSAEINTR)
			sErr.Printf("accept()错误码：%d,通过一个WSACancelBlockingCall()来取消一个（阻塞的）调用",dwErrCode);
		else if (dwErrCode==WSAEINPROGRESS)
			sErr.Printf("accept()错误码：%d,一个阻塞的套接口调用正在运行中",dwErrCode);
		else if (dwErrCode==WSAEINVAL)
			sErr.Printf("accept()错误码：%d,在accept()前未激活listen()",dwErrCode);
		else if (dwErrCode==WSAEMFILE)
			sErr.Printf("accept()错误码：%d,调用accept()时队列为空，无可用的描述字",dwErrCode);
		else if (dwErrCode==WSAENOBUFS)
			sErr.Printf("accept()错误码：%d,无可用缓冲区空间",dwErrCode);
		else if (dwErrCode==WSAENOTSOCK)
			sErr.Printf("accept()错误码：%d,描述字不是一个套接口",dwErrCode);
		else if (dwErrCode==WSAEOPNOTSUPP)
			sErr.Printf("accept()错误码：%d,该套接口类型不支持面向连接服务",dwErrCode);
		else if (dwErrCode==WSAEWOULDBLOCK)
			sErr.Printf("accept()错误码：%d,该套接口为非阻塞方式且无连接可供接受",dwErrCode);
		else if (dwErrCode==WSAECONNRESET)
			sErr.Printf("accept()错误码：%d,接受连接后在accept返回之前，被远程客户端断开连接",dwErrCode);
		else
			sErr.Printf("accept()错误码：%d",dwErrCode);
	}
	return sErr;
}
#ifdef __ALFA_TEST_VERSION_
void TestServerSocket()
{
	CSocketObject s;
	s.InitSocket("192.168.2.1",8888,TRUE);
	//if(soket<0)
		//出错 
	while(TRUE)
	{
		SOCKET acceptSoket=s.NetAccept();
		char str[100]="";
		if(s.NetRecv(acceptSoket,str,sizeof(str))<0)
		{//出错 
			return;
		}
		if(s.NetRecv(acceptSoket,str,sizeof(str))<0)
		{
			return;
		}
		CXhChar100 str1="asdffds";
		if(s.NetSend(acceptSoket,str1,sizeof(str1))<0)
		{
			return;
		}
		closesocket(acceptSoket);
	}
}
void TestClientSocket()
{
	CSocketObject socket;
	SOCKET socket1=socket.InitSocket("192.168.2.1",8888,FALSE);
	if(socket1<0)
	{
		//socket.GetErrorStr(CSocketObject::FUN_SCOKET_ERR);
		//出错
		return;
	}
	CXhChar100 a ="1234";
	if(socket.NetSend(socket1,a,sizeof(a))<0)
	{
		closesocket(socket1);
		return;
	}
	a ="789";
	if(socket.NetSend(socket1,a,sizeof(a))<0)
	{
		closesocket(socket1);
		return;
	}
	a="";
	if(socket.NetRecv(socket1,a,sizeof(a))<0)
	{
		closesocket(socket1);
		return;
	}
	closesocket(socket1);
}
#endif