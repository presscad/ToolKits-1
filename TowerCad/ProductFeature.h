#pragma once

struct PRODUCT_FEATURE{
	DWORD dwFeatureFlag;
public:
	PRODUCT_FEATURE(DWORD featureFlag=0){dwFeatureFlag=featureFlag;}
	DWORD operator =(DWORD dwFlag){return dwFeatureFlag=dwFlag;}
	operator DWORD(){return dwFeatureFlag;}

	BYTE getBitPos() const;
	void setBitPos(BYTE bitPos);
	__declspec( property(put=setBitPos,get=getBitPos))	BYTE ciBitPos;	//0 based index

	WORD getBytePos() const;
	void setBytePos(WORD bytePos);
	__declspec( property(put=setBytePos,get=getBytePos)) WORD wiBytePos;//0 based index

	WORD getFeatureSerial() const;
	void setFeatureSerial(WORD serialFeature);
	__declspec( property(put=setFeatureSerial,get=getFeatureSerial)) WORD wiFeatureSerial;	//1 based index

	static const BYTE BIT_FEATURE  = 0;	//0~ 1
	static const BYTE CHAR_FEATURE = 1;	//0~ 63
	static const BYTE WORD_FEATURE = 2;	//0~ 16383(2^14)
	static const BYTE UINT_FEATURE = 3;	//0~ 2^30
	BYTE getDataType() const;
	void setDataType(BYTE dataType);
	__declspec( property(put=setDataType,get=getDataType)) BYTE ciDataType;	//0. 1 bit; 1. 1 byte; 2. 2 bytes;3. 4 bytes
};
union FEATURE_STATE{
private:
	DWORD _dwState;
	WORD  _wState;
	BYTE  _cbState;
	bool  _bState;
public:
	FEATURE_STATE(DWORD state=0){_dwState=state;}
	FEATURE_STATE(BYTE ciDataType,DWORD stateValue);
	DWORD operator =(DWORD dwValue){return _dwState=dwValue;}
	operator DWORD(){return _dwState;}
	BYTE getDataType() const;
	void setDataType(BYTE dataType);
	__declspec( property(put=setDataType,get=getDataType)) BYTE ciDataType;	//0. 1 bit; 1. 1 byte; 2. 2 bytes;3. 4 bytes
	DWORD getDwordState() const;
	void setDwordState(DWORD state);
	__declspec( property(put=setDwordState,get=getDwordState)) DWORD dwState;
	WORD getWordState() const;
	void setWordState(WORD state);
	__declspec( property(put=setWordState,get=getWordState)) WORD wState;
	BYTE getByteState() const;
	void setByteState(BYTE state);
	__declspec( property(put=setByteState,get=getByteState)) BYTE cbState;
	bool getBoolState() const;
	void setBoolState(bool state);
	__declspec( property(put=setBoolState,get=getBoolState)) bool bState;
};

struct FEATURE_INSTANCE
{
	BYTE hostIdentity[16];
	WORD wiFeatureSerial;
	FEATURE_STATE xFeatureState;
	FEATURE_INSTANCE(){memset(this,0,sizeof(FEATURE_INSTANCE));}
};