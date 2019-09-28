#if !defined(__ENLARGE_FILE_DIALOG__)
#define __ENLARGE_FILE_DIALOG__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEnlargeFileDialog
{
	TCHAR *m_pBufOld;
	DWORD m_dwMaxOld;
	CFileDialog *m_pFileDlg;
public:
	CEnlargeFileDialog(CFileDialog *pFileDlg,int nMaxFileNum=500)
	{
		m_pFileDlg=pFileDlg;
		if(m_pFileDlg&&nMaxFileNum>0)
		{
			m_pBufOld = m_pFileDlg->m_ofn.lpstrFile;   //保存旧的指针
			m_dwMaxOld = m_pFileDlg->m_ofn.nMaxFile;    //保存旧的数量
			m_pFileDlg->m_ofn.lpstrFile = new TCHAR[ nMaxFileNum * MAX_PATH];
			ZeroMemory(m_pFileDlg->m_ofn.lpstrFile,sizeof(TCHAR) * nMaxFileNum * MAX_PATH);
			m_pFileDlg->m_ofn.nMaxFile = nMaxFileNum * MAX_PATH;    //此最大数,按MSDN解释为fileDlg.m_ofn.lpstrFile指向的缓冲的最大字符数
		}
		else 
		{
			m_pBufOld=NULL;
			m_dwMaxOld=0;
		}
	}
	~CEnlargeFileDialog()
	{
		if(m_pFileDlg&&m_pBufOld)
		{
			delete [](m_pFileDlg->m_ofn.lpstrFile);        //清除堆内存
			m_pFileDlg->m_ofn.lpstrFile = m_pBufOld;        //还原之前的指针,因为没有深入过CFileDialog源码分析,不清这个是否必须的,还是先还原上吧,
			m_pFileDlg->m_ofn.nMaxFile = m_dwMaxOld;        //还原之前的最大数
		}
	}
};
#endif