#include "StdAfx.h"
#include "XerocadDrawing.h"

BOOL ToBuffer(CBuffer& buffer,IDbEntity* pEnt)
{
	if(pEnt==NULL)
		return FALSE;
	buffer.WriteInteger(pEnt->GetDbEntType());
	if(pEnt->GetDbEntType()==IDbEntity::DbLine)
	{
		IDbLine* pDbLines=(IDbLine*)pEnt;
		buffer.WritePoint(pDbLines->StartPosition());
		buffer.WritePoint(pDbLines->EndPosition());
	}
	else if(pEnt->GetDbEntType()==IDbEntity::DbText||pEnt->GetDbEntType()==IDbEntity::DbMText) 
	{
		IDbText* pText=(IDbText*)pEnt;;
		buffer.WriteString(pText->GetTextString());
		buffer.WriteDouble(pText->GetHeight());
		buffer.WriteInteger(pText->GetTextStyleId());
		buffer.WriteInteger(pText->GetAlignment());
		buffer.WriteDouble(pText->GetRotation());
		buffer.WritePoint(pText->GetPosition());
		if(pEnt->GetDbEntType()==IDbEntity::DbMText)
		{
			IDbMText* pMText=(IDbMText*)pEnt;
			buffer.WriteDouble(pMText->GetWidth());
		}
	}
	else if(pEnt->GetDbEntType()==IDbEntity::DbCircle)
	{
		IDbCircle* pCircle=(IDbCircle*)pEnt;
		buffer.WritePoint(pCircle->Center());
		buffer.WritePoint(pCircle->Normal());
		buffer.WriteDouble(pCircle->Radius());
	}
	else  if (pEnt->GetDbEntType()==IDbEntity::DbPolyline)
	{
		IDbPolyline* pPolyline=(IDbPolyline*)pEnt;
		buffer.WriteInteger((int)pPolyline->numVerts());
		for(int	i=0;i<pPolyline->numVerts();i++)
		{
			GEPOINT pos;
			pPolyline->GetVertexAt(i,pos);
			buffer.WritePoint(pos);
		}
	}
	else
		return FALSE;
	return TRUE;
}


BOOL FromBuffer(CBuffer& buffer,IDbEntity* pEnt)
{
	long iEntType=pEnt->GetDbEntType();
	double fValue=0;
	GEPOINT pos;
	CXhChar200 sTemp;
	if(IDbEntity::DbLine==iEntType)
	{
		IDbLine* pDbLines=(IDbLine*)pEnt;
		buffer.ReadPoint(pos);
		pDbLines->SetStartPosition(pos);
		buffer.ReadPoint(pos);
		pDbLines->SetEndPosition(pos);
	}
	else if(iEntType==IDbEntity::DbText||IDbEntity::DbMText==iEntType) 
	{
		IDbText* pText=(IDbText*)pEnt;
		buffer.ReadString(sTemp);
		pText->SetTextString(sTemp);
		buffer.ReadDouble(&fValue);
		pText->SetHeight(fValue);
		pText->SetTextStyleId(buffer.ReadInteger());
		pText->SetAlignment(buffer.ReadInteger());
		buffer.ReadDouble(&fValue);
		pText->SetRotation(fValue);
		buffer.ReadPoint(pos);
		pText->SetPosition(pos);
		if(IDbEntity::DbMText==iEntType)
		{
			buffer.ReadDouble(&fValue);
			((IDbMText*)pText)->SetWidth(fValue);
		}
	}
	else if(iEntType==IDbEntity::DbCircle)
	{
		IDbCircle* pCircle=(IDbCircle*)pEnt;
		buffer.ReadPoint(pos);
		pCircle->SetCenter(pos);
		buffer.ReadPoint(pos);
		pCircle->SetNormal(pos);
		buffer.ReadDouble(&fValue);
		pCircle->SetRadius(fValue);
	}
	else if (iEntType==IDbEntity::DbPolyline)
	{
		IDbPolyline* pPolyline=(IDbPolyline*)pEnt;
		int nNumVerts=0;
		buffer.ReadInteger(&nNumVerts);
		for(int	i=0;i<nNumVerts;i++)
		{
			GEPOINT pos;
			buffer.ReadPoint(pos);
			pPolyline->AddVertexAt(i,pos);
		}
	}
	return TRUE;
}
bool DrawingToBuffer( IDrawing *pDrawing,CBuffer& buffer)
{
	BUFFERPOP stack(&buffer,0);
	buffer.WriteInteger(0);
	int i=0;
	for (IDbEntity* pEnt=pDrawing->EnumFirstDbEntity();pEnt;pEnt=pDrawing->EnumNextDbEntity())
	{
		ToBuffer(buffer,pEnt);
		stack.Increment();
	}
	stack.VerifyAndRestore();
	return true;
}

bool AttachDataToBuffer(CHashList<GRID_DATA_STRU>&hashAttachDataByPointId,CBuffer& buufer)
{
	buufer.SeekToEnd();
	int ncount=hashAttachDataByPointId.GetNodeNum();
	buufer.WriteInteger(hashAttachDataByPointId.GetNodeNum());
	for (GRID_DATA_STRU* pDataStru=hashAttachDataByPointId.GetFirst();pDataStru;pDataStru=hashAttachDataByPointId.GetNext())
	{
		buufer.WriteDword(hashAttachDataByPointId.GetCursorKey());
		buufer.WriteInteger(pDataStru->data_type);
		buufer.WriteInteger(pDataStru->type_id);
		buufer.WriteDouble(pDataStru->min_x);
		buufer.WriteDouble(pDataStru->min_y);
		buufer.WriteDouble(pDataStru->max_x);
		buufer.WriteDouble(pDataStru->max_y);
		buufer.WriteDouble(pDataStru->fTextHigh);
		buufer.WriteDouble(pDataStru->fHighToWide);
		buufer.WriteString(pDataStru->sContent);
	}
	return true;
}
int InitDrawingFromBuffer( IDrawing *pDrawing,CBuffer& buffer)
{
	buffer.SeekToBegin();
	int nEntCount=buffer.ReadInteger();
	for(int i=0;i<nEntCount;i++)
	{
		long iIdbentityType=0;
		buffer.ReadInteger(&iIdbentityType);
		IDbEntity* pEnt=pDrawing->AppendDbEntity(iIdbentityType);
		FromBuffer(buffer,pEnt);
	}
	return nEntCount;
}

bool InitAttachDataFromBuffer(CHashList<GRID_DATA_STRU>&hashAttachDataByPointId,CBuffer& buufer)
{
	int nDataCount=0;
	buufer.ReadInteger(&nDataCount);
	for (int i=0;i<nDataCount;i++)
	{
		DWORD iPointId=0;
		buufer.ReadDword(&iPointId);
		GRID_DATA_STRU *pDataStru=hashAttachDataByPointId.Add(iPointId);
		buufer.ReadInteger(&pDataStru->data_type);
		buufer.ReadInteger(&pDataStru->type_id);
		buufer.ReadDouble(&pDataStru->min_x);
		buufer.ReadDouble(&pDataStru->min_y);
		buufer.ReadDouble(&pDataStru->max_x);
		buufer.ReadDouble(&pDataStru->max_y);
		buufer.ReadDouble(&pDataStru->fTextHigh);
		buufer.ReadDouble(&pDataStru->fHighToWide);
		buufer.ReadString(pDataStru->sContent);
	}
	return true;
}

bool SaveXeroDrawingAndDataToFile(IDrawing *pDrawing,CHashList<GRID_DATA_STRU>& hashAttachDataByPointId,const char* sFileName)
{
	CBuffer buffer; 
	DrawingToBuffer(pDrawing,buffer);
	AttachDataToBuffer(hashAttachDataByPointId,buffer);
	buffer.SeekToBegin();
	FILE* fp=fopen(sFileName,"wb");
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	return true;
}
bool InitXeroDrawingAndDataFromFile(IDrawing *pDrawing,CHashList<GRID_DATA_STRU>& hashAttachDataByPointId,const char* sFileName)
{
	FILE* fp=fopen(sFileName,"rb");
	if(fp==NULL)
		return false;
	int nFileLen=fseek(fp,0,SEEK_END);
	nFileLen=ftell(fp);
	fseek(fp,0,SEEK_SET);
	CBuffer buffer;
	buffer.WriteAt(0,NULL,nFileLen);
	fread(buffer.GetBufferPtr(),nFileLen,1,fp);
	fclose(fp);
	InitDrawingFromBuffer(pDrawing,buffer);
	InitAttachDataFromBuffer(hashAttachDataByPointId,buffer);
	return false;
}