#pragma once
#include "afxwin.h"
//最终坐标.x = 预设坐标.x * 显示器宽度/预设显示器宽度。
//最终坐标.y = 预设坐标.y * 显示器高度/预设显示器高度。

class CScreenAdapter
{
private:
	//static const int SCREEN_PRESET_WIDTH=1440;
	//static const int SCREEN_PRESET_HEIGHT=900;
	static double ZoomScaleOfKeepScrSizeX;
	static double ZoomScaleOfKeepScrSizeY;
public:
	static bool InitByCurrDC(CDC* pDC);
	static CPoint TransPoint(CPoint &srcPt)
	{
		int nWidth=GetSystemMetrics(SM_CXSCREEN);  //得到屏幕宽度 
		int nHeight=GetSystemMetrics(SM_CYSCREEN);
		CPoint destPt;
		destPt.x=(int)(srcPt.x*ZoomScaleOfKeepScrSizeX+0.5);
		destPt.y=(int)(srcPt.y*ZoomScaleOfKeepScrSizeY+0.5);
		return destPt;
	}
	static int AdaptWidthX(int width)
	{
		return (int)(width*ZoomScaleOfKeepScrSizeX+0.5);
	}
	static int AdaptHeightY(int height)
	{
		return (int)(height*ZoomScaleOfKeepScrSizeY+0.5);
	}
};