#pragma once

#include "XhCharString.h"
#include "Buffer.h"
#include "HashTable.h"
#include "ServerObject.h"
#include "list.h"
#include "MemberProperty.h"
#include "Markup.h"


class CEndUser : public CServerObject
{
public:
	BYTE m_biApplyAutState;
	unsigned int m_uUserId,m_uDogNo,m_uProductId,m_uVersion;
	BYTE computer_mac[6];
	BYTE computer_ip[4];
	BYTE computer_ip2[4];
	CXhChar200 m_sComputerName;
	CXhChar200 m_sProductName;
	BYTE fingerHardId[16];
	BYTE fingerDigest[32];
	BYTE fingerDetail[192];
	DWORD m_dwApplyTimeStamp;
	CXhChar200 m_sEndUserName;	//�ն��û���
	CXhChar200 m_sPhoneNumber;	//�绰����
	CXhChar200 m_sClientName;	//�ͻ�����
	//��Ȩ����0.����1.��ʽ2.����3.���޴����
	static const int TYPE_AUT_TRAIL			= 0;
	static const int TYPE_AUT_OFFICIAL		= 1;
	static const int TYPE_AUT_RENTAL		= 2;
	static const int TYPE_AUT_APPLY_RENTAL	= 3;
	int m_iOnLineRentalState;
public:
	CEndUser();
	~CEndUser(){;}
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};

class COrder : public CServerObject
{
public:
	int endUserId;		//�ͻ��ն������ݿ��Id
	int applyRandCode;	//���������
	int applyTimeStamp;
	int dogSerial;		//�������ţ�����������
	int productId;		//����������Ĳ�Ʒ����Id
	int productVersion;	//�����Ʒ�汾��
	int rentalDays;		//�������
	int money;			//��𣬷�
	CXhChar50 fingerHardId;
	CXhChar100 orderSerial;
	CXhChar200 wxOrderSerial;	//΢�Ŷ�����
	int tempAuthFileId;
	int tempAuthTimeStamp;
public:
	COrder();
	~COrder(){;}
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};

class CProductRental : public CServerObject
{
public:
	int productId;
	CXhChar100 productName;
	CXhChar100 rentalDescription;
	int rentalDays;
	int money;
public:
	CProductRental(int product_id=0,int rental_days=0,int rental_money=0);
	~CProductRental(){;}
	void CopyProperty(CProductRental &rental);
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0);
};
