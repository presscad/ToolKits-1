#include "StdAfx.h"
#include "list.h"
#include "HashTable.h"

class CGlobalEnumerators
{
public:
	CHashListEx< CXhInternalTemplEnumerator< void > > hashEnumers;
	CGlobalEnumerators(){;}
	~CGlobalEnumerators()
	{
		hashEnumers.Empty();
	}
};
static CGlobalEnumerators globalEnumerators;
bool IXhEnumerator::Destroy()
{
	return DestroyEnumerBySerial(Serial());
}
bool IXhEnumerator::DestroyEnumerBySerial(UINT uiEnumerSerial)
{
	return globalEnumerators.hashEnumers.DeleteNode(uiEnumerSerial)!=FALSE;
}
IXhEnumerator* IXhEnumerator::NewEnumerator()
{
	return globalEnumerators.hashEnumers.Add(0);
}
