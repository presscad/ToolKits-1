#include "stdafx.h"
#include "XhBitMap.h"


XHBITMAP::XHBITMAP(void)
{
	memset((BITMAP*)this,0,sizeof(BITMAP));
}


XHBITMAP::~XHBITMAP(void)
{
	if(this->bmBits!=NULL)
		delete [](char*)bmBits;
	bmBits=NULL;
}
static DWORD Bit2ClrCount[9]={ 0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,0X00000100};
bool XHBITMAP::LoadImgFile(const char* szImgFile)
{
	char ciImgFileType=0;
	int len=strlen(szImgFile);
	for(int i=len-1;i>=0;i--)
	{
		if(szImgFile[i]=='.')
		{
			if(stricmp(&szImgFile[i+1],"bmp")==0)
				ciImgFileType=0;
			else if(stricmp(&szImgFile[i+1],"jpg")==0)
				ciImgFileType=1;
			else if(stricmp(&szImgFile[i+1],"png")==0)
				ciImgFileType=2;
			else
				return false;
			break;
		}
	}
	if(ciImgFileType==0)
		return LoadBmpFile(szImgFile);
	else
		return false;
}
bool XHBITMAP::LoadBmpFile(const char* szBmpFile)
{
	if(this->bmBits)
		delete [](BYTE*)bmBits;
	memset(this,0,sizeof(XHBITMAP));
	if(szBmpFile==NULL)
		return false;
	//"rt"会导致部分图片文件中字节巧合成EOF，最终导致读取失败 wjh-2019.1.2
	FILE *fp = fopen(szBmpFile,"rb");
	if(fp==NULL)
		return false;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	fread(&fileHeader,1,sizeof(BITMAPFILEHEADER),fp);
	fread(&infoHeader,1,sizeof(BITMAPINFOHEADER),fp);
	int i,nPalette = infoHeader.biClrUsed;
	if(nPalette==0&&infoHeader.biBitCount<=8)
		nPalette=Bit2ClrCount[infoHeader.biBitCount];
	RGBQUAD palette[256];//nPalette<=256;
	for(i=0;i<nPalette;i++)
	{
		fread(&palette[i].rgbBlue,1,1,fp);
		fread(&palette[i].rgbGreen,1,1,fp);
		fread(&palette[i].rgbRed,1,1,fp);
		fread(&palette[i].rgbReserved,1,1,fp);
	}
	this->bmType=0;
	this->bmPlanes=1;
	this->bmWidth=infoHeader.biWidth;
	this->bmBitsPixel=infoHeader.biBitCount;
	this->bmHeight=infoHeader.biHeight;
	int widthBits = infoHeader.biWidth*infoHeader.biBitCount;
	this->bmWidthBytes=((widthBits+31)/32)*4;
	char* fileBytes = new char[infoHeader.biHeight*this->bmWidthBytes];
	fread(fileBytes,1,infoHeader.biHeight*this->bmWidthBytes,fp);
	fclose(fp);
	//
	BYTE byteConstArr[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	int bm32WidthBytes=infoHeader.biWidth*4;
	BYTE* bit32bytes =new BYTE[infoHeader.biHeight*bm32WidthBytes];
	for(int rowi=0;rowi<infoHeader.biHeight;rowi++)
	{
		int row=infoHeader.biHeight-rowi-1;
		for(int i=0;i<infoHeader.biWidth;i++)
		{
			int colorindex=0;
			RGBQUAD crPixelColor;
			crPixelColor.rgbReserved=0;
			if(infoHeader.biBitCount==1)
			{	//单色图
				int ibyte=row*this->bmWidthBytes+i/8;
				if(fileBytes[ibyte]&byteConstArr[i%8])
					crPixelColor.rgbBlue=crPixelColor.rgbGreen=crPixelColor.rgbRed=255;
				else
					crPixelColor=palette[0];
			}
			else if(infoHeader.biBitCount==4)
			{	//16色图
				int ibyte=row*this->bmWidthBytes+i/2;
				if(i%2==0)
					colorindex=(fileBytes[ibyte]&0xf0)>>4;
				else
					colorindex=fileBytes[ibyte]&0x0f;
				crPixelColor=palette[colorindex];
			}
			else if(infoHeader.biBitCount==8)
			{	//256色图
				int ibyte=row*this->bmWidthBytes+i;
				colorindex=fileBytes[ibyte];
			}
			else if(infoHeader.biBitCount==24||infoHeader.biBitCount==32)
			{	//真彩24位或32位
				BYTE ciPixelBytes=infoHeader.biBitCount/8;
				int ibyte=row*this->bmWidthBytes+i*ciPixelBytes;
				memcpy(&crPixelColor,&fileBytes[ibyte],ciPixelBytes);
			}
			memcpy(&bit32bytes[rowi*bm32WidthBytes+i*4],&crPixelColor,4);
		}
	}

	delete []fileBytes;
	this->bmBits=bit32bytes;
	this->bmBitsPixel=32;
	this->bmWidthBytes=bm32WidthBytes;
	return true;
}
