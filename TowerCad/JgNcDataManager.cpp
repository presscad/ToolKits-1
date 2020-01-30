// JgNcDataManager.cpp: implementation of the CJgNcDataManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "LDS.h"
#include "JgNcDataManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJgNcDataManager::CJgNcDataManager()
{
	m_bPrintByASCII = TRUE;
	STAMP_MAX_LEN		=150;	//���ӡ(��ģ)����,mm
	STAMP_START_LENPOS	=200;	//��ӡ(��ģ)��ʼ����λ��,mm
	STAMP_CLEARANCE		=20;	//��ӡ(��ģ)����˨�׼����С��϶,mm
	STAMP_IN_WING ='0';
	strcpy(sAbsolutePosMark,"");
	strcpy(sRelativePosMark,"");
	strcpy(sQ235,"A3F");
	strcpy(sQ345,"16Mn");
	strcpy(sQ355,"Q355");
	strcpy(sQ390,"Q390");
	strcpy(sQ420,"Q420");
	strcpy(sQ460,"Q460");
	strcpy(sQ235BriefMark,"A3F");
	strcpy(sQ345BriefMark,"16Mn");
	strcpy(sQ355BriefMark,"Q355");
	strcpy(sQ390BriefMark,"Q390");
	strcpy(sQ420BriefMark,"Q420");
	strcpy(sQ460BriefMark,"Q460");
	strcpy(extension,"dat");
	m_bSpecialHoleOutputHoleD=FALSE;
	m_bSpecialHoleAutoDrillD = FALSE;
	m_bUsePartLabelPrefix = TRUE;
}
BOOL CJgNcDataManager::InitJgNcDriver(const char *nc_driver)
{
	if(nc_driver==NULL)
		return FALSE;
	else if(strlen(nc_driver)==0)
		return FALSE;
	FILE *fp =fopen(nc_driver,"rt");
	if(fp==NULL)
		return FALSE;
	char line_txt[200],bak_line_txt[200];
	directory.Empty();
	hole_head.Empty();
	print_list.Empty();
	while(!feof(fp))
	{
		if(fgets(line_txt,200,fp)==NULL)
			break;
		line_txt[strlen(line_txt)-1]='\0';
		strcpy(bak_line_txt,line_txt);
		char szTokens[] = " =\n" ;
		
		char* szToken = strtok(line_txt, szTokens) ; 
		if (szToken&&_stricmp(szToken, "PRINT_MODE") == 0)
		{
			szToken = strtok(NULL, szTokens);
			if(_stricmp(szToken,"BINARY")==0)
				m_bPrintByASCII = FALSE;//����������ļ�	
			else// if(_stricmp(szToken,"TEXT")==0)
				m_bPrintByASCII = TRUE;	//����ı��ļ�
		}
		else if (szToken&&_stricmp(szToken, "FILE_EXTENSION") == 0)
		{
			szToken = strtok(NULL, szTokens);
			memset(extension,0,sizeof(extension));
			if(szToken)
			{
				int n;
				n = min(3,strlen(szToken));
				strncpy(extension,szToken,n);
			}
		}
		else if(szToken&&_stricmp(szToken,"Q235")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ235,szToken,9);
			else
				sQ235[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q345")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ345,szToken,9);
			else
				sQ345[0]='\0';
		}
		else if (szToken&&_stricmp(szToken, "Q355") == 0)
		{
			szToken = strtok(NULL, szTokens);
			if (szToken != NULL)
				strncpy(sQ355, szToken, 9);
			else
				sQ355[0] = '\0';
		}
		else if(szToken&&_stricmp(szToken,"Q390")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ390,szToken,9);
			else
				sQ390[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q420")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ420,szToken,9);
			else
				sQ420[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q460")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ460,szToken,9);
			else
				sQ460[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q235_BRIEF")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ235BriefMark,szToken,9);
			else
				sQ235BriefMark[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q345_BRIEF")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ345BriefMark,szToken,9);
			else
				sQ345BriefMark[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q390_BRIEF")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ390BriefMark,szToken,9);
			else
				sQ390BriefMark[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q420_BRIEF")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ420BriefMark,szToken,9);
			else
				sQ420BriefMark[0]='\0';
		}
		else if(szToken&&_stricmp(szToken,"Q460_BRIEF")==0)
		{
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				strncpy(sQ460BriefMark,szToken,9);
			else
				sQ460BriefMark[0]='\0';
		}
		else if(szToken&&(szToken[0]=='D'||szToken[0]=='d'))
		{
			HOLE_HEAD *pHoleHead=hole_head.append();
			pHoleHead->d = atoi(CharNext(szToken));
			szToken = strtok(NULL, szTokens);
			strncpy(pHoleHead->alias,szToken,9);
		}
		else if(szToken&&_stricmp(szToken, "SPECIAL_HOLE") == 0)
		{	//����״���ʽ wht 19-09-13
			szToken = strtok(NULL, szTokens);
			if (szToken != NULL && stricmp(szToken, "HOLE_D") == 0)
			{
				m_bSpecialHoleOutputHoleD = TRUE;
				m_bSpecialHoleAutoDrillD = FALSE;
			}
			else if (szToken != NULL && stricmp(szToken, "AUTO_D") == 0)
			{
				m_bSpecialHoleAutoDrillD = TRUE;
				m_bSpecialHoleOutputHoleD = FALSE;
			}
		}
		else if (szToken&&_stricmp(szToken, "PART_NO_PREFIX") == 0)
		{	//���ü���ǰ׺ wht 19-10-18
			szToken = strtok(NULL, szTokens);
			if (szToken != NULL)
			{
				if (stricmp(szToken, "FALSE") == 0)
					m_bUsePartLabelPrefix = FALSE;
				else
					m_bUsePartLabelPrefix = TRUE;
			}
			else
				m_bUsePartLabelPrefix = TRUE;
		}
		else if (szToken&&_stricmp(szToken, "STAMP_MAX_LEN") == 0)
		{	//���ӡ(��ģ)����(Ĭ��150),mm
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				STAMP_MAX_LEN=atoi(szToken);
		}
		else if (szToken&&_stricmp(szToken, "STAMP_START_LENPOS") == 0)
		{	////��ӡ(��ģ)��ʼ����λ��(Ĭ��200),mm
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				STAMP_START_LENPOS=atoi(szToken);
		}
		else if (szToken&&_stricmp(szToken, "STAMP_CLEARANCE") == 0)
		{	//��ӡ(��ģ)����˨�׼����С��϶(Ĭ��20),mm
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL)
				STAMP_CLEARANCE=atoi(szToken);
		}
		else if (szToken&&_stricmp(szToken, "STAMP_IN_WING") == 0)
		{	//��ӡ(��ģ)����֫
			szToken = strtok(NULL, szTokens);
			if(szToken!=NULL&&strlen(szToken)>0)
				STAMP_IN_WING=szToken[0];
		}
		else if(szToken&&_stricmp(szToken,"HOLE_LEN_POS")==0)
		{
			szToken = strtok(NULL, szTokens);
			strncpy(sAbsolutePosMark,szToken,9);
		}
		else if(szToken&&_stricmp(szToken,"HOLE_LEN_REFPOS")==0)
		{
			szToken = strtok(NULL, szTokens);
			strncpy(sRelativePosMark,szToken,9);
		}
		else if(szToken&&_stricmp(szToken,"</DIRECTORY>")==0)
		{
			CNCDirectoryFile *pDirFile=directory.append();
			while(!feof(fp))
			{
				if(fgets(line_txt,200,fp)==NULL)
					break;
				line_txt[strlen(line_txt)-1]='\0';
				strcpy(bak_line_txt,line_txt);
				szToken = strtok(line_txt, szTokens) ; 
				if(_stricmp(line_txt,"<DIRECTORY>")==0)
					break;
				else if(_stricmp(line_txt,"PRINT_MODE")==0)
				{
					szToken = strtok(NULL, szTokens);
					if(stricmp(szToken,"BINARY")==0)
						pDirFile->m_bDirectoryPrintByASCII = FALSE;
					else
						pDirFile->m_bDirectoryPrintByASCII = TRUE;
				}
				else if(_stricmp(line_txt,"DIR_FILE_NAME")==0)
				{
					szToken = strtok(NULL, szTokens);
					strcpy(pDirFile->DIR_FILE_NAME,szToken);
				}
				else
				{
					CVariant *pVar = pDirFile->contents.append();
					pVar->SetGridString(bak_line_txt);
				}
			}
		}
		else
		{
			CVariant *pVar = print_list.append();
			pVar->SetGridString(bak_line_txt);
		}
	}
	fclose(fp);
	return TRUE;
}

CJgNcDataManager::~CJgNcDataManager()
{

}
