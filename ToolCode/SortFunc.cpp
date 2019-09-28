#include "stdafx.h"
#include "SortFunc.h"
int compare_int(const int& item1,const int& item2)
{
	if(item1>item2)
		return 1;
	else if(item1<item2)
		return -1;
	else
		return 0;
}
int compare_long(const long& item1,const long& item2)
{
	if(item1>item2)
		return 1;
	else if(item1<item2)
		return -1;
	else
		return 0;
}
int compare_double(const double& item1,const double& item2)
{
	if(item1>item2)
		return 1;
	else if(item1<item2)
		return -1;
	else
		return 0;
}
int compare_char(const char& item1,const char& item2)
{
	if(item1>item2)
		return 1;
	else if(item1<item2)
		return -1;
	else
		return 0;
}
