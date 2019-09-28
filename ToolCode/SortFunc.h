#ifndef __SORT_FUNC_
#define __SORT_FUNC_
extern int compare_int(const int& item1,const int& item2);
extern int compare_long(const long& item1,const long& item2);
extern int compare_double(const double& item1,const double& item2);
extern int compare_char(const char& item1,const char& item2);
//<DEVELOP_PROCESS_MARK nameId="">
struct ICompareClass{
	virtual int Compare(const ICompareClass *pCompareObj)const =0;
	virtual int CompareEx(const ICompareClass *pCompareObj,DWORD lPara)const{return Compare(pCompareObj);}
};
template<class COMPARE_TYPE> int ComplexCompareFunc(const COMPARE_TYPE& item1,const COMPARE_TYPE& item2)
{
	return item1.Compare(&item2);
}
template<class COMPARE_TYPE> int ComplexCompareExFunc(const COMPARE_TYPE& item1,const COMPARE_TYPE& item2,DWORD lPara)
{
	return item1.CompareEx(&item2,lPara);
}
template<class COMPARE_TYPEPTR> int ComplexComparePtrFunc(const COMPARE_TYPEPTR& item1,const COMPARE_TYPEPTR& item2)
{
	return item1->Compare(item2);
}
template<class COMPARE_TYPEPTR> int ComplexCompareExPtrFunc(const COMPARE_TYPEPTR& item1,const COMPARE_TYPEPTR& item2,DWORD lPara)
{
	return item1->CompareEx(item2,lPara);
}
template<class TYPE> int SimpleCompareFunc(const TYPE& item1,const TYPE& item2)
{
	if(item1-item2>0)
		return 1;
	else if(item1-item2<0)
		return -1;
	else
		return 0;
}
template<class COMPARE_CLS>int ClassCompareFunc(const COMPARE_CLS& item1,const COMPARE_CLS& item2)
{
	return item1.Compare(&item2);
}
//冒泡排序法(默认升序排列)
template<class TYPE>
class CBubbleSort
{
public:
	static void BubSort(TYPE *list,long nCount,int (*compare_func)(const TYPE& item1,const TYPE& item2),bool bAscending=true)
	{
		for(int i=0;i<nCount-1;i++)
		{
			bool bSorted = true;	//冒泡排序中的有序标志
			for(int j=0;j<nCount-i-1;j++)
			{
				if(( bAscending&&compare_func(list[j],list[j+1])>0)||	//升序排列
				   (!bAscending&&compare_func(list[j],list[j+1])<0))	//降序排列
				{
					TYPE tm=list[j];
					list[j]=list[j+1];
					list[j+1]=tm;
					bSorted=false;
				}
			}
			if(bSorted)
				break;
		}
	}
};

//快速排序法compare_func返回正数item1在后,item2在前,否则item1在前,item2在后
//此算法还有待改进，据《数据结构》，枢轴记录不须每次进行交换，且函数调用堆栈深度计算可能有误WJH-2006/04/19
template<class TYPE>
class CQuickSort
{
	static BOOL SubSort(TYPE *arr,long start,long end,long nCount,int (*compare_func)(const TYPE& item1,const TYPE& item2),long recursion)
	{
		long low,high;
		TYPE pivot_key;
		if(start>end)
			return FALSE;
		else if(start==end)
			return TRUE;
		if(start<0||end>=nCount)
			return FALSE;
		low = start;
		pivot_key = arr[low];
		high = end;
		bool bStartSorted=true,bEndSorted=true;
		while(low<high)
		{
			while(low<high&&compare_func(arr[high],pivot_key)>=0)
			{
				if(high<end&&compare_func(arr[high],arr[high+1])>0)
				{	//加上冒泡排序以加快快速排序的速度
					TYPE tmkey=arr[high];
					arr[high]=arr[high+1];
					arr[high+1]=tmkey;
					bEndSorted=false;
				}
				high--;
			}
			arr[low] = arr[high];
			while(low<high&&compare_func(arr[low],pivot_key)<=0)
			{
				if(low>start&&compare_func(arr[low-1],arr[low])>0)
				{	//加上冒泡排序以加快快速排序的速度
					TYPE tmkey=arr[low-1];
					arr[low-1]=arr[low];
					arr[low]=tmkey;
					bStartSorted=false;
				}
				low++;
			}
			arr[high] = arr[low];
		}
		arr[low] = pivot_key;
		if(low==end)
		{
			low--;
			end--;
		}
		else if(low==start)
		{
			start++;
			low++;
		}
		recursion++;
		if(recursion<=200)
		{
			if(!bStartSorted&&low-start>1)
				SubSort(arr,start,low,nCount,compare_func,recursion);
			if(!bEndSorted&&end-low>1)
				SubSort(arr,low,end,nCount,compare_func,recursion);
		}
		else
		{
			if(low-start>1)
				CBubbleSort<TYPE>::BubSort(arr+start,low-start+1,compare_func);
			if(end-low>1)
				CBubbleSort<TYPE>::BubSort(arr+low,end-low+1,compare_func);
		}
		return TRUE;
	}
public:
	static BOOL QuickSort(TYPE *arr,long nCount,int (*compare_func)(const TYPE& item1,const TYPE& item2))
	{
		long recursion=0;
		return SubSort(arr,0,nCount-1,nCount,compare_func,recursion);
	}
	static BOOL QuickSortClassic(TYPE *arr,long nCount)
	{
		long recursion=0;
		return SubSort(arr,0,nCount-1,nCount,ClassCompareFunc<TYPE>,recursion);
	}
};

//堆排序法compare_func返回正数item1在后,item2在前,否则item1在前,item2在后
struct COMPARE_FUNC_EXPARA{
	BOOL bAscending;
	long iSubItem;
	COMPARE_FUNC_EXPARA(BOOL ascending, long iCol){bAscending=ascending;iSubItem=iCol;}
};
template<class TYPE>
class CHeapSort
{
	static void Swap(TYPE &atom1,TYPE &atom2)
	{
		TYPE key = atom1;
		atom1 = atom2;
		atom2 = key;
	}
	static void HeapAdjust(TYPE *heap_arr,int s, int m,int (*compare_func)(const TYPE& item1,const TYPE& item2))
	{
		s+=1;	//将数组由0基底变为1基底
		m+=1;
		TYPE rc = heap_arr[s-1];
		for(int j=2*s;j<=m;j*=2)
		{	//沿key较大的孩子结点向下筛选
			if(j<m&&compare_func(heap_arr[j-1],heap_arr[j])<0)
				j++;	//j为key较大的记录的下标
			if(compare_func(rc,heap_arr[j-1])>=0)
				break;	//rc应插入在位置s上
			heap_arr[s-1] = heap_arr[j-1];
			s = j;
		}
		heap_arr[s-1] = rc;
	}
	static void HeapAdjustEx(TYPE *heap_arr,int s, int m,int (*compare_func)(const TYPE& item1,const TYPE& item2,DWORD lPara),DWORD lPara)
	{
		s+=1;	//将数组由0基底变为1基底
		m+=1;
		TYPE rc = heap_arr[s-1];
		for(int j=2*s;j<=m;j*=2)
		{	//沿key较大的孩子结点向下筛选
			if(j<m&&compare_func(heap_arr[j-1],heap_arr[j],lPara)<0)
				j++;	//j为key较大的记录的下标
			if(compare_func(rc,heap_arr[j-1],lPara)>=0)
				break;	//rc应插入在位置s上
			heap_arr[s-1] = heap_arr[j-1];
			s = j;
		}
		heap_arr[s-1] = rc;
	}
public:
	static void HeapSort(TYPE *heap_arr,long nCount,int (*compare_func)(const TYPE& item1,const TYPE& item2))
	{
		int i;
		for(i=nCount/2-1;i>=0;i--)
			HeapAdjust(heap_arr,i,nCount-1,compare_func);
		for(i=nCount-1;i>0;i--)
		{
			Swap(heap_arr[0],heap_arr[i]);
			HeapAdjust(heap_arr,0,i-1,compare_func);
		}
	}
	static void HeapSortEx(TYPE *heap_arr,long nCount,int (*compare_func_ex)(const TYPE& item1,const TYPE& item2,DWORD lPara),DWORD lPara)
	{
		int i;
		for(i=nCount/2-1;i>=0;i--)
			HeapAdjustEx(heap_arr,i,nCount-1,compare_func_ex,lPara);
		for(i=nCount-1;i>0;i--)
		{
			Swap(heap_arr[0],heap_arr[i]);
			HeapAdjustEx(heap_arr,0,i-1,compare_func_ex,lPara);
		}
	}
	static void HeapSortClassic(TYPE *heap_arr,long nCount)
	{
		HeapSort(heap_arr,nCount,ClassCompareFunc<TYPE>);
	}
};
//</DEVELOP_PROCESS_MARK>
#endif
