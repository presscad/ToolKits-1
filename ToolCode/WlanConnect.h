#pragma once
#include <objbase.h>
#include <iphlpapi.h>
#include <wlanapi.h> 

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

class CWlanConnect
{
	HANDLE m_hClient;
	PWLAN_INTERFACE_INFO m_pInterface;
	PWLAN_INTERFACE_INFO_LIST m_pInterfaceList;
	PWLAN_AVAILABLE_NETWORK m_pNetwork;
	PWLAN_AVAILABLE_NETWORK_LIST m_pNetworkList;
	static const DWORD ERROR_FUNC_WLANOPEN						= 1;	//WlanOpenHandle()
	static const DWORD ERROR_FUNC_WLANENUMINTERFACES			= 2;	//WlanEnumInterfaces()
	static const DWORD ERROR_FUNC_NULLINTERFACE					= 3;	//NullInterface
	static const DWORD ERROR_FUNC_WLANGETAVAILABLENETWORKLIST	= 4;	//WlanGetAvailableNetworkList()
	static const DWORD ERROR_FUNC_NULLNETWORK					= 5;	//NullNetwork
	static const DWORD ERROR_FUNC_WLANSETPROFILE				= 6;	//WlanSetProfile()
	static const DWORD ERROR_FUNC_WLANDISCONNECT				= 7;	//WlanDisconnect()
	static const DWORD ERROR_FUNC_WLANCONNECT					= 8;	//WlanConnect()
	DWORD m_dwRetCode;
	DWORD m_dwWlanApiRetCode;
	char IP_ADDRESS[100];		//="10.10.100.1";//Ä¬ÈÏÍø¹Ø
	char SIF_DESCRIPTION[100];	//="802.11n USB Wireless LAN Card";//ÃèÊö
	char SSID_WIFI[100];		//="Avision-715b";//wifi SSID
public:
	CWlanConnect(const char* SSIDFromWIFI,const char* wsDescription,const char* sIpAddress);
	~CWlanConnect();
	int ConnectNetwork();
	CString GetLastError();
private:

	BOOL IsConnected();
	int InternalConnectNetWork();
	int SetProfile();
};

