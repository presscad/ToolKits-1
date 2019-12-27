#pragma once
#include "IXeroCad.h"
#include "DrawDim.h"

class CLDSPart;
class CLDSLineTube;
class AcDbBlockTableRecord;
struct IDbEntsContext{
	virtual CLDSPart* WeldRootPart(){return NULL;}
	virtual void SetUser2ScrDrawScale(double scale_user2scr)=0;
	virtual double GetUser2ScrDrawScale()=0;
	virtual double ZoomToModelTextSize(double fSysFontSize)=0;	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
	virtual FRAMESIZE ZoomToModelTextSize(FRAMESIZE& framesize)=0;	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
	virtual bool IsSectionDrawing(){return false;}
	virtual bool IsBranchRod(){return false;}
	virtual IDbEntity* AppendDbEntity(int iDbEntType)=0;
	virtual void AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,double fDimR=0,char *dimText=NULL,double fDimAngleTextSize=3.0)=0;
	virtual void AppendAngularDim(CSizeAngleDim& angularDim)=0;
	virtual void AppendLine(f3dLine& line)=0;
	virtual void AppendLine(f3dArcLine& arcline)=0;
	virtual void AppendCircle(f3dCircle& circle)=0;
	virtual void AppendSizeTextDim(CSizeTextDim& sizeTextDim)=0;
	virtual void AppendTextDim(CTextOnlyDrawing& pureTextDim)=0;
	//�������ݱ�ʶ��
	virtual void AppendDataPoint(CTextFrame& dataPoint)=0;
	virtual DATAPOINT_TAG* AppendDataPoineTag()=0;
	//���ƺ�����
	virtual void DrawWeldLineSymbol(f3dPoint start,f3dPoint end,double width=0,double height=0)=0;
	virtual void DrawWeldLineSymbol(f3dArcLine arcline,double width=0,double height=0)=0;
	virtual double TransToModelLength(double scr_draw_length)=0;
	//��������������
	virtual bool IsHideFuncSetted(){return false;}
	virtual void SetHideFunc(void (*ProcessHideLine)(AcDbBlockTableRecord *pBlockTableRecord,fPtList& vertex_list,char *layer))=0;
	virtual void HideLegacyLines(fPtList& vertex_list,char* layer)=0;
};

//STORE_ACAD:�ݴ�������������Ȼ��������Acad��AdDbEntityʵ������;
//STORE_ACADINSTANT:ֱ������Acad��AdDbEntityʵ������;
//STORE_XERO:����XeroCad�е�IDbEntityʵ������
enum STORE_MODE{STORE_ACAD,STORE_ACADINSTANT,STORE_XERO};
//class CSectionViewDrawing;
class CDrawingEntsContext : public IDbEntsContext{
public:
	enum STORE_TYPE{XERO_COMMAND=1,XERO_COMPONENT=2,XERO_DRAWING=3,XERO_OTHER=100};
protected:
	STORE_MODE m_cStoreMode;	//0.��ʾֱ�Ӵ洢��Acad
	STORE_TYPE cStoreType;	
	union DATA_MANAGER{
		IDrawingComponent* pSectDrawing;//CSectionViewDrawing* pSectDrawing;
		IDrawingCommand* pXeroCmd;
		IDrawing* pXeroDrawing;
		AcDbBlockTableRecord *pBlockTableRecord;
	}context;
	double m_fUser2ScrDrawScale;			//��ͼ����
	void (*ProcessHideLineFunc)(AcDbBlockTableRecord *pBlockTableRecord,fPtList& vertex_list,char *layer);
public:
	CDrawingEntsContext(IDrawingComponent* pSectDrawing=NULL,STORE_MODE cStoreMode=STORE_ACAD){
		InitContext(pSectDrawing,cStoreMode);
	}
	CDrawingEntsContext(IDrawingCommand* pXeroCmd){InitContext(pXeroCmd);}
	CDrawingEntsContext(IDrawing* pXeroDrawing){InitContext(pXeroDrawing);}
	~CDrawingEntsContext(){;}
public:
	void InitContext(IDrawingComponent* pSectDrawing,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(IDrawing* pXeroDrawing,STORE_MODE cStoreMode=STORE_ACAD);
	void InitContext(AcDbBlockTableRecord *pBlockTableRecord);
	//��������������
	virtual void HideLegacyLines(fPtList& vertex_list,char* layer);
	virtual void SetHideFunc(void (*ProcessHideLine)(AcDbBlockTableRecord *pBlockTableRecord,fPtList& vertex_list,char *layer));
	virtual bool IsHideFuncSetted(){return ProcessHideLineFunc!=NULL;}
	virtual void SetUser2ScrDrawScale(double scale_user2scr){m_fUser2ScrDrawScale=scale_user2scr;}
public:
	//virtual bool IsBranchRod(CLDSLinePart* pBranchRod){ return false;}
	//virtual CLDSPart* WeldRootPart(){return m_pRootPart;}
	virtual double GetUser2ScrDrawScale(){return m_fUser2ScrDrawScale;}
	virtual double ZoomToModelTextSize(double fSysFontSize);	//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
	virtual FRAMESIZE ZoomToModelTextSize(FRAMESIZE& framesize);//fSysFontSize��ϵͳ�趨������߶ȣ�����ֵ�ǿ��ǵ���ͼ�������ģ�ͳߴ緶�������߶ȡ�wjh-2017.11.9
	IDbEntity* AppendDbEntity(int idDbEntType);
	void AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,double fDimR=0,char *dimText=NULL,double fDimAngleTextSize=3.0);
	void AppendAngularDim(CSizeAngleDim& angularDim);
	void AppendLine(f3dLine& line);
	void AppendLine(f3dArcLine& arcline);
	void AppendSpLine(fPtList& vertex_list);
	void AppendCircle(f3dCircle& circle);
	void AppendSizeTextDim(CSizeTextDim& sizeTextDim);
	void AppendTextDim(CTextOnlyDrawing& pureTextDim);
	//�������ݱ�ʶ��
	void AppendDataPoint(CTextFrame& dataPoint);
	DATAPOINT_TAG* AppendDataPoineTag();
	//���ƺ�����
	void DrawWeldLineSymbol(f3dLine line,f3dPoint pickPt,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dPoint start,f3dPoint end,double width=0,double height=0);
	void DrawWeldLineSymbol(f3dArcLine arcline,double width=0,double height=0);
	double TransToModelLength(double scr_draw_length);
	//�ֹܶ���ͼ�Ļ���Ǳ�ע��׼��
	void AppendTubeBaseLine(BYTE iMode,CLDSLineTube* pBaseTube,f3dPoint arrowVec,f3dPoint arrowBtm,f3dPoint arrowHead);
	void AppendTubeTaCoreVecLine(f3dPoint vec,f3dPoint xBtm,f3dPoint xHead);
};