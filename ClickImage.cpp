// author: Yuriy Yazev, july 2015
// ClickImage.cpp : implementation file
//

#include "stdafx.h"
#include "ClickImage.h"
#include "resource.h"
#include "functions.h"
#include "PGLauncherDlg.h"

#include <stdio.h>
#include <tchar.h>

// CClickImage

BOOL CClickImage::hasclass = CClickImage::RegisterMe();

IMPLEMENT_DYNAMIC(CClickImage, CWnd)

BOOL CClickImage::RegisterMe()
{
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = ::DefWindowProc; // must be this value
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
	wc.hIcon = NULL;     // child window has no icon         
	wc.hCursor = NULL;   // we use OnSetCursor                  
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.lpszMenuName = NULL;  // no menu                             
	wc.lpszClassName = _T("CLICKIMAGE_CLASS_NAME");
	return AfxRegisterClass(&wc);
}

CClickImage::CClickImage()
{
	over_flag = false;
	press_flag = false;
	up_img = 0;
	over_img = 0;
	press_img = 0;
}

CClickImage::~CClickImage()
{
	over_flag = false;
	press_flag = false;
	up_img = 0;
	over_img = 0;
	press_img = 0;
}

BEGIN_MESSAGE_MAP(CClickImage, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CClickImage message handlers

void CClickImage::OnPaint()
{
	CPaintDC dc(this);
	CRect rctWindowSize;
	GetClientRect(rctWindowSize);
	if (!image.IsNull()) {
		image.Draw(dc, rctWindowSize);
	}
}


LRESULT CClickImage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::DefWindowProc(message, wParam, lParam);
}

void CClickImage::setImage(LPCWSTR fileName)
{
	if (!image.IsNull()) image.Destroy();
	image.Load(fileName);
}

void CClickImage::setImage(int picNum)
{
	if (!image.IsNull()) image.Destroy();
	image.LoadFromResource(AfxGetInstanceHandle(), picNum);//загружаем картирнку
	this->Invalidate();
}

void CClickImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	changeImage();
	int pObj = this->GetDlgCtrlID();
	switch(pObj) {
	case IDC_ButtonFold :
		this->GetParent()->ShowWindow(SW_MINIMIZE);
		break;
	case IDC_ButtonClose :
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		break;
	case IDC_LINKIMAGE : 
		if (url != "") {
			wstring ws = wide_string(url, loc);
			LPCWSTR lurl = ws.c_str();
			ShellExecute(NULL, _T("open"), lurl, NULL, NULL, SW_SHOWNORMAL);
		}
		break;
	case IDC_Button_Enter :
		if (_exeFilePath.length() > 0) {
			_onEnterClicked = true;
			runApp();
		}
		break;
	case IDC_Button_Choose: {
			setImage(press_img);
			CWnd *wnd = this->GetParent();
			HWND *hw = &wnd->m_hWnd;
			_dirGameInstall = narrow(chooseFolder(hw));
		}
		break;
	case IDC_Button_Install :
		setImage(press_img);
		wstring dg = wide_string(_dirGameInstall, loc);
		SetCurrentDirectory(dg.c_str());
		wstring tdir = wide_string(_dirName, loc);
		LPCWSTR dir = tdir.c_str();
		CreateDirectory(dir, NULL);
		SetCurrentDirectory(dir);
		TCHAR buffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buffer);
		_dirGameInstall = narrow(buffer);
		if (saveDirPathToFile()) {
			SetCurrentDirectory(buffer);
			_doInstall = true;
			global_Download = true;
		}
		break;
	} 
}

bool CClickImage::saveDirPathToFile()
{
	wstring dh = wide_string(_dirHomeExe, loc);
	SetCurrentDirectory(dh.c_str());
	bool ret = false;
	try {
		ofstream fout(_directoryGame, ios::out);
		fout << _word_path;
		fout << " ";
		fout << _dirGameInstall;
		fout.close();
		ret = true;
	} catch (...) { }
	return ret;
}

void CClickImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	int pObj = this->GetDlgCtrlID();
	if (pObj == IDC_Button_Choose || pObj == IDC_Button_Install) {
		setImage(over_img);
	}
}

void CClickImage::OnMouseMove(UINT nFlags, CPoint point)
{
	changeImage();
}

void CClickImage::changeImage()
{
	int pObj = this->GetDlgCtrlID();
	if ((pObj == IDC_ButtonClose || pObj == IDC_ButtonFold || pObj == IDC_Button_Enter || pObj == IDC_Button_Choose 
		|| pObj == IDC_Button_Install)
		&& !over_flag) {
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		this->setImage(over_img);
		over_flag = true;
	}
	if (pObj == IDC_LINKIMAGE) 
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
}