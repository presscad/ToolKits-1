#pragma once
template <class TYPE> class CInterpolator
{
	int m_nSize;
	TYPE* map;
	double* header;
	TYPE& GetValue(const int i){return map[i];}
public:
	CInterpolator(void){m_nSize=0;map=NULL;header=NULL;}
	~CInterpolator(void)
	{
		if(map)
			delete []map;
		if(header)
			delete []header;
	}
	void Init(int size,double* header_data=NULL,TYPE* data=NULL)
	{
		if(map)
			delete []map;
		if(size<=0)
		{
			m_nSize=0;
			map=NULL;
		}
		else
		{
			m_nSize=size;
			map=new TYPE[size];
			if(data)
				memcpy(map,data,sizeof(TYPE)*size);
			else
				memset(map,0,sizeof(TYPE)*size);
			if(header)
				delete []header;
			header=new double[size];
			if(header_data)
				memcpy(header,header_data,sizeof(double)*size);
			else
				memset(header,0,sizeof(double)*size);
		}
	}
	void InitMap(TYPE* mapdata){
		memcpy(map,mapdata,m_nSize*sizeof(TYPE));
	}
	double* Header(){return header;}
	TYPE& operator [](const int i){return map[i];}
	TYPE Interpolate(double header_val){
		for(int i=0;i<m_nSize;i++)
		{
			if(header_val<header[i])
			{
				if(i==0)	//小于标题初始元素
					return map[0];
				else
					return map[i-1]+(map[i]-map[i-1])*(header_val-header[i-1])/(header[i]-header[i-1]);
			}
		}
		return map[m_nSize-1];
	}
};
template <class TYPE> class CFittingMap
{	//二维线性插值拟合
	struct GRIDCELL{
		int row,col;
		double rowScale,colScale;
	};
	struct DATAROW{
		TYPE* row;
		DATAROW(TYPE* row_addr=NULL){row=row_addr;}
		TYPE& operator[](const int col){return row[col];}
	};
	int m_nRow,m_nColumn;
	TYPE* map;
	double* row_header;
	double* col_header;
	GRIDCELL LocateGrid(double row_val,double col_val)
	{
		int i;
		GRIDCELL cell;
		for(i=0;i<m_nColumn;i++)
		{
			if(row_val<=row_header[i])
			{
				if(i==0)
				{	//小于行标题初始元素
					cell.col=0;
					cell.rowScale=0;
				}
				else
				{
					cell.col=i-1;
					cell.rowScale=(row_val-row_header[i-1])/(row_header[i]-row_header[i-1]);
				}
				break;
			}
		}
		if(i==m_nColumn)
		{
			cell.col=m_nColumn-1;
			cell.rowScale=0;
		}
		for(i=0;i<m_nRow;i++)
		{
			if(col_val<=col_header[i])
			{
				if(i==0)
				{	//小于列标题初始元素
					cell.row=0;
					cell.colScale=0;
				}
				else
				{
					cell.row=i-1;
					cell.colScale=(col_val-col_header[i-1])/(col_header[i]-col_header[i-1]);
				}
				break;
			}
		}
		if(i==m_nRow)
		{
			cell.row=m_nRow-1;
			cell.colScale=0;
		}
		return cell;
	}
	TYPE& MapValue(const int row,const int col){
		return datarow_arr[row][col];
	}
	DATAROW* datarow_arr; 
public:
	DATAROW operator [](const int row){return datarow_arr[row];}
	void InitHeader(double* rowheader,double* colheader)
	{
		memcpy(row_header,rowheader,m_nColumn*sizeof(double));
		memcpy(col_header,colheader,m_nRow*sizeof(double));
	}
	void InitMap(TYPE* mapdata){
		memcpy(map,mapdata,m_nRow*m_nColumn*sizeof(TYPE));
	}
	double* RowHeader(){return row_header;}
	double* ColHeader(){return col_header;}
	CFittingMap(void){
		m_nRow=m_nColumn=0;
		map=NULL;
		row_header=col_header=NULL;
		datarow_arr=NULL;
	}
	~CFittingMap(void){
		if(map)
			delete []map;
		if(row_header)
			delete []row_header;
		if(col_header)
			delete []col_header;
		if(datarow_arr)
			delete []datarow_arr;
	}
	void Init(int rows,int cols,TYPE* data=NULL)
	{
		if(map)
			delete []map;
		if(rows<=0||cols<=0)
			map=NULL;
		else
		{
			m_nRow=rows;
			m_nColumn=cols;
			map=new TYPE[rows*cols];
			if(data)
				memcpy(map,data,sizeof(TYPE)*rows*cols);
			else
				memset(map,0,8*rows*cols);
			if(datarow_arr)
				delete []datarow_arr;
			if(row_header)
				delete []row_header;
			row_header=new TYPE[cols];
			memset(row_header,0,8*cols);
			if(col_header)
				delete []col_header;
			col_header=new TYPE[rows];
			memset(col_header,0,8*rows);
			datarow_arr=new DATAROW[rows];
			for(int i=0;i<rows;i++)
				datarow_arr[i].row=&map[i*m_nColumn];
		}
	}
	TYPE Interpolate(double row_val,double col_val)
	{
		GRIDCELL cell=LocateGrid(row_val,col_val);
		TYPE v[2][2];
		int row1=cell.row+1,col1=cell.col+1;
		if(row1>=m_nRow)
			row1=cell.row;
		if(col1>=m_nColumn)
			col1=cell.col;
		v[0][0]=MapValue(cell.row,cell.col);
		v[0][1]=MapValue(cell.row,col1);
		v[1][0]=MapValue(row1,cell.col  );
		v[1][1]=MapValue(row1,col1);
		TYPE v_up  =v[0][0]+cell.rowScale*(v[0][1]-v[0][0]);
		TYPE v_down=v[1][0]+cell.rowScale*(v[1][1]-v[1][0]);
		return v_up+cell.colScale*(v_down-v_up);
	}
	const TYPE* getMapData(){return map;}
	int getRowCount(){return m_nRow;}
	int getColCount(){return m_nColumn;}
	_declspec(property(get=getRowCount)) int RowCount;
	_declspec(property(get=getColCount)) int ColCount;
	_declspec(property(get=getMapData)) const TYPE* MapData;
};
typedef CFittingMap<double> C2dInterpolator;