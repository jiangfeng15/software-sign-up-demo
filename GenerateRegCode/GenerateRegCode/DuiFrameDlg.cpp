#include "StdAfx.h"
#include "DuiFrameDlg.h"
#include "resource.h"
#include <string>

CDuiFrameDlg::CDuiFrameDlg(void)
{
	m_editUserName = NULL; 
	m_editRegCode1 = NULL;
	m_editRegCode2 = NULL;
	m_editRegCode3 = NULL;
	m_editRegCode4 = NULL;
	m_labelInfo = NULL;
	m_labeltab = NULL;
	m_szLabelInfo = _T("");
	m_szCurInfo = _T("");
	m_nColorIndex = 3;
}

CDuiFrameDlg::~CDuiFrameDlg(void)
{
}

void CDuiFrameDlg::InitWindow()
{
	//此处添加初始化代码
	m_editUserName = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editname")));
	ASSERT(m_editUserName);

	m_editRegCode1 = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editcode1")));
	ASSERT(m_editRegCode1);

	m_editRegCode2 = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editcode2")));
	ASSERT(m_editRegCode2);

	m_editRegCode3 = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editcode3")));
	ASSERT(m_editRegCode3);

	m_editRegCode4 = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editcode4")));
	ASSERT(m_editRegCode4);

	m_labelInfo = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("labelinfo")));
	ASSERT(m_labelInfo);

	m_labeltab = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("tabLabelinfo")));
	ASSERT(m_labeltab);

	m_editRegCode1->SetText(_T("XXXXXXX"));
	m_editRegCode2->SetText(_T("XXXXXXX"));
	m_editRegCode3->SetText(_T("XXXXXXX"));
	m_editRegCode4->SetText(_T("XXXXXXX"));

	m_labelInfo->SetText(_T("等待生成软件注册码！"));
	static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("maxbtn")))->SetEnabled(FALSE);
	SetTimer(this->m_hWnd, 101, 600, NULL);
	
}

void CDuiFrameDlg::Notify(TNotifyUI& msg)
{

	if (msg.sType == _T("click"))
	{

		if (msg.pSender->GetName() == _T("minbtn"))
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			return;
		}
		else if (msg.pSender->GetName() == _T("maxbtn"))
		{
			//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			return;
		}
		else if (msg.pSender->GetName() == _T("closebtn"))
		{
			PostQuitMessage(0);
			return;
		}
		else if (msg.pSender->GetName() == _T("restorebtn"))
		{
			//SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			return;
		}
		else if (msg.pSender->GetName() == _T("btnregister"))
		{
			OnGenerateCode();
			return ;
		}

	}

}
//windows标准消息处理
LRESULT CDuiFrameDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCACTIVATE)
	{
		if (!::IsIconic(m_hWnd))
		{
			return (wParam == 0) ? TRUE : FALSE;
		}
	}
	else if (uMsg == WM_NCCALCSIZE)
	{
		return 0;
	}
	else if (uMsg == WM_NCPAINT)
	{
		return 0;
	}
	else if(uMsg == WM_NCLBUTTONDBLCLK)
	{
		return 0;
	}
	else if (uMsg == WM_TIMER)
	{
		if(wParam == 100)
		{
			HScrollLabelInfo();
		}
		else if(wParam == 101)
		{
			TurnTabColor();
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CDuiFrameDlg::OnGenerateCode()
{
	CString szKeyID(_T("")), szResSN(_T(""));

	CDuiString szUserName(_T(""));
	szUserName = m_editUserName->GetText();
	if(szUserName.IsEmpty())
	{
		AfxMessageBox(_T("请输入用户名！"));
		return ;
	}
	szKeyID	= GetCpuNum() + GetDiskNum();
	string sKeyID1, sSerialInfo, szTemp;
	sKeyID1 = CStringA(szKeyID);

	sSerialInfo = m_md5encode.Encode(sKeyID1);

	szTemp = szTemp + sSerialInfo[0]+sSerialInfo[1]+sSerialInfo[2]+sSerialInfo[3]+sSerialInfo[7]+sSerialInfo[15]+
		sSerialInfo[31]+sSerialInfo[5]+sSerialInfo[11]+sSerialInfo[22]+sSerialInfo[30]+sSerialInfo[9]+sSerialInfo[10]+sSerialInfo[17];
	
	szResSN = GetRegCode((CString)szTemp.c_str());
	m_editRegCode1->SetText(szResSN.Mid(0,7));
	m_editRegCode2->SetText(szResSN.Mid(7,7));
	m_editRegCode3->SetText(szResSN.Mid(14,7));
	m_editRegCode4->SetText(szResSN.Mid(21,7));
	
	CString szRFilePath(_T("")), szLine(_T(""));
	szRFilePath = GetExeDirPath();
	CStdioFile clsFile;
	if(!clsFile.Open(szRFilePath+_T("\\license.txt"), CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox(_T("新建认证文件失败！"));
		return ;
	}
	szLine.Format(_T("username:%s\n"), szUserName.GetData());
	clsFile.WriteString(szLine);
	szLine.Format(_T("sn:%s\n"), szResSN);
	clsFile.WriteString(szLine);
	clsFile.Close();
	szLine.Format(_T("注册码生成成功,保存在:%s\\license.txt中！"), szRFilePath);
	m_labelInfo->SetText(szLine);

	m_szLabelInfo = szLine;
	if(m_szLabelInfo.GetLength() >= 10)
	{
		m_szCurInfo = m_szLabelInfo;
		SetTimer(this->m_hWnd, 100, 600, NULL);
	}

}

CString CDuiFrameDlg::GetCpuNum()
{
	//获取CPU序列号
	unsigned long s1, s2;
	CString szCpuID1(_T("")), szCpuID2(_T(""));
	__asm {
		mov eax, 01h
		xor edx, edx
		cpuid
		mov s1, edx
		mov s2, eax
	}
	szCpuID1.Format(_T("%08X%08X"), s1, s2);

	__asm{
		mov eax, 03h
		xor ecx, ecx
		xor edx, edx
		cpuid
		mov s1, edx
		mov s2, ecx
	}
	szCpuID2.Format(_T("%08X%08X"), s1, s2);
	
	//拼接结果
	CString szCpuID = szCpuID1;
	return szCpuID;
}

CString CDuiFrameDlg::GetDiskNum()
{
	DWORD dwSerial;
	char cDiskName[128];
	char cFileSys[128];
	CString szDiskID(_T(""));
	::GetVolumeInformation(_T("c:\\"), //磁盘根路径
		(LPWSTR)cDiskName, //用于装载磁盘名的缓冲区
		128,               //缓冲区大小
		&dwSerial,         //装载磁盘序列号的变量
		0,				   //装载文件名每一部分的长度
		0,                 //标志位
		(LPWSTR)cFileSys,  //装载文件系统的名称
		128                //缓冲区大小
		);
	szDiskID.Format(_T("%08X"), dwSerial);
	return szDiskID;
}

CString CDuiFrameDlg::GetMacAddress()
{
	CString szMacAddress(_T(""));
	NCB nInfo;
	memset(&nInfo, 0, sizeof(NCB));
	nInfo.ncb_command = NCBRESET; //设置命令
	nInfo.ncb_lana_num = 0;
	Netbios(&nInfo);

	//存储网卡信息
	ADAPTER_INFO AdaInfo;
	memset(&nInfo, 0, sizeof(NCB));
	//初始化NetBios
	nInfo.ncb_command = NCBASTAT;
	nInfo.ncb_lana_num = 0;
	nInfo.ncb_buffer = (unsigned char *)&AdaInfo;
	nInfo.ncb_length = sizeof(ADAPTER_INFO);
	strncpy((char *)nInfo.ncb_callname, "*", NCBNAMSZ);
	Netbios(&nInfo);

	szMacAddress.Format(_T("%02X%02X%02X%02X%02X%02X"), 
		AdaInfo.nStatus.adapter_address[0],
		AdaInfo.nStatus.adapter_address[1],
		AdaInfo.nStatus.adapter_address[2],
		AdaInfo.nStatus.adapter_address[3],
		AdaInfo.nStatus.adapter_address[4],
		AdaInfo.nStatus.adapter_address[6]
		);
		return szMacAddress;
}

CString CDuiFrameDlg::GetRegCode(CString szSerialInfo)
{
	int nIndex, nPostion;
	CString szRes(_T(""));
	//定义一个秘钥数组
	CString szCode[16] = {
		_T("ah"), _T("tm"), _T("ib"), _T("nw"), _T("rt"), _T("vx"), _T("zc"), _T("gf"),
		_T("pn"), _T("xq"), _T("fc"), _T("oj"), _T("wm"), _T("eq"), _T("np"), _T("qw")
	};
	for(nIndex=0; nIndex < szSerialInfo.GetLength(); nIndex++)
	{
		char cTemp = szSerialInfo[nIndex];
		if(cTemp >= 'a' && cTemp <= 'z')
		{
			nPostion = cTemp - 'a' + 10;
		}
		else
		{
			nPostion = cTemp - '0';
		}
		szRes = szRes + szCode[nPostion];
	}
	szRes.MakeUpper();
	return szRes;
}

CString CDuiFrameDlg::GetExeDirPath()
{
	CString szResDir(_T("")), szTemp(_T(""));
	char szBuf[1025] = {0}; 

	GetModuleFileName(NULL, (LPWCH)szBuf, sizeof(szBuf));
	szResDir.Format(_T("%s"), szBuf);
	szResDir = szResDir.Left(szResDir.ReverseFind('\\'));
	return szResDir;
}

void CDuiFrameDlg::HScrollLabelInfo()
{
	CString szTemp(_T(""));
	if(!m_szCurInfo.IsEmpty())
	{
		szTemp = m_szCurInfo.Right(m_szCurInfo.GetLength()-2);
		m_labelInfo->SetText(szTemp);
		m_szCurInfo = szTemp;
	}
	else
	{
		m_szCurInfo = m_szLabelInfo;
	}
}

void CDuiFrameDlg::TurnTabColor()
{
	if(m_nColorIndex == 0)
	{
		m_nColorIndex = 3;
	}
	else
	{
		switch(m_nColorIndex)
		{
		case 1:
			{
				m_labeltab->SetTextColor(0xFFB4B4FF);
				break;
			}
		case 2:
			{
				m_labeltab->SetTextColor(0xFFFFFFFF);
				break;
			}
		case 3:
			{
				m_labeltab->SetTextColor(0xFFFFC8C8);
				break;
			}
		default:
			break;
		}	
		m_nColorIndex--;
	}
}