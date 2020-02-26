#pragma once

#ifndef __LICENSE_MANAGER_H_
#define __LICENSE_MANAGER_H_

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
	static const int INFO_COMPUTER	= 0x01;	//�����Ӳ����Ϣ
	static const int INFO_ENDUSER	= 0x02;	//������û���Ϣ���û������绰��΢�źš��ͻ����ƣ�
	int m_iInfoFlag;
	BYTE m_biApplyAutState;
	unsigned int m_uUserId,m_uDogNo,m_uProductId,m_uVersion;
	BYTE computer_mac[6];
	BYTE computer_ip[4];
	BYTE computer_ip2[4];
	CXhChar200 m_sComputerName;
	CXhChar200 m_sProductName;
	BYTE hardIdCode[16];
	BYTE fingerprint[32];
	BYTE fingerDetail[192];
	BYTE functionUseFlag[64];
	DWORD m_dwApplyTimeStamp;
	CXhChar200 m_sEndUserName;	//�ն��û���
	CXhChar200 m_sPhoneNumber;	//�绰����
	CXhChar200 m_sWeiXin;		//΢�ź�
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

struct VersionRevision
{
	unsigned int Id;
	unsigned int productId;
	char revisionType;
	unsigned char important;
	bool readed;
	char title[500];
	char contents[1000];
	VersionRevision() {
		Id = 0;
		productId = 0;
		revisionType = 0;
		important = 0;
		readed = false;
		strcpy(title, "");
		strcpy(contents, "");
	}
};

class CObjectPtrArrLife
{
	void *m_pObjArr;
public:
	CObjectPtrArrLife(void *pObjArr)
	{
		m_pObjArr = pObjArr;
	}
	~CObjectPtrArrLife()
	{
		if (m_pObjArr)
			delete m_pObjArr;
		m_pObjArr = NULL;
	}
};

struct ProductVersion
{
	unsigned int productId;
	unsigned int version;
	char releaseDate[100];

	void SetReleaseData(time_t t)
	{
		CTime time(t);
		strcpy(releaseDate,time.Format("%Y-%m-%d %H%M%S"));
	}
	ProductVersion() {
		productId = 0;
		version = 0;
		strcpy(releaseDate, "");
	}
};

#endif