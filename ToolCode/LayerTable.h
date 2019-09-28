#pragma once
#ifndef __LAYER_TABLE_H_
#define __LAYER_TABLE_H_
#include "XhCharString.h"
#include "HashTable.h"
#include "f_ent_list.h"

typedef struct tagTOWERATOM_COLOR_INDEX
{
	int crNode;
	int crOutLine;		//可见轮廓线及一般字型
	int crDashLine;		//不可见轮廓线及非本段实体
	int crLs;			//螺栓符号及标注
	int crDimSimTri;	//相似三角形及标注
	int crSingleLine;	//单实线主用于单线图
	int crAxisLine;		//点划线（中心线）
	int crHuoQuLine;	//火曲线
	int crDragLine;		//引出线

	tagTOWERATOM_COLOR_INDEX() {
		crLs = 1;		//螺栓--红色
		crOutLine = 7;		//轮廓线--黑白
		crSingleLine = 7;	//单线图--黑白
		crHuoQuLine = 3;	//火曲线--绿色
		crDashLine = 1;		//长短画线--红色
		crAxisLine = 2;		//(心)轴线--黄线
		crNode = 4;			//节点--青色
		crDimSimTri = 1;	//标注--黑白
		crDragLine = 3;		//引出线--绿色
	}

}TOWERATOM_COLOR_INDEX;

//CAD环境的操作函数及图层的样式设置
typedef struct tagTMADRAWING_LAYER
{
	int colorIndex;				//图层默认的图元显示颜色索引
	AcDb::LineWeight lineWeight;//线型宽
	CXhChar16 layerName;		//图层名均应以一个字符命名
	CXhChar16 lineTypeName;		//图层默认线型的字符串名称
	AcDbObjectId layerId;		//图层对应的ACAD对象标识Id
	AcDbObjectId lineTypeId;	//图层默认线型的ACAD对象标识Id
	void CopyProperty(tagTMADRAWING_LAYER *pLayer);
}TMADRAWING_LAYER;
class LayerTable
{
public:
	static CHashStrList<TMADRAWING_LAYER> layerHashList;	//图层哈希表以图层名为键值
	static TMADRAWING_LAYER VisibleProfileLayer;	//结构图中可见轮廓线图层
	static TMADRAWING_LAYER BriefLineLayer;			//受力材单线图
	static TMADRAWING_LAYER AuxLineLayer;			//辅材单线图
	static TMADRAWING_LAYER UnvisibleProfileLayer;	//结构图中不可见轮廓线图层
	static TMADRAWING_LAYER AxisLineLayer;			//中心线、对称线、定位轴线
	static TMADRAWING_LAYER SonJgAxisLineLayer;		//子角钢心线图层 
	static TMADRAWING_LAYER BreakLineLayer;			//断开界线
	static TMADRAWING_LAYER BendLineLayer;			//角钢火曲、钢板火曲
	static TMADRAWING_LAYER AssistHuoQuLineLayer;	//辅助火曲线
	static TMADRAWING_LAYER SimPolyLayer;			//相似形标注
	static TMADRAWING_LAYER BoltSymbolLayer;		//螺栓图符
	static TMADRAWING_LAYER BoltLineLayer;			//螺栓线
	static TMADRAWING_LAYER DimSizeLayer;			//尺寸标注
	static TMADRAWING_LAYER DimTextLayer;			//文字标注
	static TMADRAWING_LAYER DamagedSymbolLine;		//板边破损标记线
	static TMADRAWING_LAYER CommonLayer;			//除以上以外的所有图元所在图层

	static TOWERATOM_COLOR_INDEX ColorIndex;

	static void InitLayerTable();
};

//图层线型设置
bool load_linetype(char *LineType);
bool CreateNewLayer(char *newlayer, char* line_type, AcDb::LineWeight line_thick,
	int color_i, AcDbObjectId &layerId, AcDbObjectId &lineTypeId);
void GetCurDwgLayers(ATOM_LIST<CXhChar50> &layer_list);
#endif