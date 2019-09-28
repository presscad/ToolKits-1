
#include "stdafx.h"
#include "CopyPasteOper.h"

BOOL ReadClipPoint(f3dPoint &point)
{
	FILE *fp;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerClip.tmp");
	if((fp=fopen(filename,"rt"))==NULL)
	{
		AfxMessageBox("无数据可粘贴!");
		return FALSE;
	}
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&point.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&point.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&point.z);
	fclose(fp);
	return TRUE;
}
void WritePointToClip(f3dPoint point)
{
	FILE *fp;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerClip.tmp");
	if((fp=fopen(filename,"wt"))==NULL)
	{
		AfxMessageBox("临时文件读写失败,粘贴失败!");
		return;
	}
	fprintf(fp,"%.14lf ",point.x);
	fprintf(fp,"%.14lf ",point.y);
	fprintf(fp,"%.14lf ",point.z);
	fclose(fp);
}
void ClearClipboard()
{
	FILE *fp;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerClip.tmp");
	if((fp=fopen(filename,"w+"))==NULL)
	{
		AfxMessageBox("临时文件清空失败!");
		return;
	}
	fclose(fp);
}
BOOL ReadClipUCS(UCS_STRU &ucs,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	UCS_STRU tm_ucs;
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerUcs.tmp");
	if(fp==NULL)
	{
		bExternalFP=FALSE;
		if((fp=fopen(filename,"rt"))==NULL)
		{
			AfxMessageBox("无数据可粘贴!");
			return FALSE;
		}
	}
	//Read ucs origin
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.origin.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.origin.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.origin.z);
	//Read axis X
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_x.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_x.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_x.z);
	//Read axis Y
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_y.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_y.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_y.z);
	//Read axis z
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_z.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_z.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&tm_ucs.axis_z.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
	ucs = tm_ucs;
	return TRUE;
}
void WriteUCSToClip(UCS_STRU &ucs,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	if(fp==NULL)
	{
		bExternalFP = FALSE;
		strcat(filename,"\\TowerUcs.tmp");
		if((fp=fopen(filename,"wt"))==NULL)
		{
			AfxMessageBox("临时文件读写失败,粘贴失败!");
			return;
		}
	}
	fprintf(fp,"%.14lf ",ucs.origin.x);
	fprintf(fp,"%.14lf ",ucs.origin.y);
	fprintf(fp,"%.14lf ",ucs.origin.z);
	fprintf(fp,"%.14lf ",ucs.axis_x.x);
	fprintf(fp,"%.14lf ",ucs.axis_x.y);
	fprintf(fp,"%.14lf ",ucs.axis_x.z);
	fprintf(fp,"%.14lf ",ucs.axis_y.x);
	fprintf(fp,"%.14lf ",ucs.axis_y.y);
	fprintf(fp,"%.14lf ",ucs.axis_y.z);
	fprintf(fp,"%.14lf ",ucs.axis_z.x);
	fprintf(fp,"%.14lf ",ucs.axis_z.y);
	fprintf(fp,"%.14lf \n",ucs.axis_z.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
}
BOOL ReadClipPlane(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	UCS_STRU tm_ucs;
	GetSystemDirectory(filename,MAX_PATH);
	strcat(filename,"\\TowerPlane.tmp");
	if(fp==NULL)
	{
		bExternalFP=FALSE;
		if((fp=fopen(filename,"rt"))==NULL)
		{
			AfxMessageBox("无数据可粘贴!");
			return FALSE;
		}
	}
	//Read 基准点
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&base_pos.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&base_pos.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&base_pos.z);
	//Read 工作法线
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&work_norm.x);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&work_norm.y);
	if(feof(fp))
	{
		AfxMessageBox("粘贴数据不完整");
		if(!bExternalFP)
			fclose(fp);
		return FALSE;
	}
	fscanf(fp,"%lf ",&work_norm.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
	return TRUE;
}
void WritePlaneToClip(f3dPoint &base_pos,f3dPoint &work_norm,FILE *fp)
{
	BOOL bExternalFP=TRUE;
	char filename[MAX_PATH];
	GetSystemDirectory(filename,MAX_PATH);
	if(fp==NULL)
	{
		bExternalFP = FALSE;
		strcat(filename,"\\TowerPlane.tmp");
		if((fp=fopen(filename,"wt"))==NULL)
		{
			AfxMessageBox("临时文件读写失败,粘贴失败!");
			return;
		}
	}
	fprintf(fp,"%.14lf ",base_pos.x);
	fprintf(fp,"%.14lf ",base_pos.y);
	fprintf(fp,"%.14lf ",base_pos.z);
	fprintf(fp,"%.14lf ",work_norm.x);
	fprintf(fp,"%.14lf ",work_norm.y);
	fprintf(fp,"%.14lf \n",work_norm.z);
	if(!bExternalFP)	//只有内部文件需要关闭此文件
		fclose(fp);
}