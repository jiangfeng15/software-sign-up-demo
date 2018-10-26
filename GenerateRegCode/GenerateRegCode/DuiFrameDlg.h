#pragma once
#include "..\DuiLib\UIlib.h"
using namespace DuiLib;
#include "Resource.h"
#include "nb30.h"
#pragma comment(lib,"netapi32.lib")
#include "Md5Encode.h"

#ifdef _DEBUG
# ifdef _UNICODE
#	 pragma comment(lib, "..\\Lib\\DuiLib_ud.lib")
# else
#	pragma comment(lib, "..\\Lib\\DuiLib_d.lib")
# endif
#else
# ifdef _UNICODE
#	pragma comment(lib, "..\\Lib\\DuiLib_u.lib")
# else
#	pragma comment(lib, "..\\Lib\\DuiLib.lib")
# endif
#endif

#define MaxNum 120

class CDuiFrameDlg: public WindowImplBase
{
public:
	virtual LPCTSTR GetWindowClassName() const { return _T("DuiMainFrameDlg"); }
	virtual CDuiString GetSkinFile() { return _T("UiConfig.xml"); }
	//virtual CDuiString GetSkinFolder() { return _T("Resource\\"); }
	virtual CDuiString GetSkinFolder() { return _T(""); }

	virtual LPCTSTR GetResourceID()const
	{
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
	virtual UILIB_RESOURCETYPE GetResourceType() const
	{
		return UILIB_ZIPRESOURCE;
	}

	virtual void InitWindow();

	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
public:
	CDuiFrameDlg(void);
	~CDuiFrameDlg(void);

	void OnGenerateCode();
public:

	CString GetCpuNum();
	CString GetDiskNum();
	CString GetMacAddress();
	CString GetRegCode(CString szSerialInfo);
	CString GetExeDirPath();
	void HScrollLabelInfo();
	void TurnTabColor();
public:
	//网卡信息结构体
	struct ADAPTER_INFO
	{
		ADAPTER_STATUS nStatus;
		NAME_BUFFER nBuffer;
	};
	CMd5Encode m_md5encode;

	//控件变量
	CEditUI * m_editUserName;
	CEditUI * m_editRegCode1;
	CEditUI * m_editRegCode2;
	CEditUI * m_editRegCode3;
	CEditUI * m_editRegCode4;
	CLabelUI * m_labelInfo;
	CLabelUI * m_labeltab;
	CString m_szLabelInfo;
	CString m_szCurInfo;
	int m_nColorIndex;
};
