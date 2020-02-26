////////////////////////////////// HttpClient.cpp
#include "stdafx.h"
#include "HttpClient.h"

#define  BUFFER_SIZE       1024

#define  NORMAL_CONNECT             INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT             NORMAL_CONNECT | INTERNET_FLAG_SECURE | INTERNET_DEFAULT_HTTPS_PORT | SECURITY_INTERNET_MASK
#define  NORMAL_REQUEST             INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST             NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

CHttpClient::CHttpClient(LPCTSTR strAgent)
{
	m_pSession = new CInternetSession(strAgent);
	m_pConnection = NULL;
	m_pFile = NULL;
}


CHttpClient::~CHttpClient(void)
{
	Clear();
	if (NULL != m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}
}

void CHttpClient::Clear()
{
	if (NULL != m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}

	if (NULL != m_pConnection)
	{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;
	}
}

int CHttpClient::ExecuteRequest(int strMethod, LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse)
{
	int result =FAILURE ;
	//WCHAR* wPostData = strPostData.GetBuffer();
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;


	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);
	if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		return FAILURE;
	}

	try
	{
		m_pConnection = m_pSession->GetHttpConnection(strServer,
			dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
			nPort);
		m_pFile = m_pConnection->OpenRequest(strMethod, strObject,
			NULL, 1, NULL, NULL,
			(dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));

		/*����������ز���*/
		m_pFile->AddRequestHeaders("Accept: */*,text/xml");//accept����ͷ�򣬱�ʾ�ͻ��˽�����Щ���͵���Ϣ
		m_pFile->AddRequestHeaders("Accept-Charset:UTF8");
		m_pFile->AddRequestHeaders("Accept-Language: zh-cn;q=0.8,en;q=0.6,ja;q=0.4");
		m_pFile->AddRequestHeaders("Content-Type:text/xml");//contentΪʵ�屨ͷ�򣬸�ʽ������
		
		bool bTryAgain=false;
		/*����body������תΪUTF-8���룬�����˱���һ��,cwordΪҪ��������*/
		char*cword = NULL; //ANSIָ��
		if (strPostData != NULL){
			DWORD  num = WideCharToMultiByte(CP_UTF8, 0, _bstr_t(strPostData), -1, NULL, 0, NULL, NULL);//Ӌ���@��UNICODEʵ���ɼ���UTF-8�ֽM��
			cword = (char*)calloc(num, sizeof(char));   //����ռ�
			if (cword == NULL)                          //�Ƿ�����
				free(cword);
			else
			{
				memset(cword, 0, num*sizeof(char));     //��ʼ��
				WideCharToMultiByte(CP_UTF8, 0, _bstr_t(strPostData), -1, cword, num, NULL, NULL);
			}
		}
		try
		{
			m_pFile->SendRequest(NULL, 0, cword, cword == NULL ? 0 : _tcslen(cword));
		}
		catch (CInternetException* e)
		{
			DWORD dwErrorCode = e->m_dwError;
			e->Delete();
			if (dwErrorCode == ERROR_INTERNET_INVALID_CA||dwErrorCode==12044)
				bTryAgain=true;
			else
			{	
				Clear();
				dwErrorCode = GetLastError();
				//printf("dwError = %d", dwErrorCode, 0);
				strResponse.Format("CInternetException Return Code: %d %d \n",e->m_dwError, dwErrorCode);

				if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
					return OUTTIME;
				else
					return FAILURE;
			}
		}
		if(bTryAgain)
		{
			DWORD dwFlags;
			m_pFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
			m_pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
			m_pFile->SendRequest(NULL, 0, cword, cword == NULL ? 0 : _tcslen(cword));
		}
		if(cword)
			free(cword);
		DWORD dwRet;
		m_pFile->QueryInfoStatusCode(dwRet);//��ѯִ��״̬
		printf("HTTP_STATUS_code:%d\n", dwRet);
		if (dwRet == HTTP_STATUS_OK){//http����ִ��ʧ��
			result = SUCCESS;
		}

		/*����http��Ӧ*/
		char szChars[BUFFER_SIZE + 1] = { 0 };
		string strRawResponse = "";
		UINT nReaded = 0;
		while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
		{
			szChars[nReaded] = '\0';
			strRawResponse += szChars;
			memset(szChars, 0, BUFFER_SIZE + 1);
		}

		/*utf8תunicode*/
		int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, strRawResponse.c_str(), -1, NULL, 0);
		WCHAR *pUnicode = new WCHAR[unicodeLen + 1];
		memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, strRawResponse.c_str(), -1, pUnicode, unicodeLen);
		strResponse = pUnicode;//������Ӧ���
		//TRACE(strResponse + L"");
		delete[]pUnicode;
		pUnicode = NULL;
		
		Clear();
	}
	catch (CInternetException* e)
	{
		DWORD dwError = 0;
		DWORD dwErrorCode = e->m_dwError;
		e->Delete();
		//
		Clear();
		dwError = GetLastError();
		//printf("dwError = %d", dwError, 0);
		//strResponse = L"CInternetException\n";
		strResponse.Format("CInternetException Return Code: %d %d\n", dwErrorCode,dwError);

		if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
		{
			return OUTTIME;
		}
		else
		{
			return FAILURE;
		}
	}
	return result;
}

int CHttpClient::HttpGet(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse)
{
	return ExecuteRequest(CHttpConnection::HTTP_VERB_GET, strUrl, NULL, strResponse);
}

int CHttpClient::HttpPost(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse)
{
	return ExecuteRequest(CHttpConnection::HTTP_VERB_POST, strUrl, strPostData, strResponse);
}
int CHttpClient::HttpPut(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse)
{
	return ExecuteRequest(CHttpConnection::HTTP_VERB_PUT, strUrl, strPostData, strResponse);
}

CString CHttpClient::Post(CString xml, CString url, bool isUseCert, int timeout)
{
	CHttpClient client;
	CString strResponse;
	client.HttpPost(url,xml,strResponse);
	return strResponse;
}

CString CHttpClient::Get(CString xml, CString url, bool isUseCert, int timeout)
{
	CHttpClient client;
	CString strResponse;
	client.HttpGet(url,xml,strResponse);
	return strResponse;
}