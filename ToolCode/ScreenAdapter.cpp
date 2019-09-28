#include "stdafx.h"
#include "ScreenAdapter.h"

double CScreenAdapter::ZoomScaleOfKeepScrSizeX=1.0;
double CScreenAdapter::ZoomScaleOfKeepScrSizeY=1.0;
bool CScreenAdapter::InitByCurrDC(CDC* pDC)
{
	CSize step(100,100);
	int iOldMode=pDC->GetMapMode();
	pDC->SetMapMode(MM_HIMETRIC);	//设成0.01mm为逻辑单位的模式
	pDC->DPtoLP(&step);
	pDC->SetMapMode(iOldMode);
	const double STD_SCALE_OF_METRIC2PIXELX=35.28;
	const double STD_SCALE_OF_METRIC2PIXELY=35.28;
	double ScaleOfPixel2MetricX=step.cx/100.0;	//std=35.28;
	double ScaleOfPixel2MetricY=step.cy/100.0;	//std=35.28;
	ZoomScaleOfKeepScrSizeX=ScaleOfPixel2MetricX/STD_SCALE_OF_METRIC2PIXELX;
	ZoomScaleOfKeepScrSizeY=ScaleOfPixel2MetricY/STD_SCALE_OF_METRIC2PIXELY;
	return true;
}
