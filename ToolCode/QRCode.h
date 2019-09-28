#pragma once
#ifndef __QR_CODE_INC_H_
#define __QR_CODE_INC_H_

#if _DEBUG
#pragma comment(lib, "..\\..\\Toolkits\\QRcode\\Debug\\QRcode.lib")
#else
#pragma comment(lib, "..\\..\\Toolkits\\QRcode\\Release\\QRcode.lib")
#endif

#include "ArrayList.h"
#include "..\QRcode\qrencode.h"

bool QRCode(const char* szSourceSring,const char* szBmpFilePath)
{
	bool bRetCode=false;
	unsigned int    unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*  pRGBData, *pSourceData, *pDestData;
	QRcode*         pQRC;
	if (pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_H, QR_MODE_8, 1))
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * 8 * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * 8;

		// Allocate pixels buffer
		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
		{
			QRcode_free(pQRC);
			return false;
		}
		// Preset to white
		memset(pRGBData, 0xff, unDataBytes);

		// Prepare bmp headers
		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER) + unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth * 8;
		kInfoHeader.biHeight = -((int)unWidth * 8);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;

		// Convert QrCode bits to bmp pixels
		pSourceData = pQRC->data;
		for (y = 0; y < unWidth; y++)
		{
			pDestData = pRGBData + unWidthAdjusted * y * 8;
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					for (l = 0; l < 8; l++)
					{
						for (n = 0; n < 8; n++)
						{
							*(pDestData + n * 3 + unWidthAdjusted * l) = 0xff;
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = 0;
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = 0;
						}
					}
				}
				pDestData += 3 * 8;
				pSourceData++;
			}
		}
		// Output the bmp file
		FILE *fp=NULL;
		if (!(fopen_s(&fp, szBmpFilePath, "wb")))
		{
			fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
			fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, fp);
			fclose(fp);
			bRetCode=true;
		}
		// Free data
		free(pRGBData);
		QRcode_free(pQRC);
	}
	return bRetCode;
}

bool QRCode(const char* szSourceSring,CBitmap &bitMap)
{
	bool bRetCode=false;
	unsigned int    unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*  pRGBData, *pSourceData, *pDestData;
	QRcode*         pQRC;
	if (pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_H, QR_MODE_8, 1))
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * 8 * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * 8;

		// Allocate pixels buffer
		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
		{
			QRcode_free(pQRC);
			return false;
		}
		// Preset to white
		memset(pRGBData, 0xff, unDataBytes);
		// Convert QrCode bits to bmp pixels
		pSourceData = pQRC->data;
		for (y = 0; y < unWidth; y++)
		{
			pDestData = pRGBData + unWidthAdjusted * y * 8;
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					for (l = 0; l < 8; l++)
					{
						for (n = 0; n < 8; n++)
						{
							*(pDestData + n * 3 + unWidthAdjusted * l) = 0x00;
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = 0x00;
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = 0x00;
						}
					}
				}
				pDestData += 3 * 8;
				pSourceData++;
			}
		}
		BITMAP image;
		image.bmType=0;
		image.bmPlanes=1;
		image.bmWidth=unWidth * 8;
		image.bmBitsPixel=24;
		image.bmHeight=((int)unWidth * 8);
		int widthBits = image.bmWidth*image.bmBitsPixel;
		image.bmWidthBytes=((widthBits+31)/32)*4;
		image.bmBits = pRGBData;
		//
		BYTE byteConstArr[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		int bm32WidthBytes=image.bmWidth*4;
		BYTE_ARRAY bit32bytes(image.bmHeight*bm32WidthBytes);
		for(int rowi=0;rowi<image.bmHeight;rowi++)
		{
			int row=rowi;//image.bmHeight-rowi-1;
			for(int i=0;i<image.bmWidth;i++)
			{
				int colorindex=0;
				RGBQUAD crPixelColor;
				crPixelColor.rgbReserved=0;
				if(image.bmBitsPixel==1)
				{	//单色图
					int ibyte=row*image.bmWidthBytes+i/8;
					if(ibyte&byteConstArr[i%8])
						crPixelColor.rgbBlue=crPixelColor.rgbGreen=crPixelColor.rgbRed=255;
				}
				/*else if(image.bmBitsPixel==4)
				{	//16色图
					int ibyte=row*image.bmWidthBytes+i/2;
					if(i%2==0)
						colorindex=(pRGBData[ibyte]&0xf0)>>4;
					else
						colorindex=pRGBData[ibyte]&0x0f;
					crPixelColor=palette[colorindex];
				}
				else if(image.bmBitsPixel==8)
				{	//256色图
					int ibyte=row*image.bmWidthBytes+i;
					colorindex=pRGBData[ibyte];
				}*/
				else if(image.bmBitsPixel==24||image.bmBitsPixel==32)
				{	//真彩24位或32位
					BYTE ciPixelBytes=image.bmBitsPixel/8;
					int ibyte=row*image.bmWidthBytes+i*ciPixelBytes;
					memcpy(&crPixelColor,&pRGBData[ibyte],ciPixelBytes);
				}
				memcpy(&bit32bytes[rowi*bm32WidthBytes+i*4],&crPixelColor,4);
			}
		}

		image.bmBitsPixel=32;
		image.bmBits=bit32bytes;
		image.bmWidthBytes=bm32WidthBytes;
		bitMap.DeleteObject();
		bRetCode=(bitMap.CreateBitmapIndirect(&image)==TRUE);
		// Free data
		free(pRGBData);
		QRcode_free(pQRC);
	}
	return bRetCode;
}
#endif