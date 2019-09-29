#pragma once
#include "Buffer.h"
#include "f_ent_list.h"
#include "HashTable.h"
#include "XhCharString.h"
#include "Variant.h"

class CBomTblTitleCfg
{
public:
	const static int T_COL_COUNT	=9;
	const static char* T_PART_NO;	 //= "���";
	const static char* T_METERIAL;	 //= "����";
	const static char* T_LEN;		 //= "����";
	const static char* T_SPEC;		 //= "���";
	const static char* T_NUM;		 //= "����";
	const static char* T_SING_WEIGHT; //= "����";
	const static char* T_NOTES;		 //= "��ע";
	const static char* T_WIDE;		 //= "���";
	const static char* T_PARTTYPE;	 //= "����";
	//
	const static int INDEX_PART_NO		= 0; //= "���";
	const static int INDEX_METERIAL		= 1; //= "����";
	const static int INDEX_LEN			= 2; //= "����";
	const static int INDEX_SPEC			= 3; //= "���";
	const static int INDEX_NUM			= 4; //= "����";
	const static int INDEX_SING_WEIGHT	= 5; //= "����";
	const static int INDEX_NOTES		= 6; //= "��ע";
	const static int INDEX_WIDE			= 7; //= "���";
	const static int INDEX_PARTTYPE		= 8; //= "����";
public:
	int m_nStartRow,m_nColCount;
	CXhChar500 m_sColTitleArr;	//
	CXhChar500 m_sColIndexArr;	//
public:
	CBomTblTitleCfg(){m_nStartRow=1;m_nColCount=0;}
	CBomTblTitleCfg(const int *colIndexArr,const char *colTitleArr,int colCount,int startRow);
	~CBomTblTitleCfg();
	void Init(const int *colIndexArr,const char *colTitleArr,int colCount,int startRow);
	bool IsMatch(CVariant2dArray &sheetContent);
	bool IsValid();
	bool IsEqual(CBomTblTitleCfg &cfg);
	bool GetHashColIndexByColTitleTbl(CHashStrList<DWORD> &hashColIndexByColTitle);
	void Clone(CBomTblTitleCfg &srcCfg);
	static bool IsMatchTitle(int colIndex,const char* title);
};

class CBomTblTitleCfgSet
{
	bool m_bAutoSaveToFile;
	CXhChar500 m_sCfgFilePath;
	CXhChar500 m_sCfgFileFolder;
	ATOM_LIST<CBomTblTitleCfg> recList;
public:
	CBomTblTitleCfgSet(const char* cfg_file_folder,bool bAutoSaveToFile);
	~CBomTblTitleCfgSet();
	void LoadFromFile();
	void SaveToFile();
	bool AppendCfg(CBomTblTitleCfg &cfg);
	CBomTblTitleCfg *FindMatchCfg(CVariant2dArray &sheetContent);

	static char CFG_FILE_NAME[50];
	static bool AppendRecToFile(CBomTblTitleCfg &cfg,const char*cfg_file_folder);
	static bool FindMatchCfg(CVariant2dArray &sheetContent,const char*cfg_file_folder,CBomTblTitleCfg *pCfg);
};