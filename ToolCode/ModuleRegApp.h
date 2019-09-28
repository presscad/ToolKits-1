#pragma once
#include <afxwin.h>         // MFC core and standard components

class MODULEREG_APP{
protected:
	CXhChar16 m_szCompanyName,m_szAppName;
public:
	const char* get_szCompanyName(){return m_szCompanyName;}
	__declspec(property(get=get_szCompanyName)) const char* szCompanyName;
	const char* get_szAppName(){return m_szAppName;}
	__declspec(property(get=get_szAppName)) const char* szAppName;
public:
	class CKeyLifeObj{
		HKEY m_hKey;
	public:
		CKeyLifeObj(HKEY hKey=NULL){
			m_hKey=hKey;
		}
		~CKeyLifeObj(){
			if(m_hKey!=NULL)
				RegCloseKey(m_hKey);
		}
	};
public:
	MODULEREG_APP(const char* _szCompanyName=NULL,const char* _szAppName=NULL){
		m_szCompanyName =(_szCompanyName!=NULL)?_szCompanyName:"Xerofox";
		m_szAppName		=_szAppName;
		if(_szCompanyName!=NULL&&_szAppName!=NULL)
			InitRegistryKey(_szCompanyName,_szAppName);
	}
	void InitRegistryKey(const char* _lpszRegistryKey=NULL,const char* _szAppName=NULL)
	{
		m_szCompanyName =_lpszRegistryKey;
		m_szAppName		=_szAppName;
	}
	HKEY GetAppRegistryKey()
	{
		CXhChar200 szAppKey("Software\\%s\\%s",this->szCompanyName,this->szAppName);
		HKEY hAppKey=NULL;
		if(RegOpenKeyEx(HKEY_CURRENT_USER,szAppKey,0,KEY_READ,&hAppKey) != ERROR_SUCCESS)
			return NULL;
		return hAppKey;
	}
	HKEY GetSectionKey(LPCTSTR lpszSection)
	{
		if(lpszSection == NULL)
			return NULL;
		DWORD dw=0;
		HKEY hAppKey=GetAppRegistryKey();
		HKEY hSectionKey = NULL;
		::RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hSectionKey, &dw);
		RegCloseKey(hAppKey);
		return hSectionKey;
	}
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,int nDefault)
	{
		ASSERT(lpszSection != NULL);
		ASSERT(lpszEntry != NULL);
		if (szAppName != NULL) // use registry
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return nDefault;
			DWORD dwValue;
			DWORD dwType;
			DWORD dwCount = sizeof(DWORD);
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount);
			RegCloseKey(hSecKey);
			if (lResult == ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_DWORD);
				ASSERT(dwCount == sizeof(dwValue));
				return (UINT)dwValue;
			}
			return nDefault;
		}
		else
			return nDefault;
	}

	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,LPCTSTR lpszDefault = NULL)
	{
		ASSERT(lpszSection != NULL);
		ASSERT(lpszEntry != NULL);
		if (szAppName != NULL)
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return lpszDefault;
			CString strValue;
			DWORD dwType=REG_NONE;
			DWORD dwCount=0;
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				NULL, &dwCount);
			if (lResult == ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_SZ);
				lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
					(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
				strValue.ReleaseBuffer();
			}
			RegCloseKey(hSecKey);
			if (lResult == ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_SZ);
				return strValue;
			}
			return lpszDefault;
		}
		else
			return lpszDefault;
	}

	BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, UINT* pBytes)
	{
		ASSERT(lpszSection != NULL);
		ASSERT(lpszEntry != NULL);
		ASSERT(ppData != NULL);
		ASSERT(pBytes != NULL);
		*ppData = NULL;
		*pBytes = 0;
		if (szAppName != NULL)
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
			{
				return FALSE;
			}

			// ensure destruction
			CKeyLifeObj rkSecKey(hSecKey);

			DWORD dwType=0;
			DWORD dwCount=0; 
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType, NULL, &dwCount);
			*pBytes = dwCount;
			if (lResult == ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_BINARY);
				*ppData = new BYTE[*pBytes];
				lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
					*ppData, &dwCount);
			}
			if (lResult == ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_BINARY);
				return TRUE;
			}
			else
			{
				delete [] *ppData;
				*ppData = NULL;
			}
			return FALSE;
		}
		else
			return FALSE;
	}


	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		int nValue)
	{
		ASSERT(lpszSection != NULL);
		ASSERT(lpszEntry != NULL);
		if (szAppName != NULL)
		{
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_DWORD,
				(LPBYTE)&nValue, sizeof(nValue));
			RegCloseKey(hSecKey);
			return lResult == ERROR_SUCCESS;
		}
		else
			return FALSE;
	}

	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		LPCTSTR lpszValue)
	{
		ASSERT(lpszSection != NULL);
		if (szAppName != NULL)
		{
			LONG lResult;
			if (lpszEntry == NULL) //delete whole section
			{
				HKEY hAppKey = GetAppRegistryKey();
				if (hAppKey == NULL)
					return FALSE;
				lResult = ::RegDeleteKey(hAppKey, lpszSection);
				RegCloseKey(hAppKey);
			}
			else if (lpszValue == NULL)
			{
				HKEY hSecKey = GetSectionKey(lpszSection);
				if (hSecKey == NULL)
					return FALSE;
				// necessary to cast away const below
				lResult = ::RegDeleteValue(hSecKey, (LPTSTR)lpszEntry);
				RegCloseKey(hSecKey);
			}
			else
			{
				HKEY hSecKey = GetSectionKey(lpszSection);
				if (hSecKey == NULL)
					return FALSE;
				lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ,
					(LPBYTE)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
				RegCloseKey(hSecKey);
			}
			return lResult == ERROR_SUCCESS;
		}
		else
			return FALSE;
	}

	BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
		LPBYTE pData, UINT nBytes)
	{
		ASSERT(lpszSection != NULL);
		if (szAppName != NULL)
		{
			LONG lResult;
			HKEY hSecKey = GetSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_BINARY,
				pData, nBytes);
			RegCloseKey(hSecKey);
			return lResult == ERROR_SUCCESS;
		}
		else
			return FALSE;
	}
};