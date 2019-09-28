#include "StdAfx.h"
#include "hardcomm.h"
#include <string.h>
#include <stdio.h>

BOOL WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
    BOOL bInfoLoaded = FALSE;
    
    for( int iController = 0; iController < 2; ++ iController )
    {
        HANDLE hScsiDriveIOCTL = 0;
        char   szDriveName[256];
        
        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can’t.
        sprintf( szDriveName, "\\\\.\\Scsi%d:", iController );
		
        //  Windows NT, Windows 2000, any rights should do
        hScsiDriveIOCTL = CreateFile( szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
		
        // if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
        //    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
        //            controller, GetLastError ());
        
        if( hScsiDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            int iDrive = 0;
            for( iDrive = 0; iDrive < 2; ++ iDrive )
            {
                char szBuffer[sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH] = { 0 };
				
                SRB_IO_CONTROL* p = ( SRB_IO_CONTROL* )szBuffer;
                SENDCMDINPARAMS* pin = ( SENDCMDINPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                DWORD dwResult;
				
                p->HeaderLength = sizeof( SRB_IO_CONTROL );
                p->Timeout = 10000;
                p->Length = SENDIDLENGTH;
                p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
                strncpy( ( char* )p->Signature, "SCSIDISK", 8 );
				
                pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
                pin->bDriveNumber = iDrive;
                
                if( DeviceIoControl( hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + sizeof( SENDCMDINPARAMS ) - 1,
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH,
                    &dwResult, NULL ) )
                {
                    SENDCMDOUTPARAMS* pOut = ( SENDCMDOUTPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                    IDSECTOR* pId = ( IDSECTOR* )( pOut->bBuffer );
                    if( pId->sModelNumber[0] )
                    {
                        if( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            // 序列号
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 10, 20 );
							
                            // Cut off the trailing blanks
                            for( UINT i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; --i )
                            {}
                            * puSerialLen += i;
							
                            // 型号
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 27, 40 );
                            // Cut off the trailing blanks
                            for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1];  --i )
                            {}
                            * puSerialLen += i;
							
                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hScsiDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            ::CloseHandle( hScsiDriveIOCTL );
        }
    }
    return bInfoLoaded;
}

BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				PDWORD lpcbBytesReturned )
{
    // Set up data structures for IDENTIFY command.
    pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
    pSCIP->irDriveRegs.bFeaturesReg     = 0;
    pSCIP->irDriveRegs.bSectorCountReg  = 1;
    pSCIP->irDriveRegs.bSectorNumberReg = 1;
    pSCIP->irDriveRegs.bCylLowReg       = 0;
    pSCIP->irDriveRegs.bCylHighReg      = 0;
    
    // calc the drive number.
    pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );
	
    // The command can either be IDE identify or ATAPI identify.
    pSCIP->irDriveRegs.bCommandReg = bIDCmd;
    pSCIP->bDriveNumber = bDriveNum;
    pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
    
    return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
        ( LPVOID ) pSCIP,
        sizeof( SENDCMDINPARAMS ) - 1,
        ( LPVOID ) pSCOP,
        sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
        lpcbBytesReturned, NULL );
}

BOOL WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
#define  DFP_GET_VERSION          0x00074080
    BOOL bInfoLoaded = FALSE;
	
    for( UINT uDrive = 0; uDrive < 4; ++ uDrive )
    {
        HANDLE hPhysicalDriveIOCTL = 0;
		
        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can’t.
        char szDriveName [256];
        sprintf( szDriveName, "\\\\.\\PhysicalDrive%d", uDrive );
		
        //  Windows NT, Windows 2000, must have admin rights
        hPhysicalDriveIOCTL = CreateFile( szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
		
        if( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            GETVERSIONOUTPARAMS VersionParams = { 0 };
            DWORD               cbBytesReturned = 0;
			
            // Get the version, etc of PhysicalDrive IOCTL
            if( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
                NULL, 
                0,
                &VersionParams,
                sizeof( GETVERSIONOUTPARAMS ),
                &cbBytesReturned, NULL ) )
            {
                // If there is a IDE device at number "i" issue commands
                // to the device
                if( VersionParams.bIDEDeviceMap != 0 )
                {
                    BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
                    SENDCMDINPARAMS  scip = { 0 };
					
                    // Now, get the ID sector for all IDE devices in the system.
                    // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
                    // otherwise use the IDE_ATA_IDENTIFY command
                    bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
                    BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };
					
                    if( DoIdentify( hPhysicalDriveIOCTL, 
                        &scip, 
                        ( PSENDCMDOUTPARAMS )&IdOutCmd, 
                        ( BYTE )bIDCmd,
                        ( BYTE )uDrive,
                        &cbBytesReturned ) )
                    {
                        if( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 );  // 序列号
							
                            // Cut off the trailing blanks
                            for( UINT i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; --i )  {}
                            * puSerialLen += i;
							
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // 型号
							
                            // Cut off the trailing blanks
                            for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; --i )  {}
                            * puSerialLen += i;
							
                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hPhysicalDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            CloseHandle( hPhysicalDriveIOCTL );
        }
    }
    return bInfoLoaded;
}

UINT FindAwardBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlen( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        //AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
        //Phoenix-Award: 03/12/2002-sis645-p4s333
        if( szBiosData[2] == '/' && szBiosData[5] == '/' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindAmiBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xF478;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlen( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        // Example: "AMI: 51-2300-000000-00101111-030199-"
        if( szBiosData[2] == '-' && szBiosData[7] == '-' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindPhoenixBios( BYTE** ppBiosAddr )
{
    UINT uOffset[3] = { 0x6577, 0x7196, 0x7550 };
    for( UINT i = 0; i < 3; ++ i )
    {
        BYTE* pBiosAddr = * ppBiosAddr + uOffset[i];
		
        BYTE szBiosData[128];
        CopyMemory( szBiosData, pBiosAddr, 127 );
        szBiosData[127] = 0;
		
        int iLen = lstrlen( ( char* )szBiosData );
        if( iLen > 0 && iLen < 128 )
        {
            // Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
            if( szBiosData[7] == '.' && szBiosData[11] == '.' )
            {
                BYTE* p = szBiosData;
                while( * p )
                {
                    if( * p < ' ' || * p >= 127 )
                    {
                        break;
                    }
                    ++ p;
                }
                if( * p == 0 )
                {
                    * ppBiosAddr = pBiosAddr;
                    return ( UINT )iLen;
                }
            }
        }
    }
    return 0;
}

// 打开设备
// filename: 设备的“文件名”
HANDLE OpenDevice(LPCTSTR filename)
{
	HANDLE hDevice;
	
	// 打开设备
	hDevice= CreateFile(filename,					// 文件名
		GENERIC_READ | GENERIC_WRITE,			// 读写方式
		FILE_SHARE_READ | FILE_SHARE_WRITE,		// 共享方式
		NULL,									// 默认的安全描述符
		OPEN_EXISTING,							// 创建方式
		0,										// 不需设置文件属性
		NULL);									// 不需参照模板文件
	
	return hDevice;
}

// 向驱动发“IDENTIFY DEVICE”命令，获得设备信息
// hDevice: 设备句柄
// pIdInfo:  设备信息结构指针
BOOL IdentifyDevice(HANDLE hDevice, PIDINFO pIdInfo)
{
	PSENDCMDINPARAMS pSCIP;		// 输入数据结构指针
	PSENDCMDOUTPARAMS pSCOP;	// 输出数据结构指针
	DWORD dwOutBytes;			// IOCTL输出数据长度
	BOOL bResult;				// IOCTL返回值
	
	// 申请输入/输出数据结构空间
    pSCIP = (PSENDCMDINPARAMS)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SENDCMDINPARAMS)-1);
    pSCOP = (PSENDCMDOUTPARAMS)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SENDCMDOUTPARAMS)+sizeof(IDINFO)-1);
	
	// 指定ATA/ATAPI命令的寄存器值
	//	pSCIP->irDriveRegs.bFeaturesReg = 0;
	//	pSCIP->irDriveRegs.bSectorCountReg = 0;
	//	pSCIP->irDriveRegs.bSectorNumberReg = 0;
	//	pSCIP->irDriveRegs.bCylLowReg = 0;
	//	pSCIP->irDriveRegs.bCylHighReg = 0;
	//	pSCIP->irDriveRegs.bDriveHeadReg = 0;
	pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
	
	// 指定输入/输出数据缓冲区大小
	pSCIP->cBufferSize = 0;
	pSCOP->cBufferSize = sizeof(IDINFO);
	
	// IDENTIFY DEVICE
	bResult = DeviceIoControl(hDevice,						// 设备句柄
		DFP_RECEIVE_DRIVE_DATA,									// 指定IOCTL
		pSCIP, sizeof(SENDCMDINPARAMS) - 1,						// 输入数据缓冲区
		pSCOP, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1,	// 输出数据缓冲区
		&dwOutBytes,											// 输出数据长度
		(LPOVERLAPPED)NULL);									// 用同步I/O
	
	// 复制设备参数结构
	memcpy(pIdInfo, pSCOP->bBuffer, sizeof(IDINFO));
	
	// 释放输入/输出数据空间
	GlobalFree(pSCOP);
	GlobalFree(pSCIP);
	
	return bResult;
}

// 向SCSI MINI-PORT驱动发“IDENTIFY PACKET DEVICE”命令，获得设备信息
// hDevice: 设备句柄
// pIdInfo:  设备信息结构指针
BOOL IdentifyDeviceAsScsi(HANDLE hDevice, int nDrive, PIDINFO pIdInfo)
{
	PSENDCMDINPARAMS pSCIP;		// 输入数据结构指针
	PSENDCMDOUTPARAMS pSCOP;	// 输出数据结构指针
	PSRB_IO_CONTROL pSRBIO;		// SCSI输入输出数据结构指针
	DWORD dwOutBytes;			// IOCTL输出数据长度
	BOOL bResult;				// IOCTL返回值
	
	// 申请输入/输出数据结构空间
    pSRBIO = (PSRB_IO_CONTROL)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SRB_IO_CONTROL)+sizeof(SENDCMDOUTPARAMS)+sizeof(IDINFO)-1);
    pSCIP = (PSENDCMDINPARAMS)((char *)pSRBIO+sizeof(SRB_IO_CONTROL));
    pSCOP = (PSENDCMDOUTPARAMS)((char *)pSRBIO+sizeof(SRB_IO_CONTROL));
	
	// 填充输入/输出数据
	pSRBIO->HeaderLength = sizeof(SRB_IO_CONTROL);
	pSRBIO->Timeout = 10000;
	pSRBIO->Length = sizeof(SENDCMDOUTPARAMS)+sizeof(IDINFO)-1;
	pSRBIO->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
    ::strncpy ((char *)pSRBIO->Signature, "SCSIDISK", 8);
	
	// 指定ATA/ATAPI命令的寄存器值
	//	pSCIP->irDriveRegs.bFeaturesReg = 0;
	//	pSCIP->irDriveRegs.bSectorCountReg = 0;
	//	pSCIP->irDriveRegs.bSectorNumberReg = 0;
	//	pSCIP->irDriveRegs.bCylLowReg = 0;
	//	pSCIP->irDriveRegs.bCylHighReg = 0;
	//	pSCIP->irDriveRegs.bDriveHeadReg = 0;
	pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
	pSCIP->bDriveNumber = nDrive;
	
	// IDENTIFY DEVICE
	bResult = DeviceIoControl(hDevice,						// 设备句柄
		IOCTL_SCSI_MINIPORT,									// 指定IOCTL
		pSRBIO, sizeof(SRB_IO_CONTROL) +sizeof(SENDCMDINPARAMS) - 1,						// 输入数据缓冲区
		pSRBIO, sizeof(SRB_IO_CONTROL) +sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1,	// 输出数据缓冲区
		&dwOutBytes,											// 输出数据长度
		(LPOVERLAPPED)NULL);									// 用同步I/O
	
	// 复制设备参数结构
	memcpy(pIdInfo, pSCOP->bBuffer, sizeof(IDINFO));
	
	// 释放输入/输出数据空间
	GlobalFree(pSRBIO);
	
	return bResult;
}

// 将串中的字符两两颠倒
// 原因是ATA/ATAPI中的WORD，与Windows采用的字节顺序相反
// 驱动程序中已经将收到的数据全部反过来，我们来个负负得正
void AdjustString(char* str, int len)
{
	char ch;
	int i;
	
	// 两两颠倒
	for(i=0;i<len;i+=2)
	{
		ch = str[i];
		str[i] = str[i+1];
		str[i+1] = ch;
	}
	
	// 若是右对齐的，调整为左对齐 (去掉左边的空格)
	i=0;
	while(i<len && str[i]==' ') i++;
	
	memmove(str, &str[i], len-i);
	str[len-i]='\0';
	// 去掉右边的空格
	i = len - 1;
	while(i>=0 && str[i]==' ')
	{
		str[i] = '\0';
		i--;
	}
}

// 读取IDE硬盘的设备信息，必须有足够权限
// nDrive: 驱动器号(0=第一个硬盘，1=0=第二个硬盘，......)
// pIdInfo: 设备信息结构指针
BOOL GetPhysicalDriveInfoInNT(int nDrive, PIDINFO pIdInfo)
{
	HANDLE hDevice;			// 设备句柄
	BOOL bResult;			// 返回结果
	char szFileName[20];	// 文件名
	
	sprintf(szFileName,"\\\\.\\PhysicalDrive%d", nDrive);
	
	hDevice = OpenDevice(szFileName);
	
	if(hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	// IDENTIFY DEVICE
	bResult = IdentifyDevice(hDevice, pIdInfo);
	
	if(bResult)
	{
		// 调整字符串
		AdjustString(pIdInfo->sSerialNumber, 20);
		AdjustString(pIdInfo->sModelNumber, 40);
		AdjustString(pIdInfo->sFirmwareRev, 8);
	}
	
	CloseHandle (hDevice);
	
	return bResult;
}

// 用SCSI驱动读取IDE硬盘的设备信息，不受权限制约
// nDrive: 驱动器号(0=Primary Master, 1=Promary Slave, 2=Secondary master, 3=Secondary slave)
// pIdInfo: 设备信息结构指针
BOOL GetIdeDriveAsScsiInfoInNT(int nDrive, PIDINFO pIdInfo)
{
	HANDLE hDevice;			// 设备句柄
	BOOL bResult;			// 返回结果
	char szFileName[20];	// 文件名
	
	sprintf(szFileName,"\\\\.\\Scsi%d", nDrive/2);
	
	hDevice = OpenDevice(szFileName);
	
	if(hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	// IDENTIFY DEVICE
	bResult = IdentifyDeviceAsScsi(hDevice, nDrive%2, pIdInfo);
	
	// 检查是不是空串
	if(pIdInfo->sModelNumber[0]=='\0')
	{
		bResult = FALSE;
	}
	
	if(bResult)
	{
		// 调整字符串
		AdjustString(pIdInfo->sSerialNumber, 20);
		AdjustString(pIdInfo->sModelNumber, 40);
		AdjustString(pIdInfo->sFirmwareRev, 8);
	}
	CloseHandle (hDevice);
	return bResult;
}