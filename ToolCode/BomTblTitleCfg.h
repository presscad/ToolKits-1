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
	const static char* T_PART_NO;	 //= "编号";
	const static char* T_METERIAL;	 //= "材质";
	const static char* T_LEN;		 //= "长度";
	const static char* T_SPEC;		 //= "规格";
	const static char* T_NUM;		 //= "件数";
	const static char* T_SING_WEIGHT; //= "单重";
	const static char* T_NOTES;		 //= "备注";
	const static char* T_WIDE;		 //= "宽度";
	const static char* T_PARTTYPE;	 //= "类型";
	const static char* T_MANU_NUM;	 //="加工数"
	const static char* T_MANU_WEIGHT;//="加工重量"
	const static char* T_REPLACE_SPEC; //= "代用规格"
	//
	const static int INDEX_PART_NO		= 0; //= "编号";
	const static int INDEX_METERIAL		= 1; //= "材质";
	const static int INDEX_LEN			= 2; //= "长度";
	const static int INDEX_SPEC			= 3; //= "规格";
	const static int INDEX_NUM			= 4; //= "件数";
	const static int INDEX_SING_WEIGHT	= 5; //= "单重";
	const static int INDEX_NOTES		= 6; //= "备注";
	const static int INDEX_WIDE			= 7; //= "宽度";
	const static int INDEX_PARTTYPE		= 8; //= "类型";
	const static int INDEX_MANU_NUM		= 9; //= "加工数";
	const static int INDEX_MANU_WEIGHT	= 10; //= "加工重量";
	const static int INDEX_REPLACE_SPEC = 11; //= "代用规格"
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
	static int DetectTitleRowIndex(CVariant2dArray &sheetContent);
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