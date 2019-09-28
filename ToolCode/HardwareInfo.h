#pragma once
#include "XhCharString.h"

//1.获取CPU ID
CXhChar200 GetCpuID();
//2.获取键盘ID
CXhChar200 GetHardDiskID();
//3.获取电脑名称
CXhChar200 GetComputerName();
//4.获取IP地址
CXhChar200 GetIP();
//5.获取MAC地址
CXhChar200 GetMacAddress();
//6.获取BIOS序号
CXhChar200 GetBOISSerialNO();

//
CXhChar200 GetComputerFingerprintStr();
//
DWORD Generate128SecretKey(BYTE key[16]);

