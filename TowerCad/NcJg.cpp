// NcJg.cpp: implementation of the CNcJg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "JgNcDataManager.h"
#include "NcJg.h"
#include "CirStack.h"
#include "DistFillet.h"
#include "PartLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNcJg::CNcJg()
{
	current_len_pos=0;
}

CNcJg::~CNcJg()
{

}
#ifdef  __LDS_CONTEXT_
BOOL CNcJg::InitJg(CLDSPart *pAnglePart)
{
	CBuffer buffer;
	pAnglePart->ToPPIBuffer(buffer);
	CProcessAngle angle;
	angle.FromPPIBuffer(buffer);
	InitProcessAngle(&angle);
}
#endif

BOOL CNcJg::InitProcessAngle(CProcessAngle *pAngle)
{
	LsCirX.RemoveAll();
	LsCirY.RemoveAll();
	memset(sPrefix,'\0',20);
	current_len_pos=0;
	if(pAngle==NULL)
		return FALSE;
	
	wing_wide = pAngle->m_fWidth;
	wing_thick= pAngle->m_fThick;
	length = pAngle->m_wLength;
	strcpy(sPartNo,pAngle->GetPartNo());
	cMaterial = pAngle->cMaterial;

	f3dCircle cir;
	BOLT_INFO *pBoltInfo=NULL;
	for(pBoltInfo=pAngle->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pAngle->m_xBoltInfoList.GetNext())
	{
		if(pBoltInfo->posY>0)
			continue;
		cir.radius = pBoltInfo->bolt_d/2.0;
		cir.centre.Set(pBoltInfo->posX,fabs(pBoltInfo->posY),0);
		int i=0;
		for(i=0;i<LsCirX.GetSize();i++)
		{
			if(LsCirX[i].centre==cir.centre)
				break;
		}
		if(i!=LsCirX.GetSize())
			continue;	//两螺栓孔重合
		LsCirX.Add(cir);
	}
	for(pBoltInfo=pAngle->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pAngle->m_xBoltInfoList.GetNext())
	{
		if(pBoltInfo->posY<0)
			continue;
		cir.radius = pBoltInfo->bolt_d/2.0;
		cir.centre.Set(pBoltInfo->posX,fabs(pBoltInfo->posY),0);
		int i=0;
		for(i=0;i<LsCirY.GetSize();i++)
		{
			if(LsCirY[i].centre==cir.centre)
				break;
		}
		if(i!=LsCirY.GetSize())
			continue;	//两螺栓孔重合
		LsCirY.Add(cir);
	}
	return TRUE;
}

int CNcJg::GetXLsCount()
{
	return LsCirX.GetSize();
}

int CNcJg::GetYLsCount()
{
	return LsCirY.GetSize();
}

BOOL CNcJg::GetXLsCircle(f3dCircle &Cir,int index)
{
	if(index<0||index>LsCirX.GetUpperBound())
		return FALSE;
	Cir = LsCirX[index];
	return TRUE;
}

BOOL CNcJg::GetYLsCircle(f3dCircle &Cir,int index)
{
	if(index<0||index>LsCirY.GetUpperBound())
		return FALSE;
	Cir = LsCirY[index];
	return TRUE;
}

BOOL CNcJg::GenNCFile(const char *sFileName,const char *sPartNoPrefix/*=NULL*/)
{
	int i,j;
	f3dCircle tm_cir;
	for(i=0;i<GetXLsCount();i++)
	{
		BOOL bSorted = TRUE;
		for(j=0;j<GetXLsCount()-i-1;j++)
		{
			if(LsCirX[j].centre.x>LsCirX[j+1].centre.x)
			{
				tm_cir = LsCirX[j];
				LsCirX[j] = LsCirX[j+1];
				LsCirX[j+1] = tm_cir;
				bSorted = FALSE;
			}
		}
		if(bSorted)	//已有序
			break;
	}
	for(i=0;i<GetYLsCount();i++)
	{
		BOOL bSorted = TRUE;
		for(j=0;j<GetYLsCount()-i-1;j++)
		{
			if(LsCirY[j].centre.x>LsCirY[j+1].centre.x)
			{
				tm_cir = LsCirY[j];
				LsCirY[j] = LsCirY[j+1];
				LsCirY[j+1] = tm_cir;
				bSorted = FALSE;
			}
		}
		if(bSorted)	//已有序
			break;
	}

	FILE *fp;
	char nc_file[MAX_PATH];
	strcpy(nc_file,sFileName);
	if(strlen(NcManager.extension)>0)
	{
		strcat(nc_file,".");
		strcat(nc_file,NcManager.extension);
	}
	strncpy(sPrefix,sPartNoPrefix,19);
	if(NcManager.m_bPrintByASCII)	//输出文本文件
	{
		if((fp=fopen(nc_file,"wt"))==NULL)
			return FALSE;
	}
	else				//输出二进制文件
	{
		if((fp=fopen(nc_file,"wb"))==NULL)
			return FALSE;
	}
	for(CVariant *pLine=NcManager.print_list.GetFirst();pLine;pLine=NcManager.print_list.GetNext())
	{
		if(pLine->sVal==NULL)
			continue;
		char sToken[20],sFormat[20];
		int i,n = strlen(pLine->sVal);
		CCirStack<int>ItemFunc;
		CCirStack<int>ItemPos;
		memset(sToken,'\0',20);
		memset(sFormat,'\0',20);
		if(stricmp(pLine->sVal,"</WING_X_HOLE>")==0)
		{
			//圆整相对距离与绝对距离的精度四舍五入误差
			CDistFillet distFilletX;
			distFilletX.coordArr.SetSize(LsCirX.GetSize());
			for(i=0;i<LsCirX.GetSize();i++)
				distFilletX.coordArr[i]=LsCirX[i].centre.x;
			distFilletX.Fillet();
			for(i=0;i<LsCirX.GetSize();i++)
				LsCirX[i].centre.x=distFilletX.coordArr[i];
			current_len_pos=current_accurate_len_pos=0;
			for(i=0;i<GetXLsCount();i++)
			{
				NcManager.print_list.push_stack();
				GetXLsCircle(tm_cir,i);
				pLine=NcManager.print_list.GetNext();
				while(pLine&&stricmp(pLine->sVal,"<WING_X_HOLE>")!=0)
				{
					PrintLn(fp,pLine->sVal,&tm_cir);
					pLine=NcManager.print_list.GetNext();
				}
				NcManager.print_list.pop_stack();
			}
			pLine=NcManager.print_list.GetNext();
			while(pLine&&stricmp(pLine->sVal,"<WING_X_HOLE>")!=0)
				pLine=NcManager.print_list.GetNext();
			continue;
		}
		else if(stricmp(pLine->sVal,"</WING_Y_HOLE>")==0)
		{
			//圆整相对距离与绝对距离的精度四舍五入误差
			CDistFillet distFilletY;
			distFilletY.coordArr.SetSize(LsCirY.GetSize());
			for(i=0;i<LsCirY.GetSize();i++)
				distFilletY.coordArr[i]=LsCirY[i].centre.x;
			distFilletY.Fillet();
			for(i=0;i<LsCirY.GetSize();i++)
				LsCirY[i].centre.x=distFilletY.coordArr[i];
			current_len_pos=current_accurate_len_pos=0;
			for(i=0;i<GetYLsCount();i++)
			{
				NcManager.print_list.push_stack();
				GetYLsCircle(tm_cir,i);
				pLine=NcManager.print_list.GetNext();
				while(pLine&&stricmp(pLine->sVal,"<WING_Y_HOLE>")!=0)
				{
					PrintLn(fp,pLine->sVal,&tm_cir);
					pLine=NcManager.print_list.GetNext();
				}
				NcManager.print_list.pop_stack();
			}
			pLine=NcManager.print_list.GetNext();
			while(pLine&&stricmp(pLine->sVal,"<WING_Y_HOLE>")!=0)
				pLine=NcManager.print_list.GetNext();
			continue;
		}
		PrintLn(fp,pLine->sVal);
	}
	//*甘肃天水煅压机床厂
	/*fprintf(fp,"\"P:L%.0f*%.0f\"\n",wing_wide,wing_thick);
	if(cMaterial!='H')
		fprintf(fp,"\"M:Q235\"\n");
	else
		fprintf(fp,"\"M:Q345\"\n");
	fprintf(fp,"\"LP:%.0f\"\n",length);
	fprintf(fp,"\"SA%.0f SB%.0f TA%.0f TB%.0f\"\n",wing_wide,wing_wide,wing_thick,wing_thick);
	for(i=0;i<GetXLsCount();i++)
	{
		GetXLsCircle(tm_cir,i);
		fprintf(fp,"\"DA%.1f \"\n\"X%.0f TR%.0f\"\n",tm_cir.radius*2,tm_cir.centre.x,tm_cir.centre.y);
	}
	for(i=0;i<GetYLsCount();i++)
	{
		GetYLsCircle(tm_cir,i);
		fprintf(fp,"\"DB%.1f \"\n\"X%.0f TR%.0f\"\n",tm_cir.radius*2,tm_cir.centre.x,tm_cir.centre.y);
	}
	double fPartNoStampPos=100;
	for(i=0;i<GetXLsCount();i++)
	{
		GetXLsCircle(tm_cir,i);
		if(tm_cir.centre.x-fPartNoStampPos<60)
		{
			fPartNoStampPos = tm_cir.centre.x;
			continue;
		}
		else
			break;
	}
	fprintf(fp,"\"MK&%s\"\n\"& X%.0f\"\n",sPartNo,fPartNoStampPos);
	fprintf(fp,"\"M30\"\n");
	fprintf(fp,"\"END\"\n");*/
	fclose(fp);
	return TRUE;
}

void CNcJg::PrintLn(FILE* fp,char *format, f3dCircle *ls_cir)
{
	if(format==NULL)
		return;;
	char sToken[20],sFormat[40],sLen[20];
	int cur_val,i,n = strlen(format);
	CCirStack<int>ItemFunc;
	CCirStack<int>ItemPos;
	memset(sToken,'\0',20);
	memset(sFormat,'\0',20);
	memset(sLen,'\0',20);
	for(i=0;i<n;i++)
	{
		if(format[i]=='<')
		{
			ItemFunc.push('<');
			ItemPos.push(i+1);
		}
		else if(format[i]=='>')
		{
			if(*ItemFunc.TopAtom()=='<')
				ItemFunc.pop(cur_val);
		}
		else if(format[i]=='#')
			fprintf(fp,"\n");
		else if(format[i]=='%')
		{
			if(*ItemFunc.TopAtom()=='%')
			{
				ItemFunc.pop(cur_val);
				ItemPos.pop(cur_val);
				memset(sToken,'\0',20);
				if(strlen(sFormat)>0)
				{
					if(strlen(sLen)>0)
						strncpy(sToken,&format[cur_val],i-cur_val-strlen(sFormat)-strlen(sLen)-3);
					else
						strncpy(sToken,&format[cur_val],i-cur_val-strlen(sFormat)-2);
				}
				else
					strncpy(sToken,&format[cur_val],i-cur_val);
				if(stricmp(sToken,"BYTE")==0)
				{
					char ch = atoi(sFormat);
					if(NcManager.m_bPrintByASCII)
						fprintf(fp,"%c",ch);
					else
						fwrite(&ch,1,1,fp);
				}
				else if(stricmp(sToken,"LENGTH")==0)
				{	//角钢长度
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							fprintf(fp,"%.0f",length);
						else //if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							fprintf(fp,"%.1f",length);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)(length+0.5);
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = (int)(length+0.5);
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)length;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&length,8,1,fp);
						}
						else //if(stricmp(sFormat,"STRING")==0)
						{
							char sText[101];
							memset(sText,' ',101);
							_snprintf(sText,100,"%.0f",length);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								sText[strlen(sText)]=' ';
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
				else if(stricmp(sToken,"WING_WIDE")==0)
				{	//肢宽
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							fprintf(fp,"%.0f",wing_wide);
						else// if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							fprintf(fp,"%.1f",wing_wide);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)(wing_wide+0.5);
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = (int)(wing_wide+0.5);
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)wing_wide;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&wing_wide,8,1,fp);
						}
						else //if(stricmp(sFormat,"STRING")==0)
						{
							char sText[101];
							memset(sText,' ',101);
							_snprintf(sText,100,"%.0f",wing_wide);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								sText[strlen(sText)]=' ';
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
				else if(stricmp(sToken,"WING_THICK")==0)
				{	//肢厚
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							fprintf(fp,"%.0f",wing_thick);
						else// if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							fprintf(fp,"%.1f",wing_thick);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)(wing_thick+0.5);
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = (int)(wing_thick+0.5);
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)wing_thick;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&wing_thick,8,1,fp);
						}
						else //if(stricmp(sFormat,"STRING")==0)
						{
							char sText[101];
							memset(sText,' ',101);
							_snprintf(sText,100,"%.0f",wing_thick);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								sText[strlen(sText)]=' ';
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
				else if(stricmp(sToken,"MATERIAL")==0)
				{	//材质
					if(NcManager.m_bPrintByASCII)
					{
						if(toupper(cMaterial)==CSteelMatLibrary::Q235BriefMark())//'S')
							fprintf(fp,"%s",NcManager.sQ235);
						else if(toupper(cMaterial)==CSteelMatLibrary::Q345BriefMark())//'H')
							fprintf(fp,"%s",NcManager.sQ345);
						else if(toupper(cMaterial)==CSteelMatLibrary::Q390BriefMark())//'G')
							fprintf(fp,"%s",NcManager.sQ390);
						else if(toupper(cMaterial)==CSteelMatLibrary::Q420BriefMark())//'T')
							fprintf(fp,"%s",NcManager.sQ420);
						else //if(toupper(cMaterial)==CSteelMatLibrary::Q460BriefMark())//'U')
							fprintf(fp,"%s",NcManager.sQ460);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short mat;
							if(toupper(cMaterial)==CSteelMatLibrary::Q235BriefMark())//'S')
								mat = (short)atoi(NcManager.sQ235);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q345BriefMark())//'H')
								mat = (short)atoi(NcManager.sQ345);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q390BriefMark())//'G')
								mat = (short)atoi(NcManager.sQ390);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q420BriefMark())//'T')
								mat = (short)atoi(NcManager.sQ420);
							else //if(toupper(cMaterial)==CSteelMatLibrary::Q460BriefMark())//'U')
								mat = (short)atoi(NcManager.sQ460);
							fwrite(&mat,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int mat;
							if(toupper(cMaterial)==CSteelMatLibrary::Q235BriefMark())//'S')
								mat = (short)atoi(NcManager.sQ235);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q345BriefMark())//'H')
								mat = (short)atoi(NcManager.sQ345);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q390BriefMark())//'G')
								mat = (short)atoi(NcManager.sQ390);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q420BriefMark())//'T')
								mat = (short)atoi(NcManager.sQ420);
							else //if(toupper(cMaterial)==CSteelMatLibrary::Q460BriefMark())//'U')
								mat = (short)atoi(NcManager.sQ460);
							fwrite(&mat,2,1,fp);
						}
						else //if(stricmp(sFormat,"STRING")==0)
						{
							char sText[100];
							memset(sText,' ',100);
							if(toupper(cMaterial)==CSteelMatLibrary::Q235BriefMark())//'S')
								sprintf(sText,"%s",NcManager.sQ235);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q345BriefMark())//'H')
								sprintf(sText,"%s",NcManager.sQ345);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q390BriefMark())//'G')
								sprintf(sText,"%s",NcManager.sQ390);
							else if(toupper(cMaterial)==CSteelMatLibrary::Q420BriefMark())//'T')
								sprintf(sText,"%s",NcManager.sQ420);
							else //if(toupper(cMaterial)==CSteelMatLibrary::Q460BriefMark())//'U')
								sprintf(sText,"%s",NcManager.sQ460);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								sText[strlen(sText)]=' ';
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
				else if(stricmp(sToken,"PART_NO")==0)
				{	//构件编号
					if(NcManager.m_bPrintByASCII)
						fprintf(fp,"%s%s",sPrefix,sPartNo);
					else
					{
						char sText[100];
						memset(sText,' ',100);
						strcpy(sText,sPrefix);
						strcat(sText,sPartNo);
						if(strlen(sLen)>0)
						{
							int len = atoi(sLen);
							sText[strlen(sText)]=' ';
							fwrite(sText,len,1,fp);
						}
						else
							fwrite(sText,strlen(sText),1,fp);
					}
				}
				else if(stricmp(sToken,"HOLE_HEAD")==0)
				{	//冲头信息
					for(HOLE_HEAD *pHoleHead=NcManager.hole_head.GetFirst();pHoleHead;pHoleHead=NcManager.hole_head.GetNext())
					{
						if(fabs(pHoleHead->d-ls_cir->radius*2)<1)
						{
							if(NcManager.m_bPrintByASCII)
								fprintf(fp,"%s",pHoleHead->alias);
							else
								fwrite(&pHoleHead->alias,strlen(pHoleHead->alias),1,fp);
							break;
						}
					}
				}
				else if(stricmp(sToken,"JG_GUIGE")==0)
				{	//角钢规格
					char sText[100];
					memset(sText,' ',100);
					sprintf(sText,"L%.0f*%.0f",wing_wide,wing_thick);
					if(strlen(sLen)>0)
					{
						int len = atoi(sLen);
						sText[strlen(sText)]=' ';
						fwrite(sText,len,1,fp);
					}
					else
						fwrite(sText,strlen(sText),1,fp);
				}
				else if(stricmp(sToken,"HOLE_G_POS")==0)
				{	//螺栓心距
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							fprintf(fp,"%.0f",ls_cir->centre.y);
						else// if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							fprintf(fp,"%.1f",ls_cir->centre.y);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)(ls_cir->centre.y+0.5);
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = (int)(ls_cir->centre.y+0.5);
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)ls_cir->centre.y;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&ls_cir->centre.y,8,1,fp);
						}
						else// if(stricmp(sFormat,"STRING")==0)
						{
							char sText[100];
							memset(sText,'\0',100);
							sprintf(sText,"%.0f",ls_cir->centre.y);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
				else if(stricmp(sToken,"HOLE_LEN_POS")==0)
				{	//螺栓纵向位置
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							fprintf(fp,"%s%.0f",NcManager.sAbsolutePosMark,ls_cir->centre.x);
						else// if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							fprintf(fp,"%s%.1f",NcManager.sAbsolutePosMark,ls_cir->centre.x);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)(ls_cir->centre.x+0.5);
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = (int)(ls_cir->centre.x+0.5);
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)ls_cir->centre.x;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&ls_cir->centre.x,8,1,fp);
						}
						else// if(stricmp(sFormat,"STRING")==0)
						{
							char sText[100];
							memset(sText,'\0',100);
							sprintf(sText,"%s%.0f",NcManager.sAbsolutePosMark,ls_cir->centre.x);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
				else if(stricmp(sToken,"HOLE_LEN_REFPOS")==0)
				{	//螺栓纵向位置
					double space=ls_cir->centre.x-current_len_pos;
					double space_accurate=ls_cir->centre.x-current_accurate_len_pos;
					if(space_accurate<=80)
						space=space_accurate;
					char sText[200]="";
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							space=ftoi(space);
						else// if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							space=ftoi(space*10)/10.0;
						if(current_len_pos==0)
							sprintf(sText,"%s%.1f",NcManager.sAbsolutePosMark,space);
						else
							sprintf(sText,"%s%.1f",NcManager.sRelativePosMark,space);
						SimplifiedNumString(sText);
						fprintf(fp,"%s",sText);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)ftoi(space);
							space=val;
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = ftoi(space);
							space=val;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)(space);
							space=val;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&space,8,1,fp);
						}
						else// if(stricmp(sFormat,"STRING")==0)
						{
							char sText[100];
							memset(sText,'\0',100);
							if(current_len_pos==0)
								sprintf(sText,"%s%.0f",NcManager.sAbsolutePosMark,space);
							else
								sprintf(sText,"%s%.0f",NcManager.sRelativePosMark,space);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
					current_len_pos+=space;
					current_accurate_len_pos=ls_cir->centre.x;
				}
				else if(stricmp(sToken,"MARK_POS")==0||stricmp(sToken,"-MARK_POS")==0)
				{	//打号位置
					bool bReverseOrder=false;
					if(stricmp(sToken,"-MARK_POS")==0)
						bReverseOrder=true;	//从末尾向前面找字号位置
					f3dCircle tm_cir;
					const int STAMP_MAX_LEN=150;
					double fPartNoStampPos=200;
					if(bReverseOrder)
					{
						fPartNoStampPos=length-200;
						while(1)
						{
							int j=0;
							BOOL bProperPos = TRUE;
							for(j=GetXLsCount()-1;j>=0;j--)
							{
								GetXLsCircle(tm_cir,j);
								if(tm_cir.centre.x>fPartNoStampPos-20&&tm_cir.centre.x<fPartNoStampPos+STAMP_MAX_LEN)
								{
									fPartNoStampPos -= 50;
									bProperPos = FALSE;
									continue;
								}
							}
							for(j=GetYLsCount()-1;j>=0;j--)
							{
								GetYLsCircle(tm_cir,j);
								if(tm_cir.centre.x>fPartNoStampPos-20&&tm_cir.centre.x<fPartNoStampPos+STAMP_MAX_LEN)
								{
									fPartNoStampPos -= 50;
									bProperPos = FALSE;
									continue;
								}
							}
							if(bProperPos)
								break;
						}
					}
					else
					{
						while(1)
						{
							int j=0;
							BOOL bProperPos = TRUE;
							for(j=0;j<GetXLsCount();j++)
							{
								GetXLsCircle(tm_cir,j);
								if(tm_cir.centre.x>fPartNoStampPos-20&&tm_cir.centre.x<fPartNoStampPos+STAMP_MAX_LEN)
								{
									fPartNoStampPos += 50;
									bProperPos = FALSE;
									continue;
								}
							}
							for(j=0;j<GetYLsCount();j++)
							{
								GetYLsCircle(tm_cir,j);
								if(tm_cir.centre.x>fPartNoStampPos-20&&tm_cir.centre.x<fPartNoStampPos+STAMP_MAX_LEN)
								{
									fPartNoStampPos += 50;
									bProperPos = FALSE;
									continue;
								}
							}
							if(bProperPos)
								break;
						}
					}
					if(NcManager.m_bPrintByASCII)
					{
						if(stricmp(sFormat,"SHORT")==0||stricmp(sFormat,"INTEGER")==0)
							fprintf(fp,"%.0f",fPartNoStampPos);
						else// if(stricmp(sFormat,"FLOAT")==0||stricmp(sFormat,"DOUBLE")==0)
							fprintf(fp,"%.1f",fPartNoStampPos);
					}
					else
					{
						if(stricmp(sFormat,"SHORT")==0)
						{
							short val = (short)(fPartNoStampPos+0.5);
							fwrite(&val,2,1,fp);
						}
						else if(stricmp(sFormat,"INTEGER")==0)
						{
							int val = (int)(fPartNoStampPos+0.5);
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"FLOAT")==0)
						{
							float val = (float)fPartNoStampPos;
							fwrite(&val,4,1,fp);
						}
						else if(stricmp(sFormat,"DOUBLE")==0)
						{
							fwrite(&fPartNoStampPos,8,1,fp);
						}
						else// if(stricmp(sFormat,"STRING")==0)
						{
							char sText[100];
							memset(sText,'\0',100);
							sprintf(sText,"%.0f",fPartNoStampPos);
							if(strlen(sLen)>0)
							{
								int len = atoi(sLen);
								fwrite(sText,len,1,fp);
							}
							else
								fwrite(sText,strlen(sText),1,fp);
						}
					}
				}
			}
			else
			{
				ItemFunc.push('%');
				ItemPos.push(i+1);
				memset(sFormat,'\0',20);
			}
		}
		else if(format[i]=='$')
		{
			if(*ItemFunc.TopAtom()=='$')
			{
				ItemFunc.pop(cur_val);
				ItemPos.pop(cur_val);
				memset(sFormat,'\0',20);
				strncpy(sFormat,&format[cur_val],i-cur_val);
				size_t str_n = strlen(sFormat);
				for(size_t jj=0;jj<str_n;jj++)
				{
					if(sFormat[jj]=='@')
					{
						sFormat[jj]='\0';
						if(strlen(sFormat)-jj-1>20)
							strncpy(sLen,&sFormat[jj+1],20);
						else
							strcpy(sLen,&sFormat[jj+1]);
						break;
					}
				}
				if(str_n==strlen(sFormat))
					strcpy(sLen,"");
			}
			else
			{
				ItemFunc.push('$');
				ItemPos.push(i+1);
			}
		}
		else if(*ItemFunc.TopAtom()=='<')
			fprintf(fp,"%c",format[i]);
	}
	if(NcManager.m_bPrintByASCII)
		fprintf(fp,"\n");
}