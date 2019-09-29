#include "StdAfx.h"
#include "DimStyle.h"

//////////////////////////////////////////////////////////////////////////
//
TMADRAWING_TEXT_STYLE TextStyleTable::hzfs;
void TextStyle(AcDbTextStyleTableRecord *pTextStyleTblRec, char *style_name)
{
#ifdef _ARX_2007
	//����������ʽ���¼��
	pTextStyleTblRec->setName((ACHAR*)_bstr_t(style_name));
	pTextStyleTblRec->setFileName(L"simplex");
#else
	//����������ʽ���¼��
	pTextStyleTblRec->setName(style_name);
	pTextStyleTblRec->setFileName("simplex");
	//���ô������ļ���
#ifndef _MAPTMA_GRX
	pTextStyleTblRec->setBigFontFileName("gbhzfs.shx");
#else
	pTextStyleTblRec->setBigFontFileName("CHINA.shx");
#endif
#endif
	//���������Ч������ȱ���
	pTextStyleTblRec->setXScale(0.7);
}
void GetTextStyleId(AcDbObjectId &TextStyleId, char *textStyle/*=NULL*/, bool init_style/*=false*/)
{
	char style[100];
	AcDbTextStyleTable *pTextStyleTbl;//����������ʽ��ָ��
	AcDbTextStyleTableRecord *pTextStyleTblRec;//����������ʽ��¼ָ��
	//��д��ʽ��������ʽ�����ָ����������ʽ���ָ��
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);
	//����������ʽ���¼
	//���������ʽ���л�û��ָ����ʽ,�򴴽���������ʽ
	//��������ʽID,�������ָ����ʽ,�򷵻�������ʽ��ID
	if (textStyle == NULL)
		strcpy(style, "Standard");
	else
		strcpy(style, textStyle);
#ifdef _ARX_2007
	if (pTextStyleTbl->getAt((ACHAR*)_bstr_t(style), TextStyleId) != Acad::eOk)
	{
		Acad::ErrorStatus rescode;
		pTextStyleTblRec = new AcDbTextStyleTableRecord;
		TextStyle(pTextStyleTblRec, _bstr_t(style));
		pTextStyleTblRec->setTextSize(0);		//���������С
		rescode = pTextStyleTbl->add(TextStyleId, pTextStyleTblRec);//��������ʽ����д��������ʽ���¼
		pTextStyleTbl->close();		//�ر�������ʽ���¼
		pTextStyleTblRec->close();	//�ر�������ʽ��
		if (rescode == Acad::eOutOfMemory)
			AfxMessageBox("�ڴ����");
		else if (rescode == Acad::eDuplicateRecordName)
			AfxMessageBox("��ʽ����");
		else if (rescode == Acad::eNoDatabase)
			AfxMessageBox("û�����ݿ�");
	}
	else
	{
		pTextStyleTbl->close();
		acdbOpenObject(pTextStyleTblRec, TextStyleId, AcDb::kForWrite);
		if (init_style)
			TextStyle(pTextStyleTblRec, _bstr_t(style));
		pTextStyleTblRec->setTextSize(0);
		//�ر�������ʽ��
		pTextStyleTblRec->close();
	}
#else
	if (pTextStyleTbl->getAt(style, TextStyleId) != Acad::eOk)
	{
		Acad::ErrorStatus rescode;
		pTextStyleTblRec = new AcDbTextStyleTableRecord;
		TextStyle(pTextStyleTblRec, style);
		pTextStyleTblRec->setTextSize(0);		//���������С
		rescode = pTextStyleTbl->add(TextStyleId, pTextStyleTblRec);	//��������ʽ����д��������ʽ���¼
		pTextStyleTbl->close();		//�ر�������ʽ���¼
		pTextStyleTblRec->close();	//�ر�������ʽ��
		if (rescode == Acad::eOutOfMemory)
			AfxMessageBox("�ڴ����");
		else if (rescode == Acad::eDuplicateRecordName)
			AfxMessageBox("��ʽ����");
		else if (rescode == Acad::eNoDatabase)
			AfxMessageBox("û�����ݿ�");
	}
	else
	{
		pTextStyleTbl->close();
		acdbOpenObject(pTextStyleTblRec, TextStyleId, AcDb::kForWrite);
		if (init_style)
			TextStyle(pTextStyleTblRec, style);
		pTextStyleTblRec->setTextSize(0);
		//�ر�������ʽ��
		pTextStyleTblRec->close();
	}
#endif
	acdbHostApplicationServices()->workingDatabase()->setTextstyle(TextStyleId);
}
void TextStyleTable::InitTextStyleTable()
{
	TextStyleTable::hzfs.bigFontName.Copy("gbhzfs.shx");
	TextStyleTable::hzfs.textStyleName.Copy("standard");
	GetTextStyleId(TextStyleTable::hzfs.textStyleId, "standard", true);	//��ʼ���ı���ʽ
}

//////////////////////////////////////////////////////////////////////////
//
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle2;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle3;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle4;		//���ַ��ڳߴ������ wht 15-04-28
TMADRAWING_DIM_STYLE DimStyleTable::dimAngleStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimRadiusStyle;
//
double TMADRAWING_DIM_STYLE::s_fDimArrowSize=1.5;
double TMADRAWING_DIM_STYLE::s_fDimTextSize=2.5;
double TMADRAWING_DIM_STYLE::s_fDimGap=0.75;
char TMADRAWING_DIM_STYLE::s_sArrowType[50] = "_OBLIQUE";
double TMADRAWING_DIM_STYLE::s_fDimAngleTextSize = 2;
TMADRAWING_DIM_STYLE::TMADRAWING_DIM_STYLE()
{
	arrowSize = gap = 0;
	m_fInnerSetTextSize = 0;
}
void TMADRAWING_DIM_STYLE::InitDimStyle(double text_height/*=0*/, double arrow_size/*=0*/, double text_gap/*=0*/)
{
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTableRecord *pDimStyleTblRec = NULL;
	acdbOpenObject(pDimStyleTblRec, dimStyleId, AcDb::kForWrite);
	if (pDimStyleTblRec)
	{
		if (arrow_size == 0)
			arrowSize = s_fDimArrowSize;
		else
			arrowSize = arrow_size;
		m_fInnerSetTextSize = text_height;
		if (m_fInnerSetTextSize == 0)
			m_fInnerSetTextSize = s_fDimTextSize;
		if (text_gap == 0)
			gap = 0.25*m_fInnerSetTextSize;
		else
			gap = text_gap;
		pDimStyleTblRec->setDimasz(arrowSize);	//��ͷ����Ϊ
		pDimStyleTblRec->setDimtxt(m_fInnerSetTextSize);	//�ߴ����ָ߶�Ϊ
		pDimStyleTblRec->setDimexe(0.5*m_fInnerSetTextSize);	//�ߴ�����ڱ�ע�Ϸ���������(�ߴ���߷�Χ)Ӧ����Ϊ��ע�ָߵ�һ�� wht 10-01-10
		pDimStyleTblRec->setDimgap(gap);	//��ע�������ּ��ӦΪ0.5�ߴ���������	wht 10-01-10
		pDimStyleTblRec->close();
	}
}
double TMADRAWING_DIM_STYLE::get_Gap() { return gap; }
double TMADRAWING_DIM_STYLE::set_Gap(double dimgap) { return gap = dimgap; }
double TMADRAWING_DIM_STYLE::CalGap(double text_height)
{
	return max(s_fDimGap, 0.25*text_height);
}

//////////////////////////////////////////////////////////////////////////
//
void DimStyleTable::InitDimStyle()
{
	DimStyleTable::dimStyle = DimStyleTable::GetDimStyle("standard", TMADRAWING_DIM_STYLE::s_fDimTextSize);			//Ĭ�ϱ�ע��ʽ����ע�ߴ�����ڳߴ���֮��
	DimStyleTable::dimStyle2 = DimStyleTable::GetDimStyle2("dimStyle2", TMADRAWING_DIM_STYLE::s_fDimTextSize);		//���ڻ��ƽǸּ�ͼ��Сͼ
	DimStyleTable::dimStyle3 = DimStyleTable::GetDimStyle("dimStyle3", TMADRAWING_DIM_STYLE::s_fDimTextSize, FALSE);//�����ߴ��עǿ�Ʒ��ڳߴ���֮�䣬��Ҫ���ڱ�ע�Ǹ������� wht 11-07-12	
	DimStyleTable::dimStyle4 = DimStyleTable::GetDimStyle("dimStyle4", TMADRAWING_DIM_STYLE::s_fDimTextSize, TRUE, 2);//���ַ��ڳߴ������ wht 15-04-28
	DimStyleTable::dimAngleStyle = DimStyleTable::GetDimAngleStyle("dimAngleStyle", TMADRAWING_DIM_STYLE::s_fDimAngleTextSize);
	DimStyleTable::dimRadiusStyle = DimStyleTable::GetDimRadiusStyle("dimRadiusStyle", TMADRAWING_DIM_STYLE::s_fDimTextSize);
}


/*DIMBLKϵͳ����
""				ʵ�ıպ�
"_DOT"			��
"_DOTSMALL"		С��
"_DOTBLANK"		���ĵ�
"_ORIGIN"		ԭ����
"_ORIGIN2"		ԭ���� 2
"_OPEN"			��
"_OPEN90"		ֱ��
"_OPEN30"		30 �Ƚ�
"_CLOSED"		�պ�
"_SMALL"		����С��
"_NONE"			��
"_OBLIQUE"		��б
"_BOXFILLED"	ʵ�Ŀ�
"_BOXBLANK"		��
"_CLOSEDBLANK"	���ıպ�
"_DATUMFILLED"	ʵ�Ļ�׼������
"_DATUMBLANK"	��׼������
"_INTEGRAL"		�������
"_ARCHTICK"		�������
*/
//���óߴ������������
void setDimVar(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height/*=2.5*/,
	BOOL bInside/*=TRUE*/, BYTE cVerticalPosType/*=1*/)
{
	pDimStyleTblRec->setDimasz(TMADRAWING_DIM_STYLE::s_fDimArrowSize);	//��ͷ����Ϊ
#ifdef _ARX_2007
	//pDimStyleTblRec ->setDimblk(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//�û������ü�ͷ���� wht 10-08-02
	pDimStyleTblRec->setDimblk((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk1((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk2((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//��ͷ����ʽ"/"
#else
	//pDimStyleTblRec ->setDimblk("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2("_ARCHTICK");	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk(TMADRAWING_DIM_STYLE::s_sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk1(TMADRAWING_DIM_STYLE::s_sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk2(TMADRAWING_DIM_STYLE::s_sArrowType);	//��ͷ����ʽ"/"
#endif
	//pDimStyleTblRec ->setDimexe(2);	//�ߴ�����ڱ�ע�Ϸ���������
	//pDimStyleTblRec ->setDimgap(1);	//��ע�������ּ��Ϊ1
	pDimStyleTblRec->setDimtmove(2);	//0.�ߴ����������ƶ�;1.�ƶ�����ʱ�������;2.�ƶ�����ʱ���������
	pDimStyleTblRec->setDimexe(0.5*txt_height);	//�ߴ�����ڱ�ע�Ϸ���������Ӧ����Ϊ��ע�ָߵ�һ�� wht 10-01-10
	double fDimGap = max(TMADRAWING_DIM_STYLE::s_fDimGap, 0.25*txt_height); //wht 10-08-02
	pDimStyleTblRec->setDimgap(fDimGap);			//��ע�������ּ��ӦΪ0.5�ߴ���������	wht 10-01-10
	pDimStyleTblRec->setDimexo(0);	//�ߴ�������ƫ����Ϊ0
	pDimStyleTblRec->setDimdli(6);	//��ע�߼��Ϊ6
	pDimStyleTblRec->setDimtih(0);	//�ߴ�����ڲ�ĵ�����ˮƽ����
	pDimStyleTblRec->setDimtoh(0);	//�������ˮƽ����
	pDimStyleTblRec->setDimtxt(txt_height);	//�ߴ����ָ߶�Ϊ
	pDimStyleTblRec->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//�趨������ʽ
	pDimStyleTblRec->setDimtad(cVerticalPosType);	//0.���� 1.����λ�ڱ�ע���Ϸ� 2.�ⲿ 3.JIS 4.�·�
	pDimStyleTblRec->setDimdec(2);	//��עС��λ��
	pDimStyleTblRec->setDimtdec(1);//��ע����λ��
	if (bInside)	//���ݴ�����������Ƿ񽫳ߴ��ע�����ڳߴ���֮�� wht 11-07-12
		pDimStyleTblRec->setDimtix(Adesk::kTrue);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	else
		pDimStyleTblRec->setDimtix(Adesk::kFalse);	//��ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	pDimStyleTblRec->setDimtofl(1);//ǿ���ڳߴ�����ڻ���ע��
}
void setDimVar2(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height)
{
	pDimStyleTblRec->setDimasz(TMADRAWING_DIM_STYLE::s_fDimArrowSize);	//��ͷ����Ϊ
#ifdef _ARX_2007
	//pDimStyleTblRec ->setDimblk(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2(L"_ARCHTICK");	//��ͷ����ʽ"/"
	//�û������ü�ͷ���� wht 10-08-02
	pDimStyleTblRec->setDimblk((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk1((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk2((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//��ͷ����ʽ"/"
#else
	//pDimStyleTblRec ->setDimblk("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk1("_ARCHTICK");	//��ͷ����ʽ"/"
	//pDimStyleTblRec ->setDimblk2("_ARCHTICK");	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk(TMADRAWING_DIM_STYLE::s_sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk1(TMADRAWING_DIM_STYLE::s_sArrowType);	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk2(TMADRAWING_DIM_STYLE::s_sArrowType);	//��ͷ����ʽ"/"
#endif
	pDimStyleTblRec->setDimtmove(2);	//0.�ߴ����������ƶ�;1.�ƶ�����ʱ�������;2.�ƶ�����ʱ���������
	pDimStyleTblRec->setDimexe(0);	//�ߴ�����ڱ�ע�Ϸ���������
	pDimStyleTblRec->setDimexo(0);	//�ߴ�������ƫ����Ϊ0
	pDimStyleTblRec->setDimdli(2);	//��ע�߼��Ϊ6
	pDimStyleTblRec->setDimtad(1);	//����λ�ڱ�ע���Ϸ�
	pDimStyleTblRec->setDimtih(0);	//�ߴ�����ڲ�ĵ�����ˮƽ����
	pDimStyleTblRec->setDimtoh(0);	//�������ˮƽ����
	pDimStyleTblRec->setDimtxt(txt_height);	//�ߴ����ָ߶�Ϊ
	pDimStyleTblRec->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//�趨������ʽ
	pDimStyleTblRec->setDimgap(1);	//��ע�������ּ��Ϊ1
	pDimStyleTblRec->setDimdec(2);	//��עС��λ��
	pDimStyleTblRec->setDimtdec(1);//��ע����λ��
	pDimStyleTblRec->setDimtix(Adesk::kTrue);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	pDimStyleTblRec->setDimtofl(1);//ǿ���ڳߴ�����ڻ���ע��
}
//���óߴ������������
void setDimAngleVar(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height)
{
	pDimStyleTblRec->setDimasz(TMADRAWING_DIM_STYLE::s_fDimArrowSize);	//��ͷ����Ϊ
#ifdef _ARX_2007
	pDimStyleTblRec->setDimblk(L"");	//��ͷ����ʽ"/"_OPEN30
	pDimStyleTblRec->setDimblk1(L"");	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk2(L"");	//��ͷ����ʽ"/"
#else
	pDimStyleTblRec->setDimblk("");	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk1("");	//��ͷ����ʽ"/"
	pDimStyleTblRec->setDimblk2("");	//��ͷ����ʽ"/"
#endif
	//pDimStyleTblRec ->setDimexe(2);	//�ߴ�����ڱ�ע�Ϸ���������
	//pDimStyleTblRec ->setDimgap(1);	//��ע�������ּ��Ϊ1
	pDimStyleTblRec->setDimexe(0.5*txt_height);	//�ߴ�����ڱ�ע�Ϸ���������Ӧ����Ϊ��ע�ָߵ�һ�� wht 10-01-10
	pDimStyleTblRec->setDimgap(0.25*txt_height);	//��ע�������ּ��ӦΪ0.5�ߴ���������	wht 10-01-10
	pDimStyleTblRec->setDimexo(0);	//�ߴ�������ƫ����Ϊ0
	pDimStyleTblRec->setDimdli(6);	//��ע�߼��Ϊ6
	pDimStyleTblRec->setDimtad(2);	//����λ�ڱ�ע���Ϸ�0:����;1:�Ϸ�;2:�ⲿ
	pDimStyleTblRec->setDimtih(0);	//�ߴ�����ڲ�ĵ�����ˮƽ����
	pDimStyleTblRec->setDimtoh(0);	//�������ˮƽ����
	pDimStyleTblRec->setDimtxt(txt_height);	//�ߴ����ָ߶�Ϊ
	pDimStyleTblRec->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//�趨������ʽ
	pDimStyleTblRec->setDimadec(1);	//��עС��λ��
	pDimStyleTblRec->setDimtdec(1);//��ע����λ��
	pDimStyleTblRec->setDimtix(Adesk::kTrue);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	pDimStyleTblRec->setDimtofl(1);//ǿ���ڳߴ�����ڻ���ע��
}
TMADRAWING_DIM_STYLE DimStyleTable::GetDimStyle(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/, BOOL bInside/*=TRUE*/, BYTE cVerticalPosType/*=1*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.dimStyleName.Copy(dimstyle);
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t(dimStyle.dimStyleName));
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#else
	if (pDimStyleTbl->getAt((char*)dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	pDimStyleTbl->close();
	setDimVar(pDimStyleTblRec, txt_height, bInside, cVerticalPosType);
	pDimStyleTblRec->close();
	return dimStyle;
	}
TMADRAWING_DIM_STYLE DimStyleTable::GetDimStyle2(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ2");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t(dimStyle.dimStyleName));
#else
	if (pDimStyleTbl->getAt((char*)dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	setDimVar2(pDimStyleTblRec, txt_height);
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}
TMADRAWING_DIM_STYLE DimStyleTable::GetDimAngleStyle(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ3");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName));
#else
	if (pDimStyleTbl->getAt(dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	setDimAngleVar(pDimStyleTblRec, txt_height);
	//pDimStyleTblRec->setDimfit(1);	//���ð뾶��ע��ʽ
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}

TMADRAWING_DIM_STYLE DimStyleTable::GetDimRadiusStyle(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ4");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//��д��ʽ�򿪳ߴ���ʽ��
	AcDbDimStyleTable *pDimStyleTbl;//����ߴ��עָ��
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName));
#else
	if (pDimStyleTbl->getAt(dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//��ߴ��ע��ʽ����д��ߴ��ע��ʽ���¼
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	setDimAngleVar(pDimStyleTblRec, txt_height);
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}