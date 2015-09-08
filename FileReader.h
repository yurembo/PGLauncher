// author: Yuriy Yazev, july 2015
#pragma once

#include <iostream>
#include <string>
#include <clocale>
#include <locale>
#include <vector>
#include <io.h>
#include "FtpConnect.h"
#include "consts.h"
#include "resource.h"
#include "ClickImage.h"
#include "SkinProgress.h"

using namespace std;

class CFileReader
{
private:
	CWnd *hWin;
	DWORDLONG g_totalSize;

	CSkinProgress *_TotalSkinProgress;
	CRichEditCtrl *_percentText;
	CRichEditCtrl *_speedText;

	//надпись и кнопка-картинка 
	CStatic *m_UpdateText;
	CClickImage *m_EnterButton;

	//FileDownData fileDownData;//сведения о скачиваемом в данный момент файле

public:
	CFileReader();
	CFileReader(CWnd *hwin);
	virtual ~CFileReader();

	bool fileSearch(LPCWSTR dir, LPCWSTR file, LPCWSTR path, CFtpConnect& connect, DWORDLONG fileCRC = 0, DWORDLONG fileSize = 0);
	void readLocalFile(CFtpConnect& connect);
	void setWin(CWnd *hwin) { hWin = hwin; }
	bool readTotalFilesSize(string file);
	DWORDLONG getTotalSize(string file);
	void setTotalProgress(CSkinProgress* prog);
	void setPercentText(CRichEditCtrl* text) { _percentText = text; }
	void setSpeedText(CRichEditCtrl* text) { _speedText = text; }
	void HideLabelsShowButton();
	void getWholeStr(ifstream &fin, string &res);
	UINT32 calcCtrlSum(LPCWSTR path);

	bool createDirSeq(LPCWSTR directory);//создать последовательность директорий
	bool createDir(LPCWSTR directory);//создать каталог

	void setUpdateText(CStatic *upText);
	void setEnterButton(CClickImage *enterBut);

	bool downloadFile(LPCWSTR path, CFtpConnect& connect, DWORDLONG fileSize, DWORDLONG totalSize);
};