#pragma once
#include <stdlib.h>
#include "XhCharString.h"

struct PARTSIZE{
	char ciSizeType;	//'L':½Ç¸Ö;'-':¸Ö°å;'T':¸Ö¹Ü;'M':ÂÝË¨£¬ÆäËü
	union{
		char szMaterial[5];
		float sfBoltGrade;		//ÂÝË¨¼¶±ð
	};
	union{
		float sfWidth;	//Ö«¿í
		float sfDia;	//¸Ö¹ÜÍâ¾¶
		float sfMd;		//ÂÝË¨Ö±¾¶
	};
	union{
		float sfThick;	//Ö«¿í£¬Ö«ºñ
		float sfBoltL;	//ÂÝË¨ÓÐÐ§³¤¶È
	};
	float sfWideOfShortLeg;	//²»µÈÖ«½Ç¸ÖµÄ½ÏÕ­Ö«¿í
	PARTSIZE(const char* sizestr=NULL){
		ciSizeType=0;
		StrCopy(szMaterial,"Q235",5);
		sfWidth=sfThick=sfWideOfShortLeg=0;
		if(sizestr!=NULL)
			ParseSizeStr(sizestr);
	}
	bool ParseSizeStr(const char* sizestr)
	{
		if(sizestr==NULL)
			return false;
		CXhChar16 itemstr;
		CXhChar50 szSizeStr=sizestr;
		char *key=NULL,*pchType=NULL,*pszSizeStr=szSizeStr;
		if((pchType=strchr(szSizeStr,'L'))!=NULL||(pchType=strchr(szSizeStr,'-'))!=NULL)
		{	//½Ç¸Ö»ò¸Ö°å
			ciSizeType=*pchType;
			if(pchType>pszSizeStr)
			{
				itemstr.NCopy(pszSizeStr,pchType-pszSizeStr);
				StrCopy(szMaterial,itemstr,5);
			}
			pszSizeStr=pchType+1;
			if((key=strtok(pszSizeStr,"Xx*"))!=NULL)
				sfWidth=(float)atof(key);
			else
				return false;
			if((key=strtok(NULL,"Xx*"))!=NULL)
			{
				sfThick=(float)atof(key);
				if((key=strtok(NULL,"Xx*"))!=NULL)
					sfWideOfShortLeg=(float)atof(key);
			}
		}
		else if((pchType=strstr(szSizeStr,"¦Õ"))!=NULL||(pchType=strstr(szSizeStr,"¦µ"))!=NULL)
		{	//¸Ö¹Ü
			ciSizeType='T';
			if(pchType>pszSizeStr)
			{
				itemstr.NCopy(pszSizeStr,pchType-pszSizeStr);
				StrCopy(szMaterial,itemstr,5);
			}
			pszSizeStr=pchType+1;
			if((key=strtok(pszSizeStr,"Xx*"))!=NULL)
				this->sfDia=(float)atof(key);
			else
				return false;
			if((key=strtok(NULL,"Xx*"))!=NULL)
				this->sfThick=(float)atof(key);
		}
		else if((pchType=strchr(szSizeStr,'M'))!=NULL)
		{	//ÂÝË¨
			ciSizeType=*pchType;
			if(pchType>pszSizeStr)
			{
				itemstr.NCopy(pszSizeStr,pchType-pszSizeStr);
				sfBoltGrade=(float)atof(itemstr);
			}
			pszSizeStr=pchType+1;
			if((key=strtok(pszSizeStr,"Xx*"))!=NULL)
				this->sfMd=(float)atof(key);
			else
				return false;
			if((key=strtok(NULL,"Xx*"))!=NULL)
				this->sfBoltL=(float)atof(key);
		}
		else
			return false;
		return true;
	}
};
