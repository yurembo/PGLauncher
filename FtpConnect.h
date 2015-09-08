// author: Yuriy Yazev, july 2015
#pragma once

#include <windows.h>
#include <WinUser.h>
#include <strsafe.h>
#include <wininet.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include "SkinProgress.h"//skin progress bar
//#include "FileDownData.h"

//#include "ProgressCtrlST.h"//progress bar

using namespace std;

class CFtpConnect
{
private:
	CWnd *hWin;
	HINTERNET hHTTPInternet, hHTTPConnect;
	HINTERNET hFTPInternet, hFTPConnect;

	CSkinProgress *_TotalSkinProgress;

	CRichEditCtrl *_percentText;
	CRichEditCtrl *_speedText;

	DWORDLONG g_downSize;//всего скачано

	LONG64 lastTime;//временная отметка
	LONG64 lastDownload;//отметка количества данных

public:
	CFtpConnect();
	CFtpConnect(CWnd *hwin);
	virtual ~CFtpConnect();

	bool WINAPI GetHttpFileToCreate(LPCWSTR file, DWORDLONG fileSize, DWORDLONG totalSize);
	bool WINAPI GetFtpFileToCreate(LPCWSTR file, DWORDLONG fileSize, DWORDLONG totalSize, bool isTxt = false);

	void setWin(CWnd *hwin) { hWin = hwin; }
	void setTotalProgress(CSkinProgress *totalProg);
	void setPercentText(CRichEditCtrl *text) { _percentText = text; }
	void setSpeedText(CRichEditCtrl *text) { _speedText = text; }
	void showPercentTotalStr(unsigned long iBytesRead, LONG64 totalSize);

	DWORDLONG getDownSize();//возвращает количество скаченного

	void createHTTPConnect();
	void closeHTTPConnect();
	void createFTPConnect();
	void closeFTPConnect();
	BOOL checkInternetConnect();
};