#ifndef __COMPARE_PART_NO_H_
#define __COMPARE_PART_NO_H_
#include "SegI.h"

//段号排序
int CompareSegI(const SEGI &seg1,const SEGI &seg2);
//对构件字符串进行排序
int ComparePartNoString(const char *str1,const char *str2,char* materialPrefixCharSet=NULL);
//比较分多数据区的字符串，如角钢规格
int CompareMultiSectionString(const char *str1,const char *str2);
//比较两种螺栓规格的大小
int CompareBoltSizeString(const char *str1,const char *str2);
#endif