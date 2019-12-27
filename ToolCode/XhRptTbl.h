#ifndef __XHRPTTBL_H
#define __XHRPTTBL_H

#include "f_ent.h"
#include "Buffer.h"
#include "XhCharString.h"

struct XHTBLDEF {
	//��ʽ��Ķ�λģʽ
	static const BYTE TOP_LEFT = 0;		//���Ͻ�
	static const BYTE TOP_RIGHT = 1;	//���Ͻ�
	static const BYTE BOTTOM_LEFT = 2;	//���½�
	static const BYTE BOTTOM_RIGHT = 3;	//���½�
	//�ı�����ģʽ
	static const BYTE TEXTALIGN_RIGHT_CENTER  = 0;	//������
	static const BYTE TEXTALIGN_MIDDLE_CENTER = 1;	//������
	static const BYTE TEXTALIGN_BOTTOM_RIGHT  = 2;	//���½�
	static const BYTE TEXTALIGN_TOP_LEFT	  = 3;	//���½�
	static const BYTE TEXTALIGN_LEFT_CENTER	  = 4;	//������
	//���Ԫ��(��ʾ)ģʽ
	static const BYTE STRING_GRID		= 0;	//��ʾ�ַ���
	static const BYTE GRAPH_BLOCK_GRID	= 1;	//��ʾͼ��
	static const BYTE PIC_BLOCK_GRID	= 2;	//��ʾOLEͼƬ
	//��Ԫ����������
	static const BYTE GRID_VT_EMPTY	= 0;	// δָ��
	static const BYTE GRID_VT_NULL	= 1;	// �գ��ݱ�����
	static const BYTE GRID_VT_I2	= 2;	// 2�ֽڴ���������
	static const BYTE GRID_VT_I4	= 3;	// 4�ֽڴ���������
	static const BYTE GRID_VT_R4	= 4;	// 4�ֽڸ�����
	static const BYTE GRID_VT_R8	= 5;	// 8�ֽڸ�����
	static const BYTE GRID_VT_DATE	= 6;	// ����
	static const BYTE GRID_VT_VARSTR= 7;	// �ַ���
	static const BYTE GRID_VT_POINTER = 8;	// ָ��
	static const BYTE GRID_VT_BOOL	= 9;	// ������; TRUE=1, FALSE=0
	static const BYTE GRID_VT_NSTR	=10;	// �̶������ַ���
};
class XHVAR{
public:
	union{
		BOOL		bVal;		// GRID_VT_BOOL
		__int16		iVal;		// GRID_VT_I2
		__int32		lVal;		// GRID_VT_I4
		float		fltVal;		// GRID_VT_R4
		double		dblVal;		// GRID_VT_R8
		//DATE		date;		// GRID_VT_DATE
		char		szStrBuf8[8];//GRID_VT_NSTR
		char*		sVal;		// GRID_VT_STRING
		void*		pVal;		// GRID_VT_POINTER
	};
	BYTE ciType;
public:
	XHVAR();
	XHVAR(const XHVAR& srcObj);
	~XHVAR();
	//
	void ClearVariant();
	bool SetGridString(const char *string,BOOL bForceToString=true);
	void Import(FILE* fp);
	void Export(FILE* fp);
};
class CXhGrid
{
	static float PREFER_TEXT_SIZE;
	bool m_bSelectStatus;	//����������������ݵĻ��������Ժܿ������� wjh-2019.10.10
public:
	//���Ԫ��������
	struct FONTSTYLE{
	public:
		//�����ʽ
		long lfHeight; 
		long lfWidth; 
		long lfEscapement; 
		long lfOrientation; 
		long lfWeight; 
		BYTE lfItalic; 
		BYTE lfUnderline; 
		BYTE lfStrikeOut; 
		BYTE lfCharSet; 
		BYTE lfOutPrecision; 
		BYTE lfClipPrecision; 
		BYTE lfQuality; 
		BYTE lfPitchAndFamily; 
		TCHAR* lfFaceName; 
		COLORREF crTextColor;
		COLORREF crBackground;
		//���ֶ��뷽ʽ
		UINT m_nTextAlignFlags;
		//�����ڱ߾�
		float m_fLeftDist,m_fRightDist,m_fTopDist,m_fBottomDist;
	public:
		FONTSTYLE();
		FONTSTYLE(const FONTSTYLE& fontstyle);
		FONTSTYLE(const LOGFONT& fontstyle);
		~FONTSTYLE();
		void Import(FILE* fp);
		void Export(FILE* fp);
		//BOOL operator ==(const CFontStyle& fontstyle)const;
		//BOOL operator !=(const CFontStyle& fontstyle)const;
		operator LOGFONT()const;
		int SetFaceName(const TCHAR *face_name);
		void CopyFromLOGFONT(const LOGFONT &logfont);
		void CopyToLOGFONT(LOGFONT &logfont)const;
	};
	struct SIMPLE{
		float sfTextSize;
		float sfTextAngle;			//������ת����
		BOOL bBold;					//�Ƿ�Ӵ�
	}simfont;
	struct{
		bool bTitle;		//���ڱ�ʶ�Ƿ�Ϊ���Ԫ���еı�����
		long type_id;		//���ڱ�ʶ��Ԫ�����������
		long ext_feature;	//������չ,�Ժ��������
	}mark;
	bool m_bVGridVirtual;	//��ֱ�ϲ��鵥Ԫ
	bool m_bHGridVirtual;	//ˮƽ�ϲ��鵥Ԫ
	int iRow,iColumn;	//����λ�ú�
	BYTE ciFontFormat;	//0.�򵥸�ʽ����;1.Windows���Ӹ�ʽ����
	BYTE grid_mode;		//GRID_MODE���Ԫ��(��ʾ)ģʽ
	XHVAR data;			//����е���������
	FONTSTYLE xFontStyle;
	DWORD uiTagFeature;	//�������һЩ����(���û���)
	UINT uidCellValSource;	//���ڿ��������趨����������ݵĵ�Ԫ�������������Id
public:
	CXhGrid();
	CXhGrid(const CXhGrid& grid);
	~CXhGrid();
	//
	bool get_blStateSelected();
	bool set_blStateSelected(bool blSelected);
	__declspec(property(put = set_blStateSelected, get = get_blStateSelected))bool blStateSelected;
public:
	bool ReadGridFromStream(BUFFER_IO* pIO);
	bool WriteGridToStream(BUFFER_IO* pIO);
	void Import(FILE* fp);
	void Export(FILE* fp);
	void Copy(const CXhGrid& grid);
};

class CXhGridMap
{
	CXhGrid *grid_map;		//���洢��
	int m_nRow,m_nColumn;	//��������������
public:
	CXhGridMap();
	~CXhGridMap();
public:
	void Destroy();
	BOOL Create(int nRow,int nColumn);
	int GetRowsCount() const{return m_nRow;}
	int GetColumnsCount() const{return m_nColumn;}
	BOOL InsertRow(int iRow);
	BOOL DelRow(int iRow);
	BOOL InsertColumn(int iColumn);
	BOOL DelColumn(int iColumn);
	CXhGrid* GetGridAt(int iRow,int iColumn);					//�������кŻ��ָ����Ԫ��ָ��
	BOOL SetGrid(int iRow, int iColumn,const CXhGrid &grid);	//��䵥Ԫ������
};
//CXhTable
class CXhTable
{
	long m_iSerial;
	BYTE ciLocaStyle;			//TBL_LOCATE_STYLE����, ���Ͻ�;���½�;���½�;���Ͻ�
	COORD3D xLocation;			//��(��λ)������
	double *rows_arr_high;		//�и��б�
	double *columns_arr_wide;	//�п��б�
	CXhGridMap grid_map;		//���洢��
public:
	CXhChar50 m_sTblName;		//�������
	//
	int get_ID() const { return m_iSerial; }
	__declspec(property(get = get_ID)) int ID;					//���
	int get_nRowsCount() const {return grid_map.GetRowsCount();}
	__declspec(property(get = get_nRowsCount)) int nRowsCount;	//����
	int get_nColsCount() const {return grid_map.GetColumnsCount();}
	__declspec(property(get = get_nColsCount)) int nColsCount;	//����
private:
	double getRowLinePos(BOOL bUpToDown, int iRow);		//���ָ�����߾����ʽ����Ӧ��Ե��λ�þ���
	double getColumnLinePos(BOOL bLToR, int iColumn);	//���ָ�����߾����ʽ����Ӧ��Ե��λ�þ���
public:
	CXhTable(long iSerial = 0);
	~CXhTable();
	//������
	void Destroy();							//���ٱ��,�ͷ��ڴ�
	BOOL Create(int nRow, int nCol, double high = 6, double width = 16);
	void CopyFrom(CXhTable *pXhRptTbl);	//�������
	void CopyFrom(CXhTable *pXhRptTbl,int iRemoveStart,int iRemoveEnd);
	void SetBasePos(const double* pos2d);	//�趨��λ���ά����
	void GetBasePos(double* pos2d);			//��ø�ʽ��Ķ�λ���ά����
	void SetLocateStyle(BYTE ciStyle);		//�趨���λ��ʽ
	BYTE GetLocateStyle();					//��ø�ʽ��Ķ�λ��ʽ
	void SetFontSize(float fontSize=2);		//�趨��ʽ�������߶�
	double GetTableWide();					//������Ÿ�ʽ����ܿ�
	double GetTableHeight();				//������Ÿ�ʽ����ܸ�
	BOOL GetTblRect(f2dRect *tbl_rect);		//������Ÿ�ʽ�������
	//���в���
	BOOL DelRow(int iRow);
	BOOL DelCol(int iCol);
	BOOL InsertRow(int iRow, double high = 0);
	BOOL InsertCol(int iCol, double wide = 0);
	BOOL SetRowFontSize(int iRow, float fontSize,bool bBold=false);
	BOOL SetRowColor(int iRow, COLORREF crFontColor, COLORREF crBackColor=RGB(255,255,255));
	BOOL SetRowHigh(int iRow, double high);					//�趨ָ���и�
	BOOL SetColWide(int iCol, double wide);					//�趨ָ���п�
	BOOL GetRowHigh(int iRow, double *high);				//���ָ���и�
	BOOL GetColWide(int iCol, double *wide);				//���ָ���п�
	BOOL GetRowLineStart(int iRow, double* pStartPos2d);	//���ָ�����ߵ���ʼλ��
	BOOL GetRowLineEnd(int iRow, double* pEndPos2d);		//���ָ�����ߵ���ֹλ��
	BOOL GetColLineStart(int iCol, double* pStartPos2d);	//���ָ�����ߵ���ʼλ��
	BOOL GetColLineEnd(int iCol, double* pEndPos2d);		//���ָ�����ߵ���ֹλ��
	//��Ԫ�����
	CXhGrid* GetGridAt(int iRow, int iCol);					//�������кŻ��ָ����Ԫ��ָ��
	BOOL CleanGrid(int iRowS, int iRowE);
	BOOL IsValidGrid(int iRow = 1, int iCol = 1);		//���������ű�ʶ��Ч���ж�
	BOOL SetGrid(int iRow, int iCol, char *string, BOOL bForceToString = TRUE);	//��䵥Ԫ������
	BOOL MergeGrid(int iCellRowS, int iCellColS, int iCellRowE, int iCellColE, BYTE cbMergeFlag = 0);
	BOOL GetGridRect(int iRow, int iCol, f2dRect &rect);	//���ָ����Ԫ��λ�þ���
	BOOL GetGridSize(int iRow, int iCol, double *height = NULL, double *wide = NULL);
	BOOL GetGridRange(int &start_i, int &start_j, int *end_i, int *end_j);		//���ָ����Ԫ���С(�߿�)
	BOOL GetGridString(int iRow, int iCol, char *string);	//���ָ����Ԫ��������ַ���
	//��������
	void SelectAll();
	int ReleaseSnapStatus();
	int GetSelectedCounts(); 
	BOOL SnapGridPos(double x, double y, double *pSnapPos2d, double scope = 1.0);
	CXhGrid * SnapGrid(double x,double y,int *iRow=NULL,int *iCol=NULL);
	CXhGrid *GetFirstSelectedGrid(CXhTable **pXhRptTbl = NULL, int *pos_i = NULL, int *pos_j = NULL); 
	//��λת��
	static double TransRowHightToCad(double fRowHight);
	static double TransRowHightToXls(double fRowHight);
	static double TransColWidthToCad(double fColWidth);
	static double TransColWidthToXls(double fColWidth);
public:	//������ݵ��뵼��
	BOOL WriteBasicTbl(BUFFER_IO *pIO);
	BOOL ReadBasicTbl(BUFFER_IO *pIO);
	BOOL ImportTxt(char *filename);				//���ı��ļ������ʽ��
	BOOL ExportTxt(char *filename);				//���ı��ļ�������ʽ��
	BOOL ImportTxt(FILE *fp);					//���ı��ļ������ʽ��
	BOOL ExportTxt(FILE *fp);					//���ı��ļ�������ʽ��
};
#endif
