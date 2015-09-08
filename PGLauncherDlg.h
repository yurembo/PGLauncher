// author: Yuriy Yazev, july 2015
// PGLauncherDlg.h : header file
//

#pragma once

#include <thread>
#include "FtpConnect.h"
#include "FileReader.h"
#include "consts.h"
#include "ClickImage.h"

#include "SkinScrollWnd.h"//scrollbar
#include "SkinScrollBar.h"//scrollbar

#include "SkinProgress.h"//progress bar

//#include "ProgressCtrlST.h"//progress bar

DWORD CALLBACK MyStreamInCallback(CFile* dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

// CPGLauncherDlg dialog
class CPGLauncherDlg : public CDialogEx
{
// Construction
public:
	CPGLauncherDlg(CWnd* pParent = NULL);	// standard constructor
	~CPGLauncherDlg();
	
// Dialog Data
	enum { IDD = IDD_PGLAUNCHER_DIALOG };

	HBITMAP	m_hBmpScrollBar;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	CFileReader reader;
	CFtpConnect connect;
	CRect parentRect;
	CFont font;

	CRichEditCtrl m_RichEdit;
	CRichEditCtrl m_PercentText;
	CRichEditCtrl m_SpeedText;
	CRichEditCtrl m_PathText;
	CRichEditCtrl m_NeedSpace;
	CRichEditCtrl m_FreeSpace;

	//для CStatic чтобы объект получал сообщения - реагировал на события, надо свойства Notify установить в true, а события прописать самостоятельно
	//кроме того, чтобы отображалась картинка, надо свойство Type установить в значение Bitmap

	CClickImage _linkImage;
	CClickImage _butClose;
	CClickImage _butFold;
	CClickImage _butEnter;
	CClickImage _butChoose;
	CClickImage _butInstall;

	//объекты - шрифты
	CFont *updateNameFont;
	LOGFONT *updateNameLogFont;
	CFont *versionFont;
	LOGFONT *versionLogFont;
	CFont *infoStringFont;
	LOGFONT *infoStringLogFont;
	CFont *insFolderFont;
	LOGFONT *insFolderLogFont;
	CFont *needSpaceFont;
	LOGFONT *needSpaceLogFont;
	CFont *freeSpaceFont;
	LOGFONT *freeSpaceLogFont;

	//Timer to download files
	UINT_PTR timer;
	//Timer to check a path
	UINT_PTR timerToCheck;
	//for progress bar
	CBitmap *m_underBitmap;
	CSkinProgress* m_TotalSkinProgress;
	CProgressCtrl m_SimpleBar;//от этого объекта берется CWin и параметры

	//надпись и кнопка-картинка 
	CStatic *m_UpdateText;
	CClickImage *m_EnterButton;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDownloadButton();
	DECLARE_EVENTSINK_MAP()
	BOOL PreTranslateMessage(MSG* pMsg);

	void buildLinkImage(LPCWSTR file, string url);
	void getLauncherFiles();
	void readLauncherFiles(CFtpConnect& connect, LPCWSTR fileTxt);

	wstring getWholeLine(ifstream &f, string &fileName1);
	void windowScreenCenter();
	void modifyObj(CClickImage *img);
	void turnButtons();
	void RichEdit_Create();
	void RichEdit_DoTitle();
	void PercentText_Create();
	void SpeedText_Create();
	void ReadFile(LPCWSTR fileName);
	void buildProgressBar();
	void destroyProgressBar();
	void makeUpdateNameLabel(const wstring name);
	void makeVersionLabel(const wstring version);
	void makeInfoString(const wstring info);
	void makeFolderToInstallLabel(const wstring str);
	void makeNeedSpaceText();
	void makeFreeSpaceText();
	void makePathText(const wstring path);
	void showLabels();
	void readDirFile();
	wstring getWideFileName(string fn);
	__int64 getFreeSpace(string path);
	void prepareSelectDir();
	void beginInstallAfterFolderSelect();
	void calcDiskSpace();

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void setTimer(const int ms);
	void setTimerToCheck(const int ms);
	void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
