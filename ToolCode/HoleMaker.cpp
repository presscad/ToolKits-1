// HoleMaker.cpp: implementation of the CHoleMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"
#include "HoleMaker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHoleMaker::~CHoleMaker()
{

}
// 生成普通圆孔(waist_len=0)或腰圆孔(waist_len>0)
BOOL CHoleMaker::Create(GECS &hole_ucs,double d,double waist_len,
						f3dPolyFace *pUpFace, f3dPolyFace *pDownFace,
						fBody *pBody,COLORREF crLsHole,int slices/*=12*/)
{
	if(!normalize(hole_ucs.axis_z))	//非法的孔方向
		return FALSE;
	if(d<eps)	//孔太小
		return FALSE;
	if(slices<3)
		return FALSE;
	if(pUpFace==NULL||pDownFace==NULL||pBody==NULL)
		return FALSE;
	UCS_STRU upface_ucs,downface_ucs;
	f3dPoint inters_up,inters_down;
	typedef f3dPoint* POINT_PTR;
	int final_slices = slices;
	if(waist_len>0)
		final_slices+=2;
	f3dPoint* scan_point_arr = new f3dPoint[final_slices];
	POINT_PTR *up_cir_vertex=new POINT_PTR[final_slices];
	POINT_PTR *down_cir_vertex=new POINT_PTR[final_slices];
	pUpFace->GetPolyUCS(upface_ucs);
	pDownFace->GetPolyUCS(downface_ucs);
	Int3dlf(inters_up,hole_ucs.origin,hole_ucs.axis_z,upface_ucs.origin,upface_ucs.axis_z);
	Int3dlf(inters_down,hole_ucs.origin,hole_ucs.axis_z,downface_ucs.origin,downface_ucs.axis_z);
	//生成孔的相对坐标系
	if(hole_ucs.axis_x.IsZero()||fabs(hole_ucs.axis_x*hole_ucs.axis_z)>EPS_COS)
		hole_ucs.axis_x = inters_vec(hole_ucs.axis_z);
	hole_ucs.axis_x -= (hole_ucs.axis_x*hole_ucs.axis_z)*hole_ucs.axis_z;
	normalize(hole_ucs.axis_x);
	hole_ucs.axis_y = hole_ucs.axis_z^hole_ucs.axis_x;
	normalize(hole_ucs.axis_y);
	f3dPoint scan_point;	//投影圆周扫描点
	BOOL bFlipHoleDirection=FALSE;
	if(hole_ucs.axis_z*upface_ucs.axis_z<0)
		bFlipHoleDirection = TRUE;	//孔方向与面的上下方向相反，应颠倒孔方向
	int i;
	double half_waist_len = 0.5*waist_len;
	for(i=0;i<slices;i++)
	{
		scan_point.x = 0.5*d*cos(2*i*Pi/slices);
		scan_point.y = 0.5*d*sin(2*i*Pi/slices);
		scan_point.z = 0;
		if(waist_len>0)
		{
			double piece = 2.0*i/slices;
			if(piece<=0.5||piece>1.5)
				scan_point.x+=half_waist_len;
			else if(piece>0.5&&piece<=1.5)
				scan_point.x-=half_waist_len;
			if(piece<0.5)
				scan_point_arr[i] = scan_point;
			else if(piece==0.5)
			{
				scan_point_arr[i] = scan_point;
				scan_point.x-=2*half_waist_len;
				scan_point_arr[i+1] = scan_point;
			}
			else if(piece<1.5)
				scan_point_arr[i+1] = scan_point;
			else if(piece==1.5)
			{
				scan_point_arr[i+1] = scan_point;
				scan_point.x+=2*half_waist_len;
				scan_point_arr[i+2] = scan_point;
			}
			else
				scan_point_arr[i+2] = scan_point;
		}
		else
			scan_point_arr[i] = scan_point;
	}
	for(i=0;i<final_slices;i++)
	{
		coord_trans(scan_point_arr[i],hole_ucs,TRUE);	//转为实际坐标系下的投影圆周扫描点
		f3dPoint *pUpPt = pBody->vertex.append(0,0,0);
		f3dPoint *pDownPt = pBody->vertex.append(0,0,0);
		Int3dlf(*pUpPt,scan_point_arr[i],hole_ucs.axis_z,upface_ucs.origin,upface_ucs.axis_z);
		Int3dlf(*pDownPt,scan_point_arr[i],hole_ucs.axis_z,downface_ucs.origin,downface_ucs.axis_z);
		up_cir_vertex[i] = pUpPt;
		down_cir_vertex[i] = pDownPt;
	}
	fLoop *pUpLoop = pUpFace->inner_loop.append();
	fLoop *pDownLoop = pDownFace->inner_loop.append();
	for(i=0;i<final_slices;i++)
	{
		f3dPolyFace *pInnerFace = pBody->faceList.append();
		pInnerFace->material = crLsHole;
		if(bFlipHoleDirection)
		{
			pUpLoop->append(up_cir_vertex[i],up_cir_vertex[(i+1)%final_slices]);
			if(i<final_slices-1)
				pDownLoop->append(down_cir_vertex[final_slices-1-i],down_cir_vertex[final_slices-2-i]);
			else
				pDownLoop->append(down_cir_vertex[final_slices-1-i],down_cir_vertex[final_slices-1]);
			pInnerFace->outer_edge.append(up_cir_vertex[(i+1)%final_slices],up_cir_vertex[i]);
			pInnerFace->outer_edge.append(up_cir_vertex[i],down_cir_vertex[i]);
			pInnerFace->outer_edge.append(down_cir_vertex[i],down_cir_vertex[(i+1)%final_slices]);
			pInnerFace->outer_edge.append(down_cir_vertex[(i+1)%final_slices],up_cir_vertex[(i+1)%final_slices]);
		}
		else
		{
			pDownLoop->append(down_cir_vertex[i],down_cir_vertex[(i+1)%final_slices]);
			if(i<final_slices-1)
				pUpLoop->append(up_cir_vertex[final_slices-1-i],up_cir_vertex[final_slices-2-i]);
			else
				pUpLoop->append(up_cir_vertex[final_slices-1-i],up_cir_vertex[final_slices-1]);
			pInnerFace->outer_edge.append(up_cir_vertex[i],up_cir_vertex[(i+1)%final_slices]);
			pInnerFace->outer_edge.append(up_cir_vertex[(i+1)%final_slices],down_cir_vertex[(i+1)%final_slices]);
			pInnerFace->outer_edge.append(down_cir_vertex[(i+1)%final_slices],down_cir_vertex[i]);
			pInnerFace->outer_edge.append(down_cir_vertex[i],up_cir_vertex[i]);
		}
	}
	delete []scan_point_arr;
	delete []up_cir_vertex;
	delete []down_cir_vertex;
	return TRUE;	
}
