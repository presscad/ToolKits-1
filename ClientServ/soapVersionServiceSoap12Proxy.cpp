/* soapVersionServiceSoap12Proxy.cpp
   Generated by gSOAP 2.8.19 from d:\Structure\!ClientServTemp\ClientServer.h

Copyright(C) 2000-2014, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "soapVersionServiceSoap12Proxy.h"

VersionServiceSoap12Proxy::VersionServiceSoap12Proxy()
{	VersionServiceSoap12Proxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

VersionServiceSoap12Proxy::VersionServiceSoap12Proxy(const struct soap &_soap) : soap(_soap)
{ }

VersionServiceSoap12Proxy::VersionServiceSoap12Proxy(const char *url)
{	VersionServiceSoap12Proxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
	soap_endpoint = url;
}

VersionServiceSoap12Proxy::VersionServiceSoap12Proxy(soap_mode iomode)
{	VersionServiceSoap12Proxy_init(iomode, iomode);
}

VersionServiceSoap12Proxy::VersionServiceSoap12Proxy(const char *url, soap_mode iomode)
{	VersionServiceSoap12Proxy_init(iomode, iomode);
	soap_endpoint = url;
}

VersionServiceSoap12Proxy::VersionServiceSoap12Proxy(soap_mode imode, soap_mode omode)
{	VersionServiceSoap12Proxy_init(imode, omode);
}

VersionServiceSoap12Proxy::~VersionServiceSoap12Proxy()
{ }

void VersionServiceSoap12Proxy::VersionServiceSoap12Proxy_init(soap_mode imode, soap_mode omode)
{	soap_imode(this, imode);
	soap_omode(this, omode);
	soap_endpoint = NULL;
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"TMSServ2", "http://xerofox.com/TMSService/TMSServiceSoap", NULL, NULL},
	{"TMSServ", "http://xerofox.com/TMSService/", NULL, NULL},
	{"TMSServ3", "http://xerofox.com/TMSService/TMSServiceSoap12", NULL, NULL},
	{"VersionServ2", "http://xerofox.com/VersionService/VersionServiceSoap", NULL, NULL},
	{"VersionServ", "http://xerofox.com/VersionService/", NULL, NULL},
	{"VersionServ3", "http://xerofox.com/VersionService/VersionServiceSoap12", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this, namespaces);
}

void VersionServiceSoap12Proxy::destroy()
{	soap_destroy(this);
	soap_end(this);
}

void VersionServiceSoap12Proxy::reset()
{	destroy();
	soap_done(this);
	soap_initialize(this);
	VersionServiceSoap12Proxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

void VersionServiceSoap12Proxy::soap_noheader()
{	this->header = NULL;
}

const SOAP_ENV__Header *VersionServiceSoap12Proxy::soap_header()
{	return this->header;
}

const SOAP_ENV__Fault *VersionServiceSoap12Proxy::soap_fault()
{	return this->fault;
}

const char *VersionServiceSoap12Proxy::soap_fault_string()
{	return *soap_faultstring(this);
}

const char *VersionServiceSoap12Proxy::soap_fault_detail()
{	return *soap_faultdetail(this);
}

int VersionServiceSoap12Proxy::soap_close_socket()
{	return soap_closesock(this);
}

int VersionServiceSoap12Proxy::soap_force_close_socket()
{	return soap_force_closesock(this);
}

void VersionServiceSoap12Proxy::soap_print_fault(FILE *fd)
{	::soap_print_fault(this, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void VersionServiceSoap12Proxy::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this, os);
}
#endif

char *VersionServiceSoap12Proxy::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this, buf, len);
}
#endif

int VersionServiceSoap12Proxy::Login(const char *endpoint, const char *soap_action, _VersionServ__Login *VersionServ__Login, _VersionServ__LoginResponse &VersionServ__LoginResponse)
{	struct soap *soap = this;
	struct __VersionServ3__Login soap_tmp___VersionServ3__Login;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/Login";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__Login.VersionServ__Login = VersionServ__Login;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__Login(soap, &soap_tmp___VersionServ3__Login);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__Login(soap, &soap_tmp___VersionServ3__Login, "-VersionServ3:Login", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__Login(soap, &soap_tmp___VersionServ3__Login, "-VersionServ3:Login", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__LoginResponse)
		return soap_closesock(soap);
	VersionServ__LoginResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__LoginResponse.soap_get(soap, "VersionServ:LoginResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::LoginByByteArr(const char *endpoint, const char *soap_action, _VersionServ__LoginByByteArr *VersionServ__LoginByByteArr, _VersionServ__LoginByByteArrResponse &VersionServ__LoginByByteArrResponse)
{	struct soap *soap = this;
	struct __VersionServ3__LoginByByteArr soap_tmp___VersionServ3__LoginByByteArr;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/LoginByByteArr";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__LoginByByteArr.VersionServ__LoginByByteArr = VersionServ__LoginByByteArr;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__LoginByByteArr(soap, &soap_tmp___VersionServ3__LoginByByteArr);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__LoginByByteArr(soap, &soap_tmp___VersionServ3__LoginByByteArr, "-VersionServ3:LoginByByteArr", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__LoginByByteArr(soap, &soap_tmp___VersionServ3__LoginByByteArr, "-VersionServ3:LoginByByteArr", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__LoginByByteArrResponse)
		return soap_closesock(soap);
	VersionServ__LoginByByteArrResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__LoginByByteArrResponse.soap_get(soap, "VersionServ:LoginByByteArrResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::GetUpdateVerByCurVer(const char *endpoint, const char *soap_action, _VersionServ__GetUpdateVerByCurVer *VersionServ__GetUpdateVerByCurVer, _VersionServ__GetUpdateVerByCurVerResponse &VersionServ__GetUpdateVerByCurVerResponse)
{	struct soap *soap = this;
	struct __VersionServ3__GetUpdateVerByCurVer soap_tmp___VersionServ3__GetUpdateVerByCurVer;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/GetUpdateVerByCurVer";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__GetUpdateVerByCurVer.VersionServ__GetUpdateVerByCurVer = VersionServ__GetUpdateVerByCurVer;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__GetUpdateVerByCurVer(soap, &soap_tmp___VersionServ3__GetUpdateVerByCurVer);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__GetUpdateVerByCurVer(soap, &soap_tmp___VersionServ3__GetUpdateVerByCurVer, "-VersionServ3:GetUpdateVerByCurVer", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__GetUpdateVerByCurVer(soap, &soap_tmp___VersionServ3__GetUpdateVerByCurVer, "-VersionServ3:GetUpdateVerByCurVer", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__GetUpdateVerByCurVerResponse)
		return soap_closesock(soap);
	VersionServ__GetUpdateVerByCurVerResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__GetUpdateVerByCurVerResponse.soap_get(soap, "VersionServ:GetUpdateVerByCurVerResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::SetRevisionReadFlag(const char *endpoint, const char *soap_action, _VersionServ__SetRevisionReadFlag *VersionServ__SetRevisionReadFlag, _VersionServ__SetRevisionReadFlagResponse &VersionServ__SetRevisionReadFlagResponse)
{	struct soap *soap = this;
	struct __VersionServ3__SetRevisionReadFlag soap_tmp___VersionServ3__SetRevisionReadFlag;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/SetRevisionReadFlag";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__SetRevisionReadFlag.VersionServ__SetRevisionReadFlag = VersionServ__SetRevisionReadFlag;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__SetRevisionReadFlag(soap, &soap_tmp___VersionServ3__SetRevisionReadFlag);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__SetRevisionReadFlag(soap, &soap_tmp___VersionServ3__SetRevisionReadFlag, "-VersionServ3:SetRevisionReadFlag", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__SetRevisionReadFlag(soap, &soap_tmp___VersionServ3__SetRevisionReadFlag, "-VersionServ3:SetRevisionReadFlag", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__SetRevisionReadFlagResponse)
		return soap_closesock(soap);
	VersionServ__SetRevisionReadFlagResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__SetRevisionReadFlagResponse.soap_get(soap, "VersionServ:SetRevisionReadFlagResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::CompareVersion(const char *endpoint, const char *soap_action, _VersionServ__CompareVersion *VersionServ__CompareVersion, _VersionServ__CompareVersionResponse &VersionServ__CompareVersionResponse)
{	struct soap *soap = this;
	struct __VersionServ3__CompareVersion soap_tmp___VersionServ3__CompareVersion;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/CompareVersion";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__CompareVersion.VersionServ__CompareVersion = VersionServ__CompareVersion;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__CompareVersion(soap, &soap_tmp___VersionServ3__CompareVersion);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__CompareVersion(soap, &soap_tmp___VersionServ3__CompareVersion, "-VersionServ3:CompareVersion", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__CompareVersion(soap, &soap_tmp___VersionServ3__CompareVersion, "-VersionServ3:CompareVersion", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__CompareVersionResponse)
		return soap_closesock(soap);
	VersionServ__CompareVersionResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__CompareVersionResponse.soap_get(soap, "VersionServ:CompareVersionResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::GetVersionReleaseNotes(const char *endpoint, const char *soap_action, _VersionServ__GetVersionReleaseNotes *VersionServ__GetVersionReleaseNotes, _VersionServ__GetVersionReleaseNotesResponse &VersionServ__GetVersionReleaseNotesResponse)
{	struct soap *soap = this;
	struct __VersionServ3__GetVersionReleaseNotes soap_tmp___VersionServ3__GetVersionReleaseNotes;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/GetVersionReleaseNotes";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__GetVersionReleaseNotes.VersionServ__GetVersionReleaseNotes = VersionServ__GetVersionReleaseNotes;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__GetVersionReleaseNotes(soap, &soap_tmp___VersionServ3__GetVersionReleaseNotes);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__GetVersionReleaseNotes(soap, &soap_tmp___VersionServ3__GetVersionReleaseNotes, "-VersionServ3:GetVersionReleaseNotes", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__GetVersionReleaseNotes(soap, &soap_tmp___VersionServ3__GetVersionReleaseNotes, "-VersionServ3:GetVersionReleaseNotes", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__GetVersionReleaseNotesResponse)
		return soap_closesock(soap);
	VersionServ__GetVersionReleaseNotesResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__GetVersionReleaseNotesResponse.soap_get(soap, "VersionServ:GetVersionReleaseNotesResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::GetDogAccessHistory(const char *endpoint, const char *soap_action, _VersionServ__GetDogAccessHistory *VersionServ__GetDogAccessHistory, _VersionServ__GetDogAccessHistoryResponse &VersionServ__GetDogAccessHistoryResponse)
{	struct soap *soap = this;
	struct __VersionServ3__GetDogAccessHistory soap_tmp___VersionServ3__GetDogAccessHistory;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/GetDogAccessHistory";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__GetDogAccessHistory.VersionServ__GetDogAccessHistory = VersionServ__GetDogAccessHistory;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__GetDogAccessHistory(soap, &soap_tmp___VersionServ3__GetDogAccessHistory);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__GetDogAccessHistory(soap, &soap_tmp___VersionServ3__GetDogAccessHistory, "-VersionServ3:GetDogAccessHistory", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__GetDogAccessHistory(soap, &soap_tmp___VersionServ3__GetDogAccessHistory, "-VersionServ3:GetDogAccessHistory", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__GetDogAccessHistoryResponse)
		return soap_closesock(soap);
	VersionServ__GetDogAccessHistoryResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__GetDogAccessHistoryResponse.soap_get(soap, "VersionServ:GetDogAccessHistoryResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::QueryDownLoadItemArrByProductId(const char *endpoint, const char *soap_action, _VersionServ__QueryDownLoadItemArrByProductId *VersionServ__QueryDownLoadItemArrByProductId, _VersionServ__QueryDownLoadItemArrByProductIdResponse &VersionServ__QueryDownLoadItemArrByProductIdResponse)
{	struct soap *soap = this;
	struct __VersionServ3__QueryDownLoadItemArrByProductId soap_tmp___VersionServ3__QueryDownLoadItemArrByProductId;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/QueryDownLoadItemArrByProductId";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__QueryDownLoadItemArrByProductId.VersionServ__QueryDownLoadItemArrByProductId = VersionServ__QueryDownLoadItemArrByProductId;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__QueryDownLoadItemArrByProductId(soap, &soap_tmp___VersionServ3__QueryDownLoadItemArrByProductId);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__QueryDownLoadItemArrByProductId(soap, &soap_tmp___VersionServ3__QueryDownLoadItemArrByProductId, "-VersionServ3:QueryDownLoadItemArrByProductId", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__QueryDownLoadItemArrByProductId(soap, &soap_tmp___VersionServ3__QueryDownLoadItemArrByProductId, "-VersionServ3:QueryDownLoadItemArrByProductId", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__QueryDownLoadItemArrByProductIdResponse)
		return soap_closesock(soap);
	VersionServ__QueryDownLoadItemArrByProductIdResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__QueryDownLoadItemArrByProductIdResponse.soap_get(soap, "VersionServ:QueryDownLoadItemArrByProductIdResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::GetFileHeader(const char *endpoint, const char *soap_action, _VersionServ__GetFileHeader *VersionServ__GetFileHeader, _VersionServ__GetFileHeaderResponse &VersionServ__GetFileHeaderResponse)
{	struct soap *soap = this;
	struct __VersionServ3__GetFileHeader soap_tmp___VersionServ3__GetFileHeader;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/GetFileHeader";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__GetFileHeader.VersionServ__GetFileHeader = VersionServ__GetFileHeader;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__GetFileHeader(soap, &soap_tmp___VersionServ3__GetFileHeader);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__GetFileHeader(soap, &soap_tmp___VersionServ3__GetFileHeader, "-VersionServ3:GetFileHeader", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__GetFileHeader(soap, &soap_tmp___VersionServ3__GetFileHeader, "-VersionServ3:GetFileHeader", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__GetFileHeaderResponse)
		return soap_closesock(soap);
	VersionServ__GetFileHeaderResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__GetFileHeaderResponse.soap_get(soap, "VersionServ:GetFileHeaderResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::QueryObjects(const char *endpoint, const char *soap_action, _VersionServ__QueryObjects *VersionServ__QueryObjects, _VersionServ__QueryObjectsResponse &VersionServ__QueryObjectsResponse)
{	struct soap *soap = this;
	struct __VersionServ3__QueryObjects soap_tmp___VersionServ3__QueryObjects;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/QueryObjects";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__QueryObjects.VersionServ__QueryObjects = VersionServ__QueryObjects;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__QueryObjects(soap, &soap_tmp___VersionServ3__QueryObjects);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__QueryObjects(soap, &soap_tmp___VersionServ3__QueryObjects, "-VersionServ3:QueryObjects", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__QueryObjects(soap, &soap_tmp___VersionServ3__QueryObjects, "-VersionServ3:QueryObjects", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__QueryObjectsResponse)
		return soap_closesock(soap);
	VersionServ__QueryObjectsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__QueryObjectsResponse.soap_get(soap, "VersionServ:QueryObjectsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::DownloadObjects(const char *endpoint, const char *soap_action, _VersionServ__DownloadObjects *VersionServ__DownloadObjects, _VersionServ__DownloadObjectsResponse &VersionServ__DownloadObjectsResponse)
{	struct soap *soap = this;
	struct __VersionServ3__DownloadObjects soap_tmp___VersionServ3__DownloadObjects;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/DownloadObjects";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__DownloadObjects.VersionServ__DownloadObjects = VersionServ__DownloadObjects;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__DownloadObjects(soap, &soap_tmp___VersionServ3__DownloadObjects);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__DownloadObjects(soap, &soap_tmp___VersionServ3__DownloadObjects, "-VersionServ3:DownloadObjects", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__DownloadObjects(soap, &soap_tmp___VersionServ3__DownloadObjects, "-VersionServ3:DownloadObjects", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__DownloadObjectsResponse)
		return soap_closesock(soap);
	VersionServ__DownloadObjectsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__DownloadObjectsResponse.soap_get(soap, "VersionServ:DownloadObjectsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int VersionServiceSoap12Proxy::SaveOrUpdateObjects(const char *endpoint, const char *soap_action, _VersionServ__SaveOrUpdateObjects *VersionServ__SaveOrUpdateObjects, _VersionServ__SaveOrUpdateObjectsResponse &VersionServ__SaveOrUpdateObjectsResponse)
{	struct soap *soap = this;
	struct __VersionServ3__SaveOrUpdateObjects soap_tmp___VersionServ3__SaveOrUpdateObjects;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://localhost/VersionService/VersionService.asmx";
	if (soap_action == NULL)
		soap_action = "http://xerofox.com/VersionService/SaveOrUpdateObjects";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___VersionServ3__SaveOrUpdateObjects.VersionServ__SaveOrUpdateObjects = VersionServ__SaveOrUpdateObjects;
	soap_serializeheader(soap);
	soap_serialize___VersionServ3__SaveOrUpdateObjects(soap, &soap_tmp___VersionServ3__SaveOrUpdateObjects);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___VersionServ3__SaveOrUpdateObjects(soap, &soap_tmp___VersionServ3__SaveOrUpdateObjects, "-VersionServ3:SaveOrUpdateObjects", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___VersionServ3__SaveOrUpdateObjects(soap, &soap_tmp___VersionServ3__SaveOrUpdateObjects, "-VersionServ3:SaveOrUpdateObjects", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&VersionServ__SaveOrUpdateObjectsResponse)
		return soap_closesock(soap);
	VersionServ__SaveOrUpdateObjectsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	VersionServ__SaveOrUpdateObjectsResponse.soap_get(soap, "VersionServ:SaveOrUpdateObjectsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}
/* End of client proxy code */