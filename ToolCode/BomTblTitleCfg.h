#pragma once
#include "Buffer.h"
#include "f_ent_list.h"
#include "HashTable.h"
#include "XhCharString.h"
#include "Variant.h"

class CBomTblTitleCfg
{
public:
	const static int T_COL_COUNT		=20;
	const static char* T_PART_NO;		//= "���";
	const static char* T_METERIAL;		//= "����";
	const static char* T_SPEC;			//= "���";
	const static char* T_LEN;			//= "����";
	const static char* T_SING_NUM;		//= "������";
	const static char* T_SING_WEIGHT;	//= "����";
	const static char* T_NOTES;			//= "��ע";
	const static char* T_WIDE;			//= "���";
	const static char* T_PARTTYPE;		//= "����";
	const static char* T_MANU_NUM;		//= "�ӹ���"
	const static char* T_MANU_WEIGHT;	//= "�ӹ�����"
	const static char* T_REPLACE_SPEC;	//= "���ù��"
	const static char* T_WELD;			//= "����"
	const static char* T_ZHI_WAN;		//= "����"
	const static char* T_CUT_ANGLE;		//= "�н�"
	const static char* T_CUT_ROOT;		//= "�ٸ�"
	const static char* T_CUT_BER;		//= "����"
	const static char* T_PUSH_FLAT;		//= "���"
	const static char* T_KAI_JIAO;		//= "����"
	const static char* T_HE_JIAO;		//= "�Ͻ�"
	//
	const static int INDEX_PART_NO		= 0; //= "���";
	const static int INDEX_METERIAL		= 1; //= "����";
	const static int INDEX_SPEC			= 2; //= "���";
	const static int INDEX_LEN			= 3; //= "����";
	const static int INDEX_SING_NUM		= 4; //= "����";
	const static int INDEX_SING_WEIGHT	= 5; //= "����";
	const static int INDEX_NOTES		= 6; //= "��ע";
	const static int INDEX_WIDE			= 7; //= "���";
	const static int INDEX_PARTTYPE		= 8; //= "����";
	const static int INDEX_MANU_NUM		= 9; //= "�ӹ���";
	const static int INDEX_MANU_WEIGHT	= 10;//= "�ӹ�����";
	const static int INDEX_REPLACE_SPEC = 11;//= "���ù��"
	const static int INDEX_WELD			= 12;//= "����"
	const static int INDEX_ZHI_WAN		= 13;//= "����"
	const static int INDEX_CUT_ANGLE	= 14;//= "�ٸ�"
	const static int INDEX_CUT_ROOT		= 15;//= "�ٸ�"
	const static int INDEX_CUT_BER		= 16;//= "����"
	const static int INDEX_PUSH_FLAT	= 17;//= "ѹ��"
	const static int INDEX_KAI_JIAO		= 18;//= "����"
	const static int INDEX_HE_JIAO		= 19;//= "�Ͻ�"
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