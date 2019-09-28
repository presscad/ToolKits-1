#include "StdAfx.h"
#include "LayerTable.h"
#include "PathManager.h"

bool load_linetype(char *LineType)
{
	AcDbLinetypeTable *pLinetypeTbl;//定义线型表指针
	AcDbObjectId LineTypeId;//定义线型的ID号
	//以读方式打开线型表，并获得线型表指针
	char line_path[MAX_PATH], sys_path[MAX_PATH];
	GetSysPath(sys_path);
	strcpy(line_path, sys_path);
	strcat(line_path, "Tower.lin");
	//如果线型表中已有当前线型用getAt()获得其ID号，否则装入
	acdbHostApplicationServices()->workingDatabase()->getLinetypeTable(pLinetypeTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if ((pLinetypeTbl->getAt((ACHAR*)_bstr_t(LineType), LineTypeId)) != Acad::eOk)
#else
	if ((pLinetypeTbl->getAt(LineType, LineTypeId)) != Acad::eOk)
#endif
	{
		pLinetypeTbl->close();//关闭线型表
#ifdef _ARX_2007
		Acad::ErrorStatus error = acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile((ACHAR*)_bstr_t(LineType), (ACHAR*)_bstr_t(line_path));
#else
		Acad::ErrorStatus error = acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile(LineType, line_path);
#endif

		if (error == Acad::eOk)
			return true;
#ifdef _ARX_2007
		else if (acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile((ACHAR*)_bstr_t(LineType), L"acad.lin") == Acad::eOk)
			return true;
		else if (acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile((ACHAR*)_bstr_t(LineType), L"acadiso.lin") == Acad::eOk)
			return true;
#else
		else if (acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile(LineType, "acad.lin") == Acad::eOk)
			return true;
		else if (acdbHostApplicationServices()->workingDatabase()->loadLineTypeFile(LineType, "acadiso.lin") == Acad::eOk)
			return true;
#endif
		else
			return false;
	}
	else
		pLinetypeTbl->close();	//关闭线型表
	return true;
}


bool CreateNewLayer(char *newlayer, char* line_type, AcDb::LineWeight line_thick,
	int color_i, AcDbObjectId &layerId, AcDbObjectId &lineTypeId)
{
	char layer[MAX_PATH];
	if (newlayer == NULL)
		return false;
	else
	{
		strcpy(layer, newlayer);
		if (strlen(newlayer) < 1)
			return false;
	}
	AcDbLayerTable *pLayerTable;
	AcDbLayerTableRecord *pLayerTableRecord;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pLayerTable->getAt((ACHAR*)_bstr_t(layer), layerId) == Acad::eOk)
#else
	if (pLayerTable->getAt(layer, layerId) == Acad::eOk)
#endif
	{
		pLayerTable->close();
		acdbOpenObject(pLayerTableRecord, layerId, AcDb::kForWrite);
		pLayerTableRecord->setIsFrozen(0);// layer to THAWED
		pLayerTableRecord->close();
		//获取图层对应的线型Id
		AcDbLinetypeTable *pLinetypeTbl;
		acdbHostApplicationServices()->workingDatabase()->getLinetypeTable(pLinetypeTbl, AcDb::kForRead);
		if (pLinetypeTbl)
		{
#ifdef _ARX_2007
			pLinetypeTbl->getAt((ACHAR*)_bstr_t(line_type), lineTypeId);
#else
			pLinetypeTbl->getAt(line_type, lineTypeId);
#endif
			pLinetypeTbl->close();
		}
		return true;
	}

	pLayerTableRecord = new AcDbLayerTableRecord;
#ifdef _ARX_2007
	pLayerTableRecord->setName((ACHAR*)_bstr_t(layer));
#else
	pLayerTableRecord->setName(layer);
#endif
	pLayerTableRecord->setIsFrozen(0);// layer to THAWED
	pLayerTableRecord->setIsOff(0);   // layer to ON
	pLayerTableRecord->setVPDFLT(0);  // viewport default
	pLayerTableRecord->setIsLocked(0);// un-locked
	pLayerTableRecord->setLineWeight(line_thick);
	AcCmColor color;
	color.setColorIndex(color_i);
	pLayerTableRecord->setColor(color);
	pLayerTable->add(layerId, pLayerTableRecord);
	pLayerTableRecord->close();
	pLayerTable->close();
	load_linetype(line_type);
	ads_name ent_name;
	if (acdbGetAdsName(ent_name, layerId) == Acad::eOk)
	{
#ifndef _MAPTMA_GRX
		struct resbuf *pData = acdbEntGet(ent_name);
		acdbEntMod(pData);
#endif
		acdbEntUpd(ent_name);
	}

	AcDbLinetypeTable *pLinetypeTbl;
	acdbOpenObject(pLayerTableRecord, layerId, AcDb::kForWrite);
	acdbHostApplicationServices()->workingDatabase()->getLinetypeTable(pLinetypeTbl,
		AcDb::kForRead);
#ifdef _ARX_2007
	pLinetypeTbl->getAt((ACHAR*)_bstr_t(line_type), lineTypeId);
#else
	pLinetypeTbl->getAt(line_type, lineTypeId);
#endif

	pLayerTableRecord->setLinetypeObjectId(lineTypeId);
	pLinetypeTbl->close();
	pLayerTableRecord->close();
	return true;
}
void GetCurDwgLayers(ATOM_LIST<CXhChar50> &layer_list)
{
	layer_list.Empty();
	AcDbLayerTable *pLayerTable;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead);
	AcDbLayerTableIterator* pIterator = NULL;
	pLayerTable->newIterator(pIterator);
	pLayerTable->close();
	for (; !pIterator->done(); pIterator->step())
	{
		AcDbLayerTableRecord *pLayerTableRecord;
		pIterator->getRecord(pLayerTableRecord, AcDb::kForRead);
		if (pLayerTableRecord == NULL)
			continue;
		pLayerTableRecord->close();
		CXhChar50 layer;
#ifdef _ARX_2007
		ACHAR *sValue = new ACHAR[50];
		pLayerTableRecord->getName(sValue);
		layer.Copy((char*)_bstr_t(sValue));
		delete[] sValue;
#else
		char *sValue = new char[50];
		pLayerTableRecord->getName(sValue);
		layer.Copy(sValue);
		delete[] sValue;
#endif
		if (layer.GetLength() > 0)
			layer_list.append(layer);
	}
}

CHashStrList<TMADRAWING_LAYER> LayerTable::layerHashList;	//图层哈希表以图层名为键值
//图层的样式
TMADRAWING_LAYER LayerTable::VisibleProfileLayer;	//结构图中可见轮廓线图层
TMADRAWING_LAYER LayerTable::BriefLineLayer;		//受力材单线图
TMADRAWING_LAYER LayerTable::AuxLineLayer;			//辅材单线图
TMADRAWING_LAYER LayerTable::UnvisibleProfileLayer;	//结构图中不可见轮廓线图层
TMADRAWING_LAYER LayerTable::AxisLineLayer;			//中心线、对称线、定位轴线
TMADRAWING_LAYER LayerTable::SonJgAxisLineLayer;	//子角钢心线图层
TMADRAWING_LAYER LayerTable::BreakLineLayer;		//断开界线
TMADRAWING_LAYER LayerTable::BendLineLayer;			//角钢火曲、钢板火曲
TMADRAWING_LAYER LayerTable::AssistHuoQuLineLayer;	//辅助火曲线
TMADRAWING_LAYER LayerTable::SimPolyLayer;			//相似形标注
TMADRAWING_LAYER LayerTable::BoltSymbolLayer;		//螺栓图符
TMADRAWING_LAYER LayerTable::BoltLineLayer;			//螺栓线
TMADRAWING_LAYER LayerTable::DimSizeLayer;			//尺寸标注 wht 11-07-04
TMADRAWING_LAYER LayerTable::DimTextLayer;			//文字标注
TMADRAWING_LAYER LayerTable::DamagedSymbolLine;		//板边破损线
TMADRAWING_LAYER LayerTable::CommonLayer;			//除以上以外的所有图元所在图层
TOWERATOM_COLOR_INDEX LayerTable::ColorIndex;
void TMADRAWING_LAYER::CopyProperty(TMADRAWING_LAYER *pLayer)
{
	if (pLayer)
	{
		layerName.Copy(pLayer->layerName);
		layerId = pLayer->layerId;
		colorIndex = pLayer->colorIndex;
		lineWeight = pLayer->lineWeight;
		lineTypeId = pLayer->lineTypeId;
		lineTypeName.Copy(pLayer->lineTypeName);
	}
	else
	{
		layerName.Copy("");
		layerId = 0;
		colorIndex = 0;
		lineWeight = AcDb::kLnWt000;
		lineTypeId = 0;
		lineTypeName.Copy("");
	}
}
//初始化绘图环境
void LayerTable::InitLayerTable()
{
	AcDbObjectId layerId;
	acdbHostApplicationServices()->workingDatabase()->setLtscale(20);
	//结构图中可见轮廓线图层
	LayerTable::VisibleProfileLayer.layerName.Copy("1");
	LayerTable::VisibleProfileLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::VisibleProfileLayer.lineWeight = AcDb::kLnWt025;
	LayerTable::VisibleProfileLayer.colorIndex = ColorIndex.crOutLine;//白色
	LayerTable::VisibleProfileLayer.layerId = 0;
	LayerTable::VisibleProfileLayer.lineTypeId = 0;
	//结构图中不可见轮廓线图层
	LayerTable::UnvisibleProfileLayer.layerName.Copy("2");
	LayerTable::UnvisibleProfileLayer.lineTypeName.Copy("HIDDEN");
	LayerTable::UnvisibleProfileLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::UnvisibleProfileLayer.colorIndex = ColorIndex.crDashLine;	//白色
	LayerTable::UnvisibleProfileLayer.layerId = 0;
	LayerTable::UnvisibleProfileLayer.lineTypeId = 0;
	//受力材单线图、总图
	LayerTable::BriefLineLayer.layerName.Copy("3");
	LayerTable::BriefLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BriefLineLayer.lineWeight = AcDb::kLnWt018;
	LayerTable::BriefLineLayer.colorIndex = ColorIndex.crOutLine;//白色
	LayerTable::BriefLineLayer.layerId = 0;
	LayerTable::BriefLineLayer.lineTypeId = 0;
	//辅材单线图
	LayerTable::AuxLineLayer.layerName.Copy("4");
	LayerTable::AuxLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::AuxLineLayer.lineWeight = AcDb::kLnWt018;
	LayerTable::AuxLineLayer.colorIndex = 4;	//青色
	LayerTable::AuxLineLayer.layerId = 0;
	LayerTable::AuxLineLayer.lineTypeId = 0;
	//螺栓图符
	LayerTable::BoltSymbolLayer.layerName.Copy("5");
	LayerTable::BoltSymbolLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BoltSymbolLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::BoltSymbolLayer.colorIndex = ColorIndex.crLs;
	LayerTable::BoltSymbolLayer.layerId = 0;
	LayerTable::BoltSymbolLayer.lineTypeId = 0;
	//相似形标注
	LayerTable::SimPolyLayer.layerName.Copy("6");
	LayerTable::SimPolyLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::SimPolyLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::SimPolyLayer.colorIndex = ColorIndex.crDimSimTri;
	LayerTable::SimPolyLayer.layerId = 0;
	LayerTable::SimPolyLayer.lineTypeId = 0;
	//中心线、对称线、定位轴线
	LayerTable::AxisLineLayer.layerName.Copy("7");
	LayerTable::AxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::AxisLineLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::AxisLineLayer.colorIndex = ColorIndex.crAxisLine;
	LayerTable::AxisLineLayer.layerId = 0;
	LayerTable::AxisLineLayer.lineTypeId = 0;
	//子角钢中心线
	LayerTable::SonJgAxisLineLayer.layerName.Copy("7A");
	LayerTable::SonJgAxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::SonJgAxisLineLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::SonJgAxisLineLayer.colorIndex = ColorIndex.crAxisLine;
	LayerTable::SonJgAxisLineLayer.layerId = 0;
	LayerTable::SonJgAxisLineLayer.lineTypeId = 0;
	//辅助火曲线
	LayerTable::AssistHuoQuLineLayer.layerName.Copy("8");
	LayerTable::AssistHuoQuLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::AssistHuoQuLineLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::AssistHuoQuLineLayer.colorIndex = ColorIndex.crHuoQuLine;
	LayerTable::AssistHuoQuLineLayer.layerId = 0;
	LayerTable::AssistHuoQuLineLayer.lineTypeId = 0;
	//角钢火曲、钢板火曲
	LayerTable::BendLineLayer.layerName.Copy("8");
	LayerTable::BendLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::BendLineLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::BendLineLayer.colorIndex = ColorIndex.crHuoQuLine;
	LayerTable::BendLineLayer.layerId = 0;
	LayerTable::BendLineLayer.lineTypeId = 0;
	//断开界线
	LayerTable::BreakLineLayer.layerName.Copy("9");
	LayerTable::BreakLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BreakLineLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::BreakLineLayer.colorIndex = ColorIndex.crDragLine;
	LayerTable::BreakLineLayer.layerId = 0;
	LayerTable::BreakLineLayer.lineTypeId = 0;
	//螺栓线
	LayerTable::BoltLineLayer.layerName.Copy("10");
	LayerTable::BoltLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::BoltLineLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::BoltLineLayer.colorIndex = ColorIndex.crDimSimTri;
	LayerTable::BoltLineLayer.layerId = 0;
	LayerTable::BoltLineLayer.lineTypeId = 0;
	//尺寸标注
	LayerTable::DimSizeLayer.layerName.Copy("11");
	LayerTable::DimSizeLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::DimSizeLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::DimSizeLayer.colorIndex = ColorIndex.crHuoQuLine;
	LayerTable::DimSizeLayer.layerId = 0;
	LayerTable::DimSizeLayer.lineTypeId = 0;
	//文字标注
	LayerTable::DimTextLayer.layerName.Copy("12");
	LayerTable::DimTextLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::DimTextLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::DimTextLayer.colorIndex = ColorIndex.crOutLine;
	LayerTable::DimTextLayer.layerId = 0;
	LayerTable::DimTextLayer.lineTypeId = 0;
	//除以上以外的所有图元所在图层
	LayerTable::CommonLayer.layerName.Copy("13");
	LayerTable::CommonLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::CommonLayer.lineWeight = AcDb::kLnWt013;
	LayerTable::CommonLayer.colorIndex = ColorIndex.crDragLine;
	LayerTable::CommonLayer.layerId = 0;
	LayerTable::CommonLayer.lineTypeId = 0;
	//板边破损标记线
	LayerTable::DamagedSymbolLine.layerName.Copy("14");
	LayerTable::DamagedSymbolLine.lineTypeName.Copy("ZIGZAG");
	LayerTable::DamagedSymbolLine.lineWeight = AcDb::kLnWt013;
	LayerTable::DamagedSymbolLine.colorIndex = ColorIndex.crDragLine;
	LayerTable::DamagedSymbolLine.layerId = 0;
	LayerTable::DamagedSymbolLine.lineTypeId = 0;
	if (LayerTable::layerHashList.GetNodeNum() == 0)
	{	//因之前增加了破损线，所以可能至少为1个 wjh-2016.6.19
		CreateNewLayer(LayerTable::VisibleProfileLayer.layerName, LayerTable::VisibleProfileLayer.lineTypeName,
			LayerTable::VisibleProfileLayer.lineWeight, LayerTable::VisibleProfileLayer.colorIndex,
			LayerTable::VisibleProfileLayer.layerId, LayerTable::VisibleProfileLayer.lineTypeId);	//1
		CreateNewLayer(LayerTable::UnvisibleProfileLayer.layerName, LayerTable::UnvisibleProfileLayer.lineTypeName,
			LayerTable::UnvisibleProfileLayer.lineWeight, LayerTable::UnvisibleProfileLayer.colorIndex,
			LayerTable::UnvisibleProfileLayer.layerId, LayerTable::UnvisibleProfileLayer.lineTypeId);//2
		CreateNewLayer(LayerTable::BriefLineLayer.layerName, LayerTable::BriefLineLayer.lineTypeName,
			LayerTable::BriefLineLayer.lineWeight, LayerTable::BriefLineLayer.colorIndex,
			LayerTable::BriefLineLayer.layerId, LayerTable::BriefLineLayer.lineTypeId);				//3	
		CreateNewLayer(LayerTable::AuxLineLayer.layerName, LayerTable::AuxLineLayer.lineTypeName,
			LayerTable::AuxLineLayer.lineWeight, LayerTable::AuxLineLayer.colorIndex,
			LayerTable::AuxLineLayer.layerId, LayerTable::AuxLineLayer.lineTypeId);					//4
		CreateNewLayer(LayerTable::BoltSymbolLayer.layerName, LayerTable::BoltSymbolLayer.lineTypeName,
			LayerTable::BoltSymbolLayer.lineWeight, LayerTable::BoltSymbolLayer.colorIndex,
			LayerTable::BoltSymbolLayer.layerId, LayerTable::BoltSymbolLayer.lineTypeId);			//5
		CreateNewLayer(LayerTable::SimPolyLayer.layerName, LayerTable::SimPolyLayer.lineTypeName,
			LayerTable::SimPolyLayer.lineWeight, LayerTable::SimPolyLayer.colorIndex,
			LayerTable::SimPolyLayer.layerId, LayerTable::SimPolyLayer.lineTypeId);					//6
		CreateNewLayer(LayerTable::AxisLineLayer.layerName, LayerTable::AxisLineLayer.lineTypeName,
			LayerTable::AxisLineLayer.lineWeight, LayerTable::AxisLineLayer.colorIndex,
			LayerTable::AxisLineLayer.layerId, LayerTable::AxisLineLayer.lineTypeId);				//7
		CreateNewLayer(LayerTable::BendLineLayer.layerName, LayerTable::BendLineLayer.lineTypeName,
			LayerTable::BendLineLayer.lineWeight, LayerTable::BendLineLayer.colorIndex,
			LayerTable::BendLineLayer.layerId, LayerTable::BendLineLayer.lineTypeId);				//8
		CreateNewLayer(LayerTable::BreakLineLayer.layerName, LayerTable::BreakLineLayer.lineTypeName,
			LayerTable::BreakLineLayer.lineWeight, LayerTable::BreakLineLayer.colorIndex,
			LayerTable::BreakLineLayer.layerId, LayerTable::BreakLineLayer.lineTypeId);				//9
		CreateNewLayer(LayerTable::BoltLineLayer.layerName, LayerTable::BoltLineLayer.lineTypeName,
			LayerTable::BoltLineLayer.lineWeight, LayerTable::BoltLineLayer.colorIndex,
			LayerTable::BoltLineLayer.layerId, LayerTable::BoltLineLayer.lineTypeId);				//10
		CreateNewLayer(LayerTable::DimSizeLayer.layerName, LayerTable::DimSizeLayer.lineTypeName,
			LayerTable::DimSizeLayer.lineWeight, LayerTable::DimSizeLayer.colorIndex,
			LayerTable::DimSizeLayer.layerId, LayerTable::DimSizeLayer.lineTypeId);					//11
		CreateNewLayer(LayerTable::DimTextLayer.layerName, LayerTable::DimTextLayer.lineTypeName,
			LayerTable::DimTextLayer.lineWeight, LayerTable::DimTextLayer.colorIndex,
			LayerTable::DimTextLayer.layerId, LayerTable::DimTextLayer.lineTypeId);					//12
		CreateNewLayer(LayerTable::CommonLayer.layerName, LayerTable::CommonLayer.lineTypeName,
			LayerTable::CommonLayer.lineWeight, LayerTable::CommonLayer.colorIndex,
			LayerTable::CommonLayer.layerId, LayerTable::CommonLayer.lineTypeId);					//13
		CreateNewLayer(LayerTable::DamagedSymbolLine.layerName, LayerTable::DamagedSymbolLine.lineTypeName,
			LayerTable::DamagedSymbolLine.lineWeight, LayerTable::DamagedSymbolLine.colorIndex,
			LayerTable::DamagedSymbolLine.layerId, LayerTable::DamagedSymbolLine.lineTypeId);					//14
		CreateNewLayer(LayerTable::SonJgAxisLineLayer.layerName, LayerTable::SonJgAxisLineLayer.lineTypeName,
			LayerTable::SonJgAxisLineLayer.lineWeight, LayerTable::SonJgAxisLineLayer.colorIndex,
			LayerTable::SonJgAxisLineLayer.layerId, LayerTable::SonJgAxisLineLayer.lineTypeId);		//7A
		LayerTable::AssistHuoQuLineLayer.CopyProperty(&LayerTable::BendLineLayer);	//8
		/*
		CreateNewLayer(LayerTable::SonJgAxisLineLayer.layerName,LayerTable::SonJgAxisLineLayer.lineTypeName,
			LayerTable::SonJgAxisLineLayer.lineWeight,LayerTable::SonJgAxisLineLayer.colorIndex,
			LayerTable::SonJgAxisLineLayer.layerId,LayerTable::SonJgAxisLineLayer.lineTypeId);		//14
		CreateNewLayer(LayerTable::AssistHuoQuLineLayer.layerName,LayerTable::AssistHuoQuLineLayer.lineTypeName,
			LayerTable::AssistHuoQuLineLayer.lineWeight,LayerTable::AssistHuoQuLineLayer.colorIndex,
			LayerTable::AssistHuoQuLineLayer.layerId,LayerTable::AssistHuoQuLineLayer.lineTypeId);	//15
		*/
		LayerTable::layerHashList.SetValue(LayerTable::VisibleProfileLayer.layerName, LayerTable::VisibleProfileLayer);
		LayerTable::layerHashList.SetValue(LayerTable::UnvisibleProfileLayer.layerName, LayerTable::UnvisibleProfileLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BriefLineLayer.layerName, LayerTable::BriefLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::AuxLineLayer.layerName, LayerTable::AuxLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BoltSymbolLayer.layerName, LayerTable::BoltSymbolLayer);
		LayerTable::layerHashList.SetValue(LayerTable::SimPolyLayer.layerName, LayerTable::SimPolyLayer);
		LayerTable::layerHashList.SetValue(LayerTable::AxisLineLayer.layerName, LayerTable::AxisLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BendLineLayer.layerName, LayerTable::BendLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BreakLineLayer.layerName, LayerTable::BreakLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::BoltLineLayer.layerName, LayerTable::BoltLineLayer);
		LayerTable::layerHashList.SetValue(LayerTable::DimSizeLayer.layerName, LayerTable::DimSizeLayer);
		LayerTable::layerHashList.SetValue(LayerTable::DimTextLayer.layerName, LayerTable::DimTextLayer);
		LayerTable::layerHashList.SetValue(LayerTable::CommonLayer.layerName, LayerTable::CommonLayer);
		LayerTable::layerHashList.SetValue(LayerTable::DamagedSymbolLine.layerName, LayerTable::DamagedSymbolLine);
		LayerTable::layerHashList.SetValue(LayerTable::SonJgAxisLineLayer.layerName, LayerTable::SonJgAxisLineLayer);
		//LayerTable::layerHashList.SetValue(LayerTable::AssistHuoQuLineLayer.layerName,LayerTable::AssistHuoQuLineLayer);
	}
	else
	{
		if (LayerTable::layerHashList.GetValue("14") == NULL)
		{
			TMADRAWING_LAYER* pLayer = LayerTable::layerHashList.Add("14");
			*pLayer = LayerTable::DamagedSymbolLine;
		}
		TMADRAWING_LAYER* pLayer = NULL;
		for (pLayer = LayerTable::layerHashList.GetFirst(); pLayer; pLayer = LayerTable::layerHashList.GetNext())
		{
			int n = LayerTable::layerHashList.GetNodeNum();
			CreateNewLayer(pLayer->layerName, pLayer->lineTypeName, pLayer->lineWeight, pLayer->colorIndex,
				pLayer->layerId, pLayer->lineTypeId);
		}
		//可见轮廓
		CXhChar16 sLayerName;
		sLayerName.Copy(LayerTable::VisibleProfileLayer.layerName);
		LayerTable::VisibleProfileLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//不可见轮廓
		sLayerName.Copy(LayerTable::UnvisibleProfileLayer.layerName);
		LayerTable::UnvisibleProfileLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//辅助单线图
		sLayerName.Copy(LayerTable::AuxLineLayer.layerName);
		LayerTable::AuxLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//中心线
		sLayerName.Copy(LayerTable::AxisLineLayer.layerName);
		LayerTable::AxisLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//角钢火曲
		sLayerName.Copy(LayerTable::BendLineLayer.layerName);
		LayerTable::BendLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//螺栓图
		sLayerName.Copy(LayerTable::BoltSymbolLayer.layerName);
		LayerTable::BoltSymbolLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//断开界线
		sLayerName.Copy(LayerTable::BreakLineLayer.layerName);
		LayerTable::BreakLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//收力材单线图
		sLayerName.Copy(LayerTable::BriefLineLayer.layerName);
		LayerTable::BriefLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//尺寸标注
		sLayerName.Copy(LayerTable::DimSizeLayer.layerName);
		LayerTable::DimSizeLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//文字标注
		sLayerName.Copy(LayerTable::DimTextLayer.layerName);
		LayerTable::DimTextLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//相似型标注
		sLayerName.Copy(LayerTable::SimPolyLayer.layerName);
		LayerTable::SimPolyLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//螺栓线
		sLayerName.Copy(LayerTable::BoltLineLayer.layerName);
		LayerTable::BoltLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//其余
		sLayerName.Copy(LayerTable::CommonLayer.layerName);
		LayerTable::CommonLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//钢板轮廓破损标识线
		sLayerName.Copy(LayerTable::DamagedSymbolLine.layerName);
		LayerTable::DamagedSymbolLine.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//辅助火曲线
		sLayerName.Copy(LayerTable::AssistHuoQuLineLayer.layerName);
		LayerTable::AssistHuoQuLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
		//子角钢中心线
		sLayerName.Copy(LayerTable::SonJgAxisLineLayer.layerName);
		LayerTable::SonJgAxisLineLayer.CopyProperty(LayerTable::layerHashList.GetValue(sLayerName));
	}
}
