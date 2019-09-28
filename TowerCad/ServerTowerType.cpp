#include "StdAfx.h"
#include <stdlib.h>
#include "ServerTowerType.h"
#include "ArrayList.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////////
// CServerFile
IMPLEMENT_GET(CServerTowerPart,ppi)
{
	if(_ciPartType==CProcessPart::TYPE_PLATE)
		return plate;
	else
		return angle;
}
CServerTowerPart::CServerTowerPart(UINT key)
{
	plate.SetKey(key);
	angle.SetKey(key);
	ppifiledata=NULL;
	_uiPpiFileLength=0;
	_id=m_idTowerType=m_idSrvFileObj=0;
	m_uiCurrTidVerSerial=m_uiCurrPpiVerSerial=0;
	_ciPartType=CProcessPart::TYPE_LINEANGLE;
	m_sServerClassName="TowerPart";
	m_cbSyncState=SYNC_NONE;
}
CServerTowerPart::CServerTowerPart(CProcessPart* pPPIPart/*=NULL*/)
{
	ppifiledata=NULL;
	_uiPpiFileLength=0;
	_id=m_idTowerType=m_idSrvFileObj=0;
	m_uiCurrTidVerSerial=m_uiCurrPpiVerSerial=0;
	m_sServerClassName="TowerPart";
	_ciPartType=CProcessPart::TYPE_LINEANGLE;
	if(pPPIPart)
		InitByPPIPart(pPPIPart);
}
CServerTowerPart::~CServerTowerPart()
{
	if(ppifiledata!=NULL)
		delete[] ppifiledata;
	ppifiledata=NULL;
	_uiPpiFileLength=0;
}
bool CServerTowerPart::ClonePart(CServerTowerPart *pNewPart)
{	//克隆PPI构件信息
	if(pNewPart==NULL)
		return false;
	char pool[5120];//5120=4096+1024;
	CBuffer partBuffer(pool,5120);
	partBuffer.ClearContents();
	ppi.ToPPIBuffer(partBuffer);
	partBuffer.SeekToBegin();
	pNewPart->FromPPIBuffer(partBuffer);
	_id=pNewPart->Id;
	m_idTowerType=pNewPart->m_idTowerType;
	m_idSrvFileObj=pNewPart->m_idSrvFileObj;
	m_uiCurrTidVerSerial=pNewPart->m_uiCurrTidVerSerial;
	return true;
}
void CServerTowerPart::FromPPIBuffer(CBuffer &buffer)
{
	_ciPartType=CProcessPart::RetrievedPartTypeAndLabelFromBuffer(buffer);
	if(_ciPartType==CProcessPart::TYPE_PLATE)
		plate.FromPPIBuffer(buffer);
	else
		angle.FromPPIBuffer(buffer);
}

void CServerTowerPart::InitByPPIPart(CProcessPart* pPPIPart)
{
	CBuffer buf;
	pPPIPart->ToPPIBuffer(buf);
	buf.SeekToBegin();
	FromPPIBuffer(buf);
}
void CServerTowerPart::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if( bIncludeId&&(
		dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_LABELnID)>0))
		buffer.WriteInteger(this->m_id);//_id);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_LABELnID)>0)
		buffer.WriteString(ppi.Label);//_partNo);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_BASICNONEID)>0)
	{
		//buffer.WriteInteger(m_idTowerType);
		buffer.WriteInteger(m_uiCurrPpiVerSerial);
		buffer.WriteInteger(m_uiCurrTidVerSerial);
		buffer.WriteByte(ppi.m_cPartType);	//构件类型
		//buffer.WriteInteger(0);//_materialTypeId);
		buffer.WriteByte(ppi.cMaterial);
		//buffer.WriteString("");//_materialMark);
		buffer.WriteInteger(1);//_num);
		buffer.WriteDword(ppi.iSeg);
		buffer.WriteDouble(ppi.m_fWidth);
		buffer.WriteDouble(ppi.m_fThick);
		buffer.WriteWord(ppi.m_wLength);
		CXhChar50 sizestr=ppi.GetSpec(true);
		buffer.WriteString(sizestr);//_specification);
		buffer.WriteDouble(ppi.GetWeight());//_weight);
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_PROCESSDATA)>0)
	{
		buffer.WriteDouble(ppi.GetWeight());//_realWeight);
		buffer.WriteDouble(ppi.GetWeight());//_minWeight);
		buffer.WriteDouble(0);//_realArea);
		buffer.WriteDouble(0);//_minArea);
		buffer.WriteDouble(0);//_weldingLineVolume);
		buffer.WriteDouble(0);//_wingAngle);
		buffer.WriteInteger(0);//_m16LsNum);
		buffer.WriteInteger(0);//_m20LsNum);
		buffer.WriteInteger(0);//_m24LsNum);
		buffer.WriteInteger(0);//_mxLsNum);
		buffer.WriteInteger(0);//_manuHourBore);
		buffer.WriteInteger(0);//_manuHourClashHole);
		buffer.WriteInteger(0);//_manuHourCutAngle);
		buffer.WriteInteger(0);//_manuHourCutBer);
		buffer.WriteInteger(0);//_manuHourCutRoot);
		buffer.WriteInteger(0);//_manuHourFillet);
		buffer.WriteInteger(0);//_manuHourIncise);
		buffer.WriteInteger(0);//_manuHourKaiHe);
		buffer.WriteInteger(0);//_manuHourPushFlat);
		buffer.WriteInteger(0);//_manuHourWeld);
		buffer.WriteInteger(0);//_manuHourZhiWan);
		buffer.WriteString("");//_notes);
		//buffer.Write(_finalUpdateDate);
		buffer.WriteBooleanThin(false);//_batchMakeHole);
		buffer.WriteInteger(1);//_partFileArr.Length);*/
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_PROCESSFILEDATA)>0)
	{
		char block[2048]={0};
		CBuffer ppibuf(block,2048);
		ppibuf.ClearContents();
		ppi.ToPPIBuffer(ppibuf);
		buffer.WriteDword(ppibuf.Length);
		buffer.Write(ppibuf.GetBufferPtr(),ppibuf.Length);
	}
}
void CServerTowerPart::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if( bIncludeId&&(
		dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_LABELnID)>0))
		_id = buffer.ReadInteger();
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_LABELnID)>0)
	{
		CXhChar16 label16;
		buffer.ReadString(label16,17);
		ppi.SetPartNo(label16);
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_BASICNONEID)>0)
	{
		//buffer.WriteInteger(m_idTowerType);
		buffer.ReadInteger(&m_uiCurrPpiVerSerial);
		buffer.ReadInteger(&m_uiCurrTidVerSerial);
		char cMaterial='S';
		BYTE ciPartType=0;
		buffer.ReadByte(&ciPartType);	//构件类型
		ppi.m_cPartType=ciPartType;
		//buffer.WriteInteger(0);//_materialTypeId);
		buffer.ReadByte(&cMaterial);
		ppi.cMaterial=cMaterial;
		//buffer.WriteString("");//_materialMark);
		buffer.Offset(4);//_num);
		DWORD dwSegI=0;
		buffer.ReadDword(&dwSegI);
		ppi.iSeg=dwSegI;
		double width,thick,weight;
		buffer.ReadDouble(&width);
		ppi.m_fWidth=(float)width;
		buffer.ReadDouble(&thick);
		ppi.m_fThick=(float)thick;
		WORD wiLength=0;
		buffer.ReadWord(&wiLength);
		ppi.m_wLength=wiLength;
		CXhChar50 sizestr;
		buffer.ReadString(sizestr,51);//_specification);
		ppi.SetSpec(sizestr);
		buffer.ReadDouble(&weight);
		ppi.m_fWeight=(float)weight;;//_weight);
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_PROCESSDATA)>0)
	{
		/*_towerTypeId = br.ReadInteger();
		_materialTypeId = br.ReadInteger();
		_currTidVerSerial = br.ReadInteger();
		_materialBriefMark = br.ReadInteger();
		_materialMark = br.ReadString();
		_num = br.ReadInteger();
		_segI = br.ReadInteger();
		_wide = br.ReadDouble();
		_thick = br.ReadDouble();
		_length = br.ReadDouble();
		_specification = br.ReadString();
		_weight = br.ReadDouble();
		_realWeight = br.ReadDouble();
		_minWeight = br.ReadDouble();
		_realArea = br.ReadDouble();
		_minArea = br.ReadDouble();
		_weldingLineVolume = br.ReadDouble();
		_wingAngle = br.ReadDouble();
		_m16LsNum = br.ReadInteger();
		_m20LsNum = br.ReadInteger();
		_m24LsNum = br.ReadInteger();
		_mxLsNum = br.ReadInteger();
		this._manuHourBore = br.ReadInteger();
		this._manuHourClashHole = br.ReadInteger();
		this._manuHourCutAngle = br.ReadInteger();
		this._manuHourCutBer = br.ReadInteger();
		this._manuHourCutRoot = br.ReadInteger();
		this._manuHourFillet = br.ReadInteger();
		this._manuHourIncise = br.ReadInteger();
		this._manuHourKaiHe = br.ReadInteger();
		this._manuHourPushFlat = br.ReadInteger();
		this._manuHourWeld = br.ReadInteger();
		this._manuHourZhiWan = br.ReadInteger();
		_notes = br.ReadString();
		_finalUpdateDate = br.ReadDate();
		_batchMakeHole = br.ReadBoolean();
		int length = br.ReadInteger();
		if(length >0)
		{
			_partLsArr = new PartLs[length];
			for(int i=0; i<length; i++)
			{
				_partLsArr[i] = new PartLs();
				_partLsArr[i].FromByteArr(br,updateFlag);
			}
		}
		else
			_partLsArr = null;
		length = br.ReadInteger();
		if(length>0)
		{
			_partFileArr = new PartFile[length];
			for(int i=0;i<length;i++)
			{
				_partFileArr[i] = new PartFile();
				_partFileArr[i].FromByteArr(br,updateFlag);
				if (_partFileArr[i].fileId == 3)
					relaTidPartFileId = _partFileArr[i].;
			}
		}
		else
			_partFileArr = null;*/
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerPart::SERIALIZE_PROCESSFILEDATA)>0)
	{
		if(ppifiledata!=NULL)
			delete []ppifiledata;
		buffer.ReadInteger(&_uiPpiFileLength);
		ppifiledata =new char[_uiPpiFileLength];
		buffer.Read(ppifiledata,_uiPpiFileLength);
	}
}
bool CServerTowerPart::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()==0)
		return false;
	ppi.SetKey(_id=atoi(attribstr));
	attribstr=xml.GetAttrib("idPpiFileObj");
	if(attribstr.GetLength()>0)
		m_idSrvFileObj=atoi(attribstr);
	else
		m_idSrvFileObj=0;
	attribstr=xml.GetAttrib("currPpiVerSerial");
	if(attribstr.GetLength()>0)
		m_uiCurrPpiVerSerial=atoi(attribstr);
	else
		m_uiCurrPpiVerSerial=0;
	attribstr=xml.GetAttrib("currTidVerSerial");
	if(attribstr.GetLength()>0)
		m_uiCurrTidVerSerial=atoi(attribstr);
	else
		m_uiCurrTidVerSerial=0;
	return true;
}
CServerFile::CServerFile()
{
	m_sServerClassName="TowerTypeFile";
	_id=0;
	m_idSrvFileObj=dwVersion=0;
	m_bHasPhysicalFile=false;
}
void CServerFile::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{ 
	if(bIncludeId)
		buffer.WriteInteger(_id);
	//buffer.WriteString("TmaFile");
	buffer.WriteStringToUTF8(m_sFileName);	//文件在客户端的原始名称
	buffer.WriteStringToUTF8(m_sDescription);
	buffer.WriteInteger(dwVersion);
	buffer.WriteInteger(m_idSrvFileObj);			//文件在文件服务器存储的实际物理文件Id;
	buffer.WriteBooleanThin(m_bHasPhysicalFile);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_FILEDATA)>0)
	{
		int len=m_dataBuff.GetLength();
		if(len>0)
		{
			buffer.WriteInteger(len);
			buffer.Write(m_dataBuff.GetBufferPtr(),len);
		}
		else
			buffer.WriteInteger(len);
	}
	else 
		buffer.WriteInteger((int)-1);
}
void CServerFile::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if (bIncludeId)
		buffer.ReadInteger(&_id);
	buffer.ReadStringFromUTF8(m_sFileName);
	buffer.ReadStringFromUTF8(m_sDescription);
	buffer.ReadInteger(&dwVersion);
	buffer.ReadInteger(&m_idSrvFileObj);
	buffer.ReadBooleanThin(&m_bHasPhysicalFile);
	int len=0;
	buffer.ReadInteger(&len);
	m_dataBuff.ClearBuffer();
	if(len>0)
	{
		m_dataBuff.Write(buffer.GetCursorBuffer(),len);
		buffer.SeekPosition(buffer.GetCursorPosition()+len);
	}
}
bool CServerFile::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()==0)
		return false;
	_id=atoi(attribstr);
	attribstr=xml.GetAttrib("fileObjId");
	if(attribstr.GetLength()==0)
		return false;
	this->m_idSrvFileObj=atoi(attribstr);
	return true;
}
//////////////////////////////////////////////////////////////////////////
// CTidFile
CTidFile::CTidFile()
{
	m_idTowerType=m_idSrvFileObj=_id=0;
	m_bHasPhysicalFile=false;
	m_sServerClassName="TidFile";
}
void CTidFile::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	buffer.WriteInteger(m_idTowerType);
	DWORD serialzeFileFlag=CServerFile::SERIALIZE_BASIC;
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CTidFile::SERIALIZE_FILEDATA)>0)
		serialzeFileFlag|=CServerFile::SERIALIZE_FILEDATA;
	CServerFile::ToBuffer(buffer,bIncludeId,serialzeFileFlag);
}
void CTidFile::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	buffer.ReadInteger(&m_idTowerType);
	CServerFile::FromBuffer(buffer,bIncludeId);
}
//////////////////////////////////////////////////////////////////////////
// CTapShellFile
CTapShellFile::CTapShellFile()
{
	m_idTowerType=m_idSrvFileObj=_id=0;
	m_bHasPhysicalFile=false;
	m_sServerClassName="TapShellFile";
}
void CTapShellFile::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	DWORD serialzeFileFlag=CServerFile::SERIALIZE_BASIC;
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CTapShellFile::SERIALIZE_FILEDATA)>0)
		serialzeFileFlag|=CServerFile::SERIALIZE_FILEDATA;
	CServerFile::ToBuffer(buffer,bIncludeId,serialzeFileFlag);
}
void CTapShellFile::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	CServerFile::FromBuffer(buffer,bIncludeId);
}
//////////////////////////////////////////////////////////////////////////
// CTapModel
IMPLEMENT_GET(CTapModel,pSrvTowerType)
{
	return m_pMapSrvTowerType;
}
CTapModel::CTapModel(CServerTowerType* pMapSrvTowerType)
{
	_id=pMapSrvTowerType!=NULL?pMapSrvTowerType->Id:0;
	 m_pMapSrvTowerType=pMapSrvTowerType;
	 m_sServerClassName="TapModel";
}
void CTapModel::ToBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.WriteInteger(Id);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_TIDFILE))
	{
		DWORD serializeFile=CServerFile::SERIALIZE_BASIC;
		if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_TIDFILE_DATA)>0)
			serializeFile|=CServerFile::SERIALIZE_FILEDATA;
		tidfile.ToBuffer(buffer,true,serializeFile);
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_TAPSHELLFILE))
	{
		DWORD serializeFile=CServerFile::SERIALIZE_BASIC;
		if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_TAPSHELLFILE_DATA)>0)
			serializeFile|=CServerFile::SERIALIZE_FILEDATA;
		shellfile.m_idTowerType=m_pMapSrvTowerType!=NULL?m_pMapSrvTowerType->Id:0;
		shellfile.ToBuffer(buffer,true,serializeFile);
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_ASSEMBLYHEIGHT))
	{
		if(m_pMapSrvTowerType==NULL)
			buffer.WriteInteger(0);
		else
		{
			DWORD serializeFile=CAssemblyHeight::SERIALIZE_BASIC;
			if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_ASSEMBLYHEIGHT_DATA)>0)
				serializeFile|=CAssemblyHeight::SERIALIZE_INFOFILEDATA;
			BUFFERPOP stack(&buffer,m_pMapSrvTowerType->listAssemblyHeights.Count);
			buffer.WriteInteger(m_pMapSrvTowerType->listAssemblyHeights.Count);
			for(CAssemblyHeight *pAssembly=m_pMapSrvTowerType->listAssemblyHeights.EnumObjectFirst();pAssembly;
				pAssembly=m_pMapSrvTowerType->listAssemblyHeights.EnumObjectNext())
			{
				pAssembly->ToBuffer(buffer,true,serializeFile);
				stack.Increment();
			}
			if(!stack.VerifyAndRestore())
				logerr.Log("呼高数量出现错误！");
		}
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_TOWERFILES))
	{
		if(m_pMapSrvTowerType==NULL)
			buffer.WriteInteger(0);
		else
		{
			buffer.WriteInteger(m_pMapSrvTowerType->m_towerFileGroupList.GetNodeNum());
			for(CServerTowerFileGroup *pFileGroup=m_pMapSrvTowerType->m_towerFileGroupList.GetFirst();pFileGroup;pFileGroup=m_pMapSrvTowerType->m_towerFileGroupList.GetNext())
				pFileGroup->ToBuffer(buffer,true,CServerTowerFileGroup::SERIALIZE_TAP);
		}
	}
}
void CTapModel::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
}
BOOL CTapModel::ParseSaveOrUpdateServerRetStr(char *sResult)
{
	return false;
}
bool CTapModel::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()==0)
		return false;
	this->_id=atoi(attribstr);
	attribstr=xml.GetAttrib("tidFileId");
	if(attribstr.GetLength()>0)
		tidfile.SetKey(atoi(attribstr));
	else
		tidfile.SetKey(0);
	attribstr=xml.GetAttrib("tidFileObjId");
	tidfile.m_idSrvFileObj=attribstr.GetLength()>0?atoi(attribstr):0;
	//attribstr=xml.GetAttrib("tapFileObjId");
	attribstr=xml.GetAttrib("tapShellFileObjId");
	if(attribstr.GetLength()>0)
		shellfile.m_idSrvFileObj=atoi(attribstr);
	else
		shellfile.m_idSrvFileObj=0;
	/*attribstr=xml.GetAttrib("currTidVerSerial");
	if(attribstr.GetLength()>0)
		m_uiCurrTidVerSerial=atoi(attribstr);
	else
		m_uiCurrTidVerSerial=0;*/
	if(m_pMapSrvTowerType==NULL||m_pMapSrvTowerType->listAssemblyHeights.Count==0)
		return true;
	CAssemblyHeight *pHeight=m_pMapSrvTowerType->listAssemblyHeights.EnumObjectFirst();

	CXhChar100 objname="AssemblyHeight";//pHeight->GetServerClassName();
	CXhChar100 objsetname = "AssemblyHeightSet";
	while(pHeight!=NULL&&xml.FindChildElem())   
	{
		xml.IntoElem();
		CString elemname=xml.GetTagName();
		if(elemname.Compare(objsetname)==0)
		{
			while(pHeight!=NULL&&xml.FindChildElem())   
			{
				xml.IntoElem();
				pHeight->ExtractFromXmlStr(xml);
				//if(!pHeight->ExtractFromXmlStr(xml))
				//	logerr.Log("");
				pHeight=m_pMapSrvTowerType->listAssemblyHeights.EnumObjectNext();
				xml.OutOfElem();
			}
		}
		else if(elemname.Compare(objname)==0)
		{
			pHeight->ExtractFromXmlStr(xml);
			pHeight=m_pMapSrvTowerType->listAssemblyHeights.EnumObjectNext();
		}
		xml.OutOfElem();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
// CTowerFile
DefCreateLocalTowerFileFunc CServerTowerFile::CreateLocalTowerFileFunc=NULL;
DefDestroyLocalTowerFileFunc CServerTowerFile::DestroyLocalTowerFileFunc=NULL;
DefCreateLocalFileGroupFunc CServerTowerFileGroup::CreateLocalFileGroupFunc=NULL;
DefDestroyLocalFileGroupFunc CServerTowerFileGroup::DestroyLocalFileGroupFunc=NULL;
CServerTowerFile::CServerTowerFile()
{
	m_idTowerType=0;
	m_idFileGroup=0;
	m_dataBuff.ClearBuffer();
	m_pLocalTowerFile=NULL;
	m_pBelongFileGroup=NULL;
	m_sServerClassName="TowerFile";
}
CServerTowerFile::~CServerTowerFile()
{
	if(DestroyLocalTowerFileFunc)
		DestroyLocalTowerFileFunc(m_pLocalTowerFile);
}
void CServerTowerFile::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	CServerFile::ToBuffer(buffer,bIncludeId);
	buffer.WriteInteger(m_idTowerType);
	buffer.WriteInteger(m_idFileGroup);
	buffer.WriteStringToUTF8("TowerTypeFile");	//文件类型
}
void CServerTowerFile::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	CServerFile::FromBuffer(buffer,bIncludeId);
	buffer.ReadInteger(&m_idTowerType);
	buffer.ReadInteger(&m_idFileGroup);
	CXhChar50 sFileType;
	buffer.ReadStringFromUTF8(sFileType,51);
}
void* CServerTowerFile::GetLocalTowerFile()
{
	if(m_pLocalTowerFile==NULL)
		m_pLocalTowerFile=CreateLocalTowerFileFunc(this);
	return m_pLocalTowerFile;
}
void* CServerTowerFile::BelongModel()
{
	if(m_pBelongFileGroup==NULL)
		return NULL;
	if(m_pBelongFileGroup->m_pBelongTowerType==NULL)
		return NULL;
	return m_pBelongFileGroup->m_pBelongTowerType->BelongModel();
}

//////////////////////////////////////////////////////////////////////////
// CServerTowerFileGroup
IMPLEMENT_GET(CServerTowerFileGroup ,pActiveFile)
{
	if(m_pActiveFile==NULL || m_pActiveFile->Id!=m_idActiveFile)
		return m_pActiveFile=hashTowerFiles.GetValue(m_idActiveFile);
	else
		return m_pActiveFile;
}
CServerTowerFileGroup::CServerTowerFileGroup()
{
	_id=m_localIdInTapModel=0;
	m_idTowerType=0;
	m_pActiveFile=NULL;
	m_pBelongTowerType=NULL;
	m_pLocalFileGroup=NULL;
	m_idActiveFile=m_uiActFileVerSerial=0;
	m_sServerClassName="TowerTypeFileGroup";
}
CServerTowerFileGroup::~CServerTowerFileGroup()
{
	if(DestroyLocalFileGroupFunc)
		DestroyLocalFileGroupFunc(m_pLocalFileGroup);
}
void CServerTowerFileGroup::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.WriteInteger(Id);
	buffer.WriteStringToUTF8(m_sName);
	buffer.WriteStringToUTF8(m_sDescription);
	buffer.WriteStringToUTF8(m_sSegStr);
	buffer.WriteStringToUTF8(shareSegIncStr);
	buffer.WriteStringToUTF8(priorPartLabelStr);
	buffer.WriteInteger(m_idTowerType);
	buffer.WriteInteger(m_idActiveFile);
	buffer.WriteInteger(m_uiActFileVerSerial);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&OPER_LOCALTAPID)>0)
		buffer.WriteInteger(m_localIdInTapModel);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&OPER_USER)>0)
		buffer.WriteInteger(0);	//_operUserId
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_ATTACHFILES)>0)
	{
		buffer.WriteInteger(hashTowerFiles.GetNodeNum());
		for(CServerTowerFile *pFile=hashTowerFiles.GetFirst();pFile;pFile=hashTowerFiles.GetNext())
			pFile->ToBuffer(buffer);
	}
}
void CServerTowerFileGroup::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.ReadInteger(&_id);
	buffer.ReadStringFromUTF8(m_sName);
	buffer.ReadStringFromUTF8(m_sDescription);
	buffer.ReadStringFromUTF8(m_sSegStr);
	buffer.ReadStringFromUTF8(shareSegIncStr);
	buffer.ReadStringFromUTF8(priorPartLabelStr);
	buffer.ReadInteger(&m_idTowerType);
	buffer.ReadInteger(&m_idActiveFile);
	buffer.ReadInteger(&m_uiActFileVerSerial);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&OPER_LOCALTAPID)>0)
		buffer.ReadInteger(&m_localIdInTapModel);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&OPER_USER)>0)
		buffer.Offset(4);	//_operUserId
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_ATTACHFILES)>0)
	{
		int id=0,nFileCount=0;
		buffer.ReadInteger(&nFileCount);
		for(int i=0;i<nFileCount;i++)
		{
			buffer.ReadInteger(&id);
			CServerTowerFile *pFile=hashTowerFiles.Add(id);
			pFile->m_idTowerType=m_idTowerType;
			pFile->FromBuffer(buffer,false);
			pFile->m_pBelongFileGroup=this;
			if(pFile->Id==m_idActiveFile)
				m_pActiveFile=pFile;
		}
	}
}
void CServerTowerFileGroup::ToBufferForPrjTowerType(CBuffer &buffer)
{
	buffer.WriteInteger(Id);
	buffer.WriteStringToUTF8(m_sName);
	buffer.WriteStringToUTF8(m_sDescription);
	buffer.WriteStringToUTF8(m_sSegStr);
	CServerTowerFile* pFile=pActiveFile;
	if(pFile)
	{
		buffer.WriteInteger(m_idActiveFile);
		buffer.WriteStringToUTF8(pFile->m_sFileName);
		buffer.WriteStringToUTF8(pFile->m_sDescription);
		buffer.WriteInteger(pFile->dwVersion);
		buffer.WriteInteger(pFile->m_idSrvFileObj);
		//buffer.WriteBoolean(pFile->m_bHasPhysicalFile);
		if(pFile->m_dataBuff.GetLength()>0)
		{
			buffer.WriteInteger(pFile->m_dataBuff.GetLength());
			buffer.Write(pFile->m_dataBuff.GetBufferPtr(),pFile->m_dataBuff.GetLength());
		}
		else 
			buffer.WriteInteger((int)-1);
	}
	else 
		buffer.WriteInteger((int)-1);
}
void CServerTowerFileGroup::FromBufferForAssemblyTowerType(CBuffer &buffer)
{
	buffer.ReadInteger(&_id);
	buffer.ReadStringFromUTF8(m_sName);
	buffer.ReadStringFromUTF8(m_sDescription);
	buffer.ReadStringFromUTF8(m_sSegStr);
	buffer.ReadInteger(&m_idActiveFile);
	CServerTowerFile* pFile=pActiveFile;
	if(pFile)
	{
		buffer.ReadStringFromUTF8(pFile->m_sFileName);
		buffer.ReadStringFromUTF8(pFile->m_sDescription);
		buffer.ReadInteger(&pFile->dwVersion);
		buffer.ReadInteger(&pFile->m_idSrvFileObj);
		//buffer.ReadBoolean(&pFile->m_bHasPhysicalFile);
		if(pFile->Id==m_idActiveFile)
			m_pActiveFile=pFile;
		//
		pFile->m_dataBuff.ClearContents();
		int nFileMatterLen=buffer.ReadInteger();
		if(nFileMatterLen>0)
		{
			pFile->m_dataBuff.Write(buffer.GetCursorBuffer(),nFileMatterLen);
			buffer.SeekPosition(buffer.GetCursorPosition()+nFileMatterLen);
		}
		pFile->m_idFileGroup=Id;
		pFile->m_idTowerType=m_idTowerType;
		pFile->m_pBelongFileGroup=this;
	}
}
void* CServerTowerFileGroup::GetLocalFileGroup()
{
	if(m_pLocalFileGroup==NULL)
		m_pLocalFileGroup=CreateLocalFileGroupFunc(this);
	return m_pLocalFileGroup;
}
void* CServerTowerFileGroup::BelongModel()
{
	if(m_pBelongTowerType==NULL)
		return NULL;
	return m_pBelongTowerType->BelongModel();
}
//////////////////////////////////////////////////////////////////////////
// CServerTowerType
CServerTowerType::CServerTowerType()
{
	_id=m_idBelongProject=0;
	state=1;
	m_sServerClassName="TowerType";
}
bool CServerTowerType::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()==0)
		return false;
	_id=atoi(attribstr);
	attribstr=xml.GetAttrib("projectId");
	if(attribstr.GetLength()>0)
		this->m_idBelongProject=atoi(attribstr);
	return true;
}
void CServerTowerType::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.WriteInteger(Id);
	buffer.WriteInteger(state);
	buffer.WriteInteger(m_idBelongProject);
	buffer.WriteStringToUTF8(m_sName);
	buffer.WriteStringToUTF8(m_sProjName);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerType::SERIALIZE_FILEGROUP))
	{
		buffer.WriteInteger(m_towerFileGroupList.GetNodeNum());
		for(CServerTowerFileGroup *pFileGroup=m_towerFileGroupList.GetFirst();pFileGroup;pFileGroup=m_towerFileGroupList.GetNext())
			pFileGroup->ToBuffer(buffer);
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerType::SERIALIZE_ASSEMBLYHEIGHT))
	{
		int nHeightCount=0;
		BOOL pushed=buffer.PushPositionStack();
		buffer.WriteInteger(nHeightCount);//listAsemblyHeights.GetNodeNum());
		for(CAssemblyHeight *pAssembly=listAssemblyHeights.EnumObjectFirst();pAssembly;pAssembly=listAssemblyHeights.EnumObjectNext(),nHeightCount++)
			pAssembly->ToBuffer(buffer,true,CAssemblyHeight::SERIALIZE_BASIC);
		buffer.PopPositionStack(pushed);
		buffer.WriteInteger(nHeightCount);
		buffer.RestoreNowPosition();
	}
}
void CServerTowerType::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.ReadInteger(&_id);
	buffer.ReadInteger(&state);
	buffer.ReadInteger(&m_idBelongProject);
	buffer.ReadStringFromUTF8(m_sName);
	buffer.ReadStringFromUTF8(m_sProjName);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerType::SERIALIZE_FILEGROUP))
	{
		int nFileGroupCount=buffer.ReadInteger();
		m_towerFileGroupList.Empty();
		for(int i=0;i<nFileGroupCount;i++)
		{
			CServerTowerFileGroup *pFileGroup=m_towerFileGroupList.append();
			pFileGroup->m_idTowerType=_id;
			if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerType::SERIALIZE_ASSEMBLYHEIGHT))
				pFileGroup->FromBuffer(buffer,true,CServerTowerFileGroup::SERIALIZE_TAP);
			else
				pFileGroup->FromBuffer(buffer,true,CServerTowerFileGroup::SERIALIZE_TMA);
			pFileGroup->m_pBelongTowerType=this;
		}
	}
	if(dwSerialzeFlag==0||(dwSerialzeFlag&CServerTowerType::SERIALIZE_ASSEMBLYHEIGHT))
	{
		int nHeightCount=buffer.ReadInteger();
		listAssemblyHeights.DeleteList();
		for(int i=0;i<nHeightCount;i++)
		{
			CAssemblyHeight *pAssembly=listAssemblyHeights.AttachObject();
			pAssembly->m_pBelongTowerType=this;
			pAssembly->FromBuffer(buffer,true,CAssemblyHeight::SERIALIZE_BASIC);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// CAssemblyHeight
CAssemblyHeight::CAssemblyHeight()
{
	_id=localIdInTapModel=0;
	m_idTowerType=m_idTidFile=0;
	//m_pActiveFile=NULL;
	m_pBelongTowerType=NULL;
	m_pTowerInstance=NULL;
	//m_pLocalFileGroup=NULL;
	//m_idActiveFile=m_uiVerSerial=0;
	m_nManuCount=1;
	m_iFoundationStyle=0;
	m_sServerClassName="AssemblyHeight";
}
CAssemblyHeight::~CAssemblyHeight()
{
	//if(DestroyLocalFileGroupFunc)
	//	DestroyLocalFileGroupFunc(m_pLocalFileGroup);
}
void CAssemblyHeight::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.WriteInteger(_id);
	buffer.WriteInteger(m_idTowerType);
	buffer.WriteInteger(m_idTidFile);
	buffer.WriteStringToUTF8(placeName,placeName.GetLengthMax());
	buffer.WriteStringToUTF8(heightName,heightName.GetLengthMax());
	buffer.WriteStringToUTF8(segIncStr,segIncStr.GetLengthMax());
	buffer.WriteDouble(m_fAntiTheftHeight);
	buffer.WriteInteger(m_nManuCount);
	buffer.WriteInteger(m_iFoundationStyle);
	//关联物理数据文件的内容
	buffer.WriteStringToUTF8(m_sFileName);	//文件在客户端的原始名称
	buffer.WriteStringToUTF8(m_sDescription);
	//buffer.WriteInteger(dwVersion);
	buffer.WriteInteger(m_idSrvFileObj);			//文件在文件服务器存储的实际物理文件Id;
	buffer.WriteBooleanThin(m_bHasPhysicalFile);
	if(dwSerialzeFlag==0||(dwSerialzeFlag&SERIALIZE_INFOFILEDATA)>0)
	{
		int len=m_dataBuff.GetLength();
		if(len>0)
		{
			buffer.WriteInteger(len);
			buffer.Write(m_dataBuff.GetBufferPtr(),len);
		}
	}
	else 
		buffer.WriteInteger((int)-1);
}
void CAssemblyHeight::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		_id=buffer.ReadInteger();
	buffer.ReadInteger(&m_idTowerType);
	buffer.ReadInteger(&m_idTidFile);
	buffer.ReadStringFromUTF8(placeName,placeName.GetLengthMax());
	buffer.ReadStringFromUTF8(heightName,heightName.GetLengthMax());
	buffer.ReadStringFromUTF8(segIncStr,segIncStr.GetLengthMax());
	buffer.ReadDouble(&m_fAntiTheftHeight);
	buffer.ReadInteger(&m_nManuCount);
	buffer.ReadInteger(&m_iFoundationStyle);
	//关联物理数据文件的内容
	buffer.ReadStringFromUTF8(m_sFileName);	//文件在客户端的原始名称
	buffer.ReadStringFromUTF8(m_sDescription);
	//buffer.WriteInteger(dwVersion);
	buffer.ReadInteger(&m_idSrvFileObj);			//文件在文件服务器存储的实际物理文件Id;
	buffer.ReadBooleanThin(&m_bHasPhysicalFile);
	int len=0;
	m_dataBuff.ClearContents();
	buffer.ReadInteger(&len);
	if(len>0)
	{
		m_dataBuff.Write(buffer.GetCursorBuffer(),len);
		buffer.Offset(len);
	}
}

//////////////////////////////////////////////////////////////////////////
// CManuElemTask
//////////////////////////////////////////////////////////////////////////
CServerManuTask::CServerManuTask()
{
	_id=0;
	m_sServerClassName="ManuElemTask";
}
void CServerManuTask::ToBuffer(CBuffer &buffer, bool bIncludeId/*=true*/, DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.WriteInteger(_id);
	buffer.WriteStringToUTF8(m_sName);
	buffer.WriteInteger(hashTowerTypes.GetNodeNum());
	for(CServerTowerType *pTowerType=hashTowerTypes.GetFirst();pTowerType;pTowerType=hashTowerTypes.GetNext())
	{
		buffer.WriteInteger(pTowerType->Id);
		buffer.WriteStringToUTF8(pTowerType->m_sName);
	}
}
void CServerManuTask::FromBuffer(CBuffer &buffer,bool bIncludeId/*=true*/,DWORD dwSerialzeFlag/*=0*/)
{
	if(bIncludeId)
		buffer.ReadInteger(&_id);
	buffer.ReadStringFromUTF8(m_sName);
	int nTowerTypeCount=buffer.ReadInteger();
	for(int i=0;i<nTowerTypeCount;i++)
	{
		int towerTypeId=buffer.ReadInteger();
		CServerTowerType *pTowerType=hashTowerTypes.Add(towerTypeId);
		buffer.ReadStringFromUTF8(pTowerType->m_sName);
	}
}
//////////////////////////////////////////////////////////////////////////
// 解析保存函数返回值
// 结果字符串格式，例：123:45#32|33#44|(CTowerFileGroup.m_id:[CServerTowerFile.m_id#CServerTowerFile.m_fileId])
BOOL CServerTowerFileGroup::ParseSaveOrUpdateServerRetStr(char *sResult)
{
	if(sResult==NULL)
		return FALSE;
	char sTemp[100]={0};
	strcpy(sTemp,sResult);
	char *key=strchr(sTemp,':');
	if(key!=NULL)
	{
		*key=0;
		if(Id<=0)
			_id=atoi(sTemp);
		if(hashTowerFiles.GetNodeNum()>0)
		{
			char *sFileIdArr=key+1;
			int i=0;
			DYN_ARRAY<CXhChar50> fileRetKeyArr(hashTowerFiles.GetNodeNum());
			for(char* key2=strtok(sFileIdArr,"|");key2;key2=strtok(NULL,"|"),i++)
			{
				if(i>(int)fileRetKeyArr.Size())
				{
					logerr.Log("保存塔型文件组时，塔型文件id个数与塔型文件个数不匹配！");
					return FALSE;
				}
				fileRetKeyArr[i].Copy(key2);
			}
			i=0;
			char sFileKey[50]={0};
			for(CServerTowerFile *pFile=hashTowerFiles.GetFirst();pFile;pFile=hashTowerFiles.GetNext(),i++)
			{
				strcpy(sFileKey,(char*)fileRetKeyArr[i]);
				key=strchr(sTemp,'#');
				if(key!=NULL)
				{
					*key=0;
					if(pFile->Id<=0)
						pFile->_id=atoi(sTemp);
					char *sFileId=key+1;
					if(pFile->m_idSrvFileObj<=0)
						pFile->m_idSrvFileObj=atoi(sFileId);
				}
			}
		}
		return TRUE;
	}
	else 
		return FALSE;
}
bool CServerTowerFileGroup::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()==0)
		return false;
	_id=atoi(attribstr);

	attribstr=xml.GetAttrib("activateFileId");
	if(attribstr.GetLength()!=0)
	{
		m_idActiveFile=atoi(attribstr);
		attribstr=xml.GetAttrib("activeFileVerSerial");
		if(attribstr.GetLength()>0)
			m_uiActFileVerSerial=atoi(attribstr);
	}

	CServerTowerFile *pFile=hashTowerFiles.GetFirst();
	while(pFile!=NULL&&xml.FindChildElem("TowerFile"))
	{
		xml.IntoElem();
		int idLocalFile=0;
		attribstr=xml.GetAttrib("id");
		if(attribstr.GetLength()>0)
			pFile->_id=atoi(attribstr);
		attribstr=xml.GetAttrib("idFile");
		if(attribstr.GetLength()>0)
			pFile->m_idSrvFileObj=atoi(attribstr);
		pFile->m_idFileGroup=_id;
		xml.OutOfElem();
		pFile=hashTowerFiles.GetNext();
	}
	return true;
}
//结果字符串格式，例：123:45(CServerTowerFile.m_id:CServerTowerFile.m_fileId)
BOOL CServerTowerFile::ParseSaveOrUpdateServerRetStr(char *sResult)
{
	char sTemp[100]={0};
	strcpy(sTemp,sResult);
	char *key=strchr(sTemp,':');
	if(key!=NULL)
	{
		*key=0;
		if(Id<=0)
			_id=atoi(sTemp);
		char *sFileId=key+1;
		if(m_idSrvFileObj<=0)
			m_idSrvFileObj=atoi(sFileId);
		return TRUE;
	}
	else
		return FALSE;
}
bool CServerTowerFile::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()<=0)
		return false;
	_id=atoi(attribstr);
	attribstr=xml.GetAttrib("fileId");
	if(attribstr.GetLength()>0)
		m_idSrvFileObj=atoi(attribstr);
	return true;
}
CServerContents AgentServer;
CServerContents::CServerContents()
{
	m_pActiveTowerFile=NULL;
}
void CServerContents::ParseAndUpdateTowerTypesFromBuffer(CBuffer &buffer,int iStateTowerType/*=0*/,DWORD dwSerialzeFlag/*=0*/)
{
	CServerTowerType* pServTowerType=NULL;
	CHashSet<CServerTowerType*> hashLegacyStateTowerTypes;
	if(iStateTowerType>0)
	{
		for(pServTowerType=hashTowerTypes.GetFirst();pServTowerType;pServTowerType=hashTowerTypes.GetNext())
		{
			if(pServTowerType->state!=iStateTowerType)
				continue;
			hashLegacyStateTowerTypes.SetValue(pServTowerType->Id,pServTowerType);
		}
	}
	long i=0,nCount=buffer.ReadInteger();
	for(i=0;i<nCount;i++)
	{
		long id=buffer.ReadInteger();
		pServTowerType=hashTowerTypes.Add(id);
		pServTowerType->FromBuffer(buffer,false,dwSerialzeFlag);
		if(hashLegacyStateTowerTypes.GetValue(id))
			hashLegacyStateTowerTypes.DeleteNode(id);
	}
	for(pServTowerType=hashLegacyStateTowerTypes.GetFirst();pServTowerType;pServTowerType=hashLegacyStateTowerTypes.GetNext())
		hashTowerTypes.DeleteNode(pServTowerType->Id);
}
void CServerContents::ParseProcessPartsFromBuffer(CBuffer &buffer,CHashListEx<CProcessPart>* pHashProcessParts)
{
	long i=0,nCount=buffer.ReadInteger();
	for(i=0;i<nCount;i++)
	{
		long id=buffer.ReadInteger();
		CProcessPart* pProcessPart=pHashProcessParts->Add(id);
		pProcessPart->FromPPIBuffer(buffer);
	}
}
void CServerContents::ParseManuTasksFromBuffer(CBuffer &buffer,CHashListEx<CServerManuTask>* pHashTasks)
{
	if(pHashTasks==NULL)
		return;
	pHashTasks->Empty();
	long i=0,nCount=buffer.ReadInteger();
	for(i=0;i<nCount;i++)
	{
		long id=buffer.ReadInteger();
		CServerManuTask* pTask=pHashTasks->Add(id);
		pTask->FromBuffer(buffer,false,0);
	}
}
CServerTowerFile* CServerContents::GetActiveTowerFile()
{
	return m_pActiveTowerFile;//.GetActiveObj();
}
void CServerContents::ActivateTowerFile(CServerTowerFile *pTowerFile)
{
	m_pActiveTowerFile=pTowerFile;
	CServerTowerType *pTowerType=hashTowerTypes.GetValue(pTowerFile->m_idTowerType);
	ActivateTowerType(pTowerType);
	//m_serverContent.ActivateObj(pTowerFile);
}
CServerTowerType* CServerContents::GetActiveTowerType()
{
	return m_pActiveTowerType;//.GetActiveObj();
}
void CServerContents::ActivateTowerType(CServerTowerType *pTowerType)
{
	m_pActiveTowerType=pTowerType;
}
