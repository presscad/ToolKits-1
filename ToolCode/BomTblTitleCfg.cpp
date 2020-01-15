#include "stdafx.h"
#include "BomTblTitleCfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#if defined(DEBUG_NEW)		//只有在MFC工程中才有DEBUG_NEW的定义
#define new DEBUG_NEW
#endif
#endif

static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if(value.vt==VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if(value.vt==VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if(value.vt==VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if(value.vt==VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else 
		return sValue;
}
//////////////////////////////////////////////////////////////////////////
// CBomTblTitleCfg
const char* CBomTblTitleCfg::T_PART_NO		= "编号";
const char* CBomTblTitleCfg::T_METERIAL		= "材质";
const char* CBomTblTitleCfg::T_LEN			= "长度";
const char* CBomTblTitleCfg::T_SPEC			= "规格";
const char* CBomTblTitleCfg::T_SING_NUM		= "件数";
const char* CBomTblTitleCfg::T_SING_WEIGHT	= "单重";
const char* CBomTblTitleCfg::T_NOTES		= "备注";
const char* CBomTblTitleCfg::T_WIDE			= "宽度";
const char* CBomTblTitleCfg::T_PARTTYPE		= "类型";
const char* CBomTblTitleCfg::T_MANU_NUM		= "加工数";
const char* CBomTblTitleCfg::T_MANU_WEIGHT	= "加工重量";
const char* CBomTblTitleCfg::T_REPLACE_SPEC = "代用规格";
const char* CBomTblTitleCfg::T_WELD			= "焊接";
const char* CBomTblTitleCfg::T_ZHI_WAN		= "制弯";
const char* CBomTblTitleCfg::T_CUT_ANGLE	= "切角";
const char* CBomTblTitleCfg::T_CUT_ROOT		= "刨根";
const char* CBomTblTitleCfg::T_CUT_BER		= "铲背";
const char* CBomTblTitleCfg::T_PUSH_FLAT	= "压扁";
const char* CBomTblTitleCfg::T_KAI_JIAO		= "开角";
const char* CBomTblTitleCfg::T_HE_JIAO		= "合角";
CBomTblTitleCfg::CBomTblTitleCfg(const int *colIndexArr,const char *colTitleArr,int colCount,int startRow)
{
	Init(colIndexArr,colTitleArr,colCount,startRow);
}
CBomTblTitleCfg::~CBomTblTitleCfg()
{
}
void CBomTblTitleCfg::Init(const int *colIndexArr,const char *colTitleArr,int colCount,int startRow)
{
	if(colIndexArr!=NULL&&colCount>0)
	{
		for(int i=0;i<colCount;i++)
			m_sColIndexArr.Append(CXhChar50("%d|",colIndexArr[i]));
	}
	if(colTitleArr!=NULL)
		m_sColTitleArr.Copy(colTitleArr);
	m_nStartRow=startRow;
	m_nColCount=colCount;
}
bool CBomTblTitleCfg::IsMatch(CVariant2dArray &sheetContent)
{
	if(m_nStartRow<=0)
		return false;
	else if(m_nStartRow==1&&m_sColIndexArr.GetLength()>0)
	{	//应从内容重取样判断是否匹配
		return true;
	}
	else
	{
		int i=0,i2=0,index=0,nColCount=0;
		CXhChar100 titleArr[100];
		CString sTitle=m_sColTitleArr;
		while ((i2=sTitle.Find('|',i)) != -1)
		{
			titleArr[index]=sTitle.Mid(i,i2-i);
			index++;
			if(index==100)
				break;	//最多支持100列
			i=i2+1;
		}
		nColCount=index;
		//验证标题是否完全一致
		VARIANT var;
		int iTitleRow=m_nStartRow-1;
		for(int i=0;i<nColCount;i++)
		{
			CXhChar100 sSheetTitle;
			CXhChar100 sOrgTitle=titleArr[i];
			if(sheetContent.GetValueAt(iTitleRow-1,i,var))
				sSheetTitle=VariantToString(var);
			if(!sSheetTitle.EqualNoCase(sOrgTitle))
				return false;
		}
		CHashStrList<DWORD> hashColIndexByColTitle;
		GetHashColIndexByColTitleTbl(hashColIndexByColTitle);
		if (hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_PART_NO)==NULL||
			hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_LEN)==NULL||
			hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SPEC)==NULL)
		{	//缺少必要列，返回false
			return false;
		}
		else
			return true;
	}
}
bool CBomTblTitleCfg::IsValid()
{
	if(m_nStartRow<=0||m_sColIndexArr.GetLength()<=0||
	  (m_nStartRow==1&&m_sColTitleArr.GetLength()>0)||
	  (m_nStartRow>1&&m_sColTitleArr.GetLength()<=0))
		return false;
	else
		return true;
}
bool CBomTblTitleCfg::IsEqual(CBomTblTitleCfg &cfg)
{
	if( cfg.m_nStartRow!=m_nStartRow||
		cfg.m_nColCount!=m_nColCount||
		cfg.m_sColIndexArr.EqualNoCase(m_sColIndexArr)||
		cfg.m_sColTitleArr.EqualNoCase(m_sColTitleArr))
		return false;
	else
		return true;
}
bool CBomTblTitleCfg::GetHashColIndexByColTitleTbl(CHashStrList<DWORD> &hashColIndexByColTitle)
{
	if (m_sColIndexArr.GetLength() <= 0)
		return false;
	int colIndexArr[100]={0};
	int i=0,i2=0,index=0,nColCount=0;
	CString sColIndex=m_sColIndexArr;
	while ((i2=sColIndex.Find('|',i)) != -1)
	{
		colIndexArr[index]=atoi(sColIndex.Mid(i,i2-i));
		index++;
		if(index==100)
			break;	//最多支持100列
		i=i2+1;
	}
	nColCount=index;
	hashColIndexByColTitle.Empty();
	for(int i=0;i<T_COL_COUNT;i++)
	{
		if(colIndexArr[i]<=0)
			continue;
		CXhChar100 sKey;
		if (i == INDEX_PART_NO)
			sKey.Copy(T_PART_NO);
		else if (i == INDEX_METERIAL)
			sKey.Copy(T_METERIAL);
		else if (i == INDEX_SPEC)
			sKey.Copy(T_SPEC);
		else if (i == INDEX_LEN)
			sKey.Copy(T_LEN);
		else if (i == INDEX_SING_NUM)
			sKey.Copy(T_SING_NUM);
		else if (i == INDEX_SING_WEIGHT)
			sKey.Copy(T_SING_WEIGHT);
		else if (i == INDEX_NOTES)
			sKey.Copy(T_NOTES);
		else if (i == INDEX_WIDE)
			sKey.Copy(T_WIDE);
		else if (i == INDEX_PARTTYPE)
			sKey.Copy(T_PARTTYPE);
		else if (i == INDEX_MANU_NUM)
			sKey.Copy(T_MANU_NUM);
		else if (i == INDEX_MANU_WEIGHT)
			sKey.Copy(T_MANU_WEIGHT);
		else if (i == INDEX_REPLACE_SPEC)
			sKey.Copy(T_REPLACE_SPEC);
		else if (i == INDEX_WELD)
			sKey.Copy(T_WELD);
		else if (i == INDEX_ZHI_WAN)
			sKey.Copy(T_ZHI_WAN);
		else if (i == INDEX_CUT_ANGLE)
			sKey.Copy(T_CUT_ANGLE);
		else if (i == INDEX_CUT_ROOT)
			sKey.Copy(T_CUT_ROOT);
		else if (i == INDEX_CUT_BER)
			sKey.Copy(T_CUT_BER);
		else if (i == INDEX_PUSH_FLAT)
			sKey.Copy(T_PUSH_FLAT);
		else if (i == INDEX_KAI_JIAO)
			sKey.Copy(T_KAI_JIAO);
		else if (i == INDEX_HE_JIAO)
			sKey.Copy(T_HE_JIAO);
		int iTitle=(i<nColCount)?(colIndexArr[i]-1):0;
		hashColIndexByColTitle.SetValue(sKey,iTitle);
	}
	return (hashColIndexByColTitle.GetNodeNum()>0);
}
void CBomTblTitleCfg::Clone(CBomTblTitleCfg &srcCfg)
{
	m_nStartRow=srcCfg.m_nStartRow;
	m_nColCount=srcCfg.m_nColCount;
	m_sColTitleArr.Copy(srcCfg.m_sColTitleArr);
	m_sColIndexArr.Copy(srcCfg.m_sColIndexArr);
}
bool CBomTblTitleCfg::IsMatchTitle(int colIndex,const char* title)
{
	CXhChar100 sTitle(title);
	if(INDEX_PART_NO==colIndex)
	{
		if( sTitle.EqualNoCase(T_PART_NO)||
			sTitle.EqualNoCase("构件编号")||
			sTitle.EqualNoCase("零件编号")||
			sTitle.EqualNoCase("图纸号码") ||
			sTitle.EqualNoCase("部件名") ||
			sTitle.EqualNoCase("件号") ||
			strstr(sTitle,"编号")!=0)
			return true;
		else
			return false;
	}
	else if(INDEX_METERIAL==colIndex)
	{
		if( sTitle.EqualNoCase(T_METERIAL)||
			sTitle.EqualNoCase("设计材质")||
			strstr(sTitle,"材质")!=0)
			return true;
		else
			return false;
	}
	else if(INDEX_LEN==colIndex)
	{
		if( sTitle.EqualNoCase(T_LEN)||
			sTitle.EqualNoCase("实际尺寸")||
			strstr(sTitle,"长度")!=0)
			return true;
		else
			return false;
	}
	else if(INDEX_SPEC==colIndex)
	{
		if( sTitle.EqualNoCase(T_SPEC)||
			sTitle.EqualNoCase("设计规格")||
			(strstr(sTitle, "规格") != 0 && strstr(sTitle, "代用") == NULL))
			return true;
		else
			return false;
	}
	else if(INDEX_SING_NUM==colIndex)
	{
		if( sTitle.EqualNoCase(T_SING_NUM)||
			sTitle.EqualNoCase("单基")||
			sTitle.EqualNoCase("单基数量") ||
			sTitle.EqualNoCase("数量") ||
			sTitle.EqualNoCase("单基\n数量") ||
			sTitle.EqualNoCase("单数") ||
			strstr(sTitle, "单基") != 0)
			return true;
		else
			return false;
	}
	else if (INDEX_MANU_NUM == colIndex)
	{
		if (sTitle.EqualNoCase(T_MANU_NUM) ||
			sTitle.EqualNoCase("加工") ||
			sTitle.EqualNoCase("加工数量") ||
			sTitle.EqualNoCase("加工数") ||
			sTitle.EqualNoCase("总数") ||
			sTitle.EqualNoCase("加工总数量"))
			return true;
		else
			return false;
	}
	else if(INDEX_SING_WEIGHT==colIndex)
	{
		if( sTitle.EqualNoCase(T_SING_WEIGHT)||
			sTitle.EqualNoCase("单重")||
			sTitle.EqualNoCase("单件重")||
			sTitle.EqualNoCase("单件重量")||
			sTitle.EqualNoCase("单件\n重量"))
			return true;
		else
			return false;
	}
	else if(INDEX_NOTES==colIndex)
	{
		if( sTitle.EqualNoCase(T_NOTES)||
			sTitle.EqualNoCase("备注"))
			return true;
		else
			return false;
	}
	else if(INDEX_WIDE==colIndex)
	{
		if( sTitle.EqualNoCase(T_WIDE)||
			sTitle.EqualNoCase("宽度"))
			return true;
		else
			return false;
	}
	else if(INDEX_PARTTYPE==colIndex)
	{
		if( sTitle.EqualNoCase(T_PARTTYPE))
			return true;
		else
			return false;
	}
	else if (INDEX_MANU_NUM == colIndex)
	{
		if (sTitle.EqualNoCase(T_MANU_NUM))
			return true;
		else
			return false;
	}
	else if (INDEX_REPLACE_SPEC == colIndex)
	{
		if (sTitle.EqualNoCase(T_REPLACE_SPEC))
			return true;
		else
			return false;
	}
	else
		return false;
}
int CBomTblTitleCfg::DetectTitleRowIndex(CVariant2dArray &sheetContent)
{
	return -1;
}
//////////////////////////////////////////////////////////////////////////
// CBolTblTitleCfgSet
char CBomTblTitleCfgSet::CFG_FILE_NAME[50]="TblTitle.cfg";
CBomTblTitleCfgSet::CBomTblTitleCfgSet(const char* cfg_file_path,bool bAutoSaveToFile)
{
	m_bAutoSaveToFile=bAutoSaveToFile;
	if(cfg_file_path!=NULL)
	{
		m_sCfgFileFolder.Copy(cfg_file_path);
		if(m_sCfgFileFolder.EndWith('\\'))
			m_sCfgFilePath.Printf("%s%s",(char*)m_sCfgFileFolder,CFG_FILE_NAME);
		else
			m_sCfgFilePath.Printf("%s/%s",(char*)m_sCfgFileFolder,CFG_FILE_NAME);
		LoadFromFile();
	}
	else
		m_sCfgFileFolder.Empty();
}

CBomTblTitleCfgSet::~CBomTblTitleCfgSet()
{
	if(m_bAutoSaveToFile)
		SaveToFile();
}

void CBomTblTitleCfgSet::SaveToFile()
{
	if(m_sCfgFilePath.GetLength()<=0)
		return;
	FILE *fp=fopen(m_sCfgFilePath,"wt");
	if(fp==NULL)
		return;
	for(CBomTblTitleCfg *pCfg=recList.GetFirst();pCfg;pCfg=recList.GetNext())
		fprintf(fp,"Format=%d$%s$%s\n",pCfg->m_nStartRow,(char*)pCfg->m_sColTitleArr,(char*)pCfg->m_sColIndexArr);
	fclose(fp);
}

void CBomTblTitleCfgSet::LoadFromFile()
{
	if(m_sCfgFilePath.GetLength()<=0)
		return;
	FILE *fp=fopen(m_sCfgFilePath,"rt");
	if(fp==NULL)
		return;
	CString sLine;
	int nStartRow=0;
	char col_title[MAX_PATH]={0},col_index[MAX_PATH]={0};
	char line_txt[MAX_PATH],key_word[100];
	while(!feof(fp))
	{
		if(fgets(line_txt,MAX_PATH,fp)==NULL)
			break;
		char sText[MAX_PATH];
		strcpy(sText,line_txt);
		sLine=sText;
		sLine.Replace(' ','^');	//将空格替换为^,避免解析错误
		sLine.Replace('=',' ');
		sLine.Replace('$',' ');
		sprintf(line_txt,"%s",sLine);
		sscanf(line_txt,"%s %d %s %s",&key_word,&nStartRow,&col_title,&col_index);
		if(stricmp(key_word,"Format")!=0)
			continue;
		CBomTblTitleCfg *pCfg=recList.append();
		pCfg->m_nStartRow=nStartRow;
		if(strlen(col_title)>0)
		{
			sLine=col_title;
			sLine.Replace('^',' ');	//将^替换为空格
			pCfg->m_sColTitleArr.Copy(sLine);
		}
		if(strlen(col_index)>0)
			pCfg->m_sColIndexArr.Copy(col_index);
	}
	fclose(fp);
}
CBomTblTitleCfg *CBomTblTitleCfgSet::FindMatchCfg(CVariant2dArray &sheetContent)
{
	for(CBomTblTitleCfg *pCfg=recList.GetFirst();pCfg;pCfg=recList.GetNext())
	{
		if(pCfg->IsMatch(sheetContent))
			return pCfg;
	}
	return NULL;
}


bool CBomTblTitleCfgSet::AppendCfg(CBomTblTitleCfg &cfg)
{
	for(CBomTblTitleCfg *pCfg=recList.GetFirst();pCfg;pCfg=recList.GetNext())
	{
		if(pCfg->IsEqual(cfg))
			return false;
	}
	recList.append(cfg);
	return true;
}

bool CBomTblTitleCfgSet::AppendRecToFile(CBomTblTitleCfg &cfg,const char*cfg_file_folder)
{
	if(!cfg.IsValid())
		return false;
	CBomTblTitleCfgSet cfgSet(cfg_file_folder,true);
	return cfgSet.AppendCfg(cfg);
}

bool CBomTblTitleCfgSet::FindMatchCfg(CVariant2dArray &sheetContent,const char*cfg_file_folder,CBomTblTitleCfg *pCfg)
{
	CBomTblTitleCfgSet cfgSet(cfg_file_folder,false);
	CBomTblTitleCfg *pMatchCfg=cfgSet.FindMatchCfg(sheetContent);
	if(pMatchCfg)
	{
		if(pCfg!=NULL)
			pCfg->Clone(*pMatchCfg);
		return true;
	}
	else
		return false;
}