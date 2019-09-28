#pragma once
class XHBITMAP : public BITMAP
{
public:
	XHBITMAP(void);
	~XHBITMAP(void);
	bool LoadImgFile(const char* szImgFile);
	bool LoadBmpFile(const char* szBmpFile);
};

