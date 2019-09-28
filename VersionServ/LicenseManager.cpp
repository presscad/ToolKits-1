#include "afxwin.h"
#include "LicenseManager.h"
#include "ArrayList.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////////
// CEndUser
CEndUser::CEndUser()
{
	m_sServerClassName="EndUser";
	m_uUserId=m_uDogNo=m_uProductId=m_uVersion=0;
	memset(computer_mac,0,6);
	memset(computer_ip,0,4);
	memset(computer_ip2,0,4);
	memset(hardIdCode,0,16);
	memset(fingerprint,0,32);
	memset(fingerDetail,0,192);
	memset(fingerprint, 0, 64);
	memset(functionUseFlag, 0, 64);
	m_iOnLineRentalState=0;
	m_biApplyAutState=false;
	m_uProductId=0;
	m_iInfoFlag = INFO_COMPUTER;
}
static const int ENDUSER_VERSION = 1;
static CXhChar50 VersionToString(int nVersion)
{
	int version_sect[4] = { 0 };
	version_sect[0] = nVersion / 1000000;
	nVersion %= 1000000;
	version_sect[1] = nVersion / 10000;
	nVersion %= 10000;
	version_sect[2] = nVersion / 100;
	nVersion %= 100;
	version_sect[3] = nVersion;
	char sSectVersion[8];
	CXhChar50 version;
	for (int i = 0; i < 4; i++)
	{
		sprintf(sSectVersion, "%d", version_sect[i]);
		if (i > 0)
			version.Append('.');
		version.Append(sSectVersion);
	}
	return version;
}
//将字符串型的版本号转换为一个长整数，如"1.01.3.21"-->1 01 03 21
static long FromStringVersion(const char* version)
{
	char local_version[20] = "";
	strncpy(local_version, version, 20);
	char *key = strtok(local_version, " .,-");
	UINT version_item[4] = { 0 };
	int n, i = 0;
	while (key != NULL && i < 4)
	{
		n = (int)strlen(key);
		if (n == 1)
			version_item[i] = key[0] - '0';
		else if (n > 1)
			version_item[i] = (key[0] - '0') * 10 + key[1] - '0';
		key = strtok(NULL, ".,-");
		i++;
	}
	long nVersion = version_item[0] * 1000000 + version_item[1] * 10000 + version_item[2] * 100 + version_item[3];
	//long nVersion=version_item[0]*lic.ConstE6()+version_item[1]*lic.ConstE4()+version_item[2]*lic.ConstE2()+version_item[3];
	return nVersion;
}
void CEndUser::ToBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	buffer.WriteInteger(0xFFFFFFFF);	//版本号标识位：为兼容之前版本，前四个字节设置为版本号标识位 wht 19-04-01
	buffer.WriteInteger(ENDUSER_VERSION);
	buffer.WriteInteger(m_iInfoFlag);
	buffer.WriteInteger(_id);
	buffer.WriteInteger(4);
	buffer.Write(computer_ip,4);
	buffer.WriteInteger(4);
	buffer.Write(computer_ip2,4);
	buffer.WriteInteger(6);
	buffer.Write(computer_mac,6);
	buffer.WriteStringToUTF8(m_sComputerName);
	buffer.WriteInteger(16);
	buffer.Write(hardIdCode,16);
	buffer.WriteInteger(32);
	buffer.Write(fingerprint,32);
	buffer.WriteInteger(192);
	buffer.Write(fingerDetail,192);
	buffer.WriteInteger(m_iOnLineRentalState);
	buffer.WriteInteger(m_uProductId);
	buffer.WriteInteger(m_dwApplyTimeStamp);
	buffer.WriteStringToUTF8(m_sEndUserName);
	buffer.WriteStringToUTF8(m_sPhoneNumber);
	buffer.WriteStringToUTF8(m_sClientName);
	buffer.WriteStringToUTF8(m_sWeiXin);
	CXhChar50 sProductVersion=VersionToString(m_uVersion);
	buffer.WriteStringToUTF8(sProductVersion);
	buffer.WriteInteger(m_uDogNo);
	buffer.WriteInteger(64);
	buffer.Write(functionUseFlag, 64);
	
}

void CEndUser::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	long nVersionFlag=buffer.ReadInteger();	//读取版本标识字节
	long nVersion = 0;
	if (nVersionFlag == (int)0xFFFFFFFF)
	{
		nVersion = buffer.ReadInteger();
		buffer.ReadInteger();	//读取infoUpdateFlag
	}
	else
		buffer.SeekToBegin();
	_id = buffer.ReadInteger();
	int len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(computer_ip,len);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(computer_ip2,len);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(computer_mac,len);
	buffer.ReadStringFromUTF8(m_sComputerName);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(hardIdCode,len);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(fingerprint,len);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(fingerDetail,len);
	m_iOnLineRentalState=buffer.ReadInteger();
	m_uProductId=buffer.ReadInteger();
	m_dwApplyTimeStamp=buffer.ReadInteger();
	buffer.ReadStringFromUTF8(m_sEndUserName);
	buffer.ReadStringFromUTF8(m_sPhoneNumber);
	buffer.ReadStringFromUTF8(m_sClientName);
	if (nVersion >= 1)
	{
		buffer.ReadStringFromUTF8(m_sWeiXin);
		CXhChar50 sProductVersion;
		buffer.ReadStringFromUTF8(sProductVersion);
		FromStringVersion(sProductVersion);
		m_uDogNo = buffer.ReadInteger();
		len = buffer.ReadInteger();
		if (len > 0)
			buffer.Read(functionUseFlag, len);
	}
}

//////////////////////////////////////////////////////////////////////////
// COrder
COrder::COrder()
{
	m_sServerClassName="Order";
	orderSerial=endUserId=applyRandCode=applyTimeStamp=dogSerial=
	productId=productVersion=rentalDays=money=tempAuthFileId=tempAuthTimeStamp=0;
}
void COrder::ToBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	buffer.WriteInteger(_id);
	buffer.WriteStringToUTF8(orderSerial);
	buffer.WriteInteger(endUserId);
	buffer.WriteStringToUTF8(fingerHardId);
	buffer.WriteInteger(applyRandCode);
	buffer.WriteInteger(applyTimeStamp);
	buffer.WriteInteger(dogSerial);
	buffer.WriteInteger(productId);
	buffer.WriteInteger(productVersion);
	buffer.WriteInteger(rentalDays);
	buffer.WriteInteger(money);
	buffer.WriteStringToUTF8(wxOrderSerial);
	buffer.WriteInteger(tempAuthFileId);
	buffer.WriteInteger(tempAuthTimeStamp);
}
void COrder::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	_id = buffer.ReadInteger();
	buffer.ReadStringFromUTF8(orderSerial);
	endUserId = buffer.ReadInteger();
	buffer.ReadStringFromUTF8(fingerHardId);
	applyRandCode = buffer.ReadInteger();
	applyTimeStamp = buffer.ReadInteger();
	dogSerial = buffer.ReadInteger();
	productId = buffer.ReadInteger();
	productVersion = buffer.ReadInteger();
	rentalDays = buffer.ReadInteger();
	money = buffer.ReadInteger();
	buffer.ReadStringFromUTF8(wxOrderSerial);
	tempAuthFileId = buffer.ReadInteger();
	tempAuthTimeStamp = buffer.ReadInteger();
}

//////////////////////////////////////////////////////////////////////////
// CProductRental
CProductRental::CProductRental(int product_id/*=0*/,int rental_days/*=0*/,int rental_money/*=0*/)
{
	productId=product_id;
	rentalDays=rental_days;
	money=rental_money;
}
void CProductRental::CopyProperty(CProductRental& rental)
{
	_id=rental.Id;
	productId=rental.productId;
	productName.Copy(rental.productName);
	rentalDescription.Copy(rental.rentalDescription);
	rentalDays=rental.rentalDays;
	money=rental.money;
}
void CProductRental::ToBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	buffer.WriteInteger(_id);
	buffer.WriteInteger(productId);
	buffer.WriteStringToUTF8(productName);
	buffer.WriteInteger(rentalDays);
	buffer.WriteStringToUTF8(rentalDescription);
	buffer.WriteInteger(money);
}
void CProductRental::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	_id = buffer.ReadInteger();
	productId = buffer.ReadInteger();
	buffer.ReadStringFromUTF8(productName);
	rentalDays = buffer.ReadInteger();
	buffer.ReadStringFromUTF8(rentalDescription);
	money = buffer.ReadInteger(); 
}