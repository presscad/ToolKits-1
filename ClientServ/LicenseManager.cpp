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
	memset(fingerHardId,0,16);
	memset(fingerDigest,0,32);
	memset(fingerDetail,0,192);
	m_iOnLineRentalState=0;
	m_biApplyAutState=false;
	m_uProductId=0;
}
void CEndUser::ToBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	buffer.WriteInteger(_id);
	buffer.WriteInteger(4);
	buffer.Write(computer_ip,4);
	buffer.WriteInteger(4);
	buffer.Write(computer_ip2,4);
	buffer.WriteInteger(6);
	buffer.Write(computer_mac,6);
	buffer.WriteStringToUTF8(m_sComputerName);
	buffer.WriteInteger(16);
	buffer.Write(fingerHardId,16);
	buffer.WriteInteger(32);
	buffer.Write(fingerDigest,32);
	buffer.WriteInteger(192);
	buffer.Write(fingerDetail,192);
	buffer.WriteInteger(m_iOnLineRentalState);
	buffer.WriteInteger(m_uProductId);
	buffer.WriteInteger(m_dwApplyTimeStamp);
	buffer.WriteStringToUTF8(m_sEndUserName);
	buffer.WriteStringToUTF8(m_sPhoneNumber);
	buffer.WriteStringToUTF8(m_sClientName);
	
}
void CEndUser::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
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
		buffer.Read(fingerHardId,len);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(fingerDigest,len);
	len = buffer.ReadInteger();
	if(len>0)
		buffer.Read(fingerDetail,len);
	m_iOnLineRentalState=buffer.ReadInteger();
	m_uProductId=buffer.ReadInteger();
	m_dwApplyTimeStamp=buffer.ReadInteger();
	buffer.ReadStringFromUTF8(m_sEndUserName);
	buffer.ReadStringFromUTF8(m_sPhoneNumber);
	buffer.ReadStringFromUTF8(m_sClientName);
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