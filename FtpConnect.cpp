// author: Yuriy Yazev, july 2015
#include "stdafx.h"
#include "consts.h"
#include "FtpConnect.h"
#include "resource.h"
#include "functions.h"
#include <algorithm>
#include "ErrorOutFunctions.h"

CFtpConnect::CFtpConnect()
{
//	pProg = nullptr;
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	lastTime = 0;
	lastDownload = 0;
}

CFtpConnect::CFtpConnect(CWnd *hwin)
{
	hWin = hwin;
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	lastTime = 0;
	lastDownload = 0;
}

void CFtpConnect::createHTTPConnect()
{
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	lastTime = 0;
	lastDownload = 0;
	
	wstring stemp = wide_string(_appName, loc);
	LPCWSTR sw = stemp.c_str();
	hHTTPInternet = InternetOpen(sw, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hHTTPInternet) {
		//InternetErrorOut(hWin->GetSafeHwnd(), GetLastError(), TEXT("Connect Failed"));
		return;
	}
	if (hHTTPInternet) {
		wstring sip = wide_string(_domen_name, loc);
		LPCWSTR ip = sip.c_str();
		wstring sun = wide_string(_userName, loc);
		LPCWSTR un = sun.c_str();
		wstring sup = wide_string(_userPassword, loc);
		LPCWSTR up = sup.c_str();
		hHTTPConnect = InternetConnect(hHTTPInternet, ip, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1u);
		if (!hHTTPConnect) {
			//InternetErrorOut(hWin->GetSafeHwnd(), GetLastError(), TEXT("FTP Failed"));
			return;
		}
	}

	return;
}

void CFtpConnect::createFTPConnect()
{
	wstring stemp = wide_string(_appName, loc);
	LPCWSTR sw = stemp.c_str();
	hFTPInternet = InternetOpen(sw, INTERNET_OPEN_TYPE_PROXY, TEXT("proxy"), TEXT("test"), 0);
	if (!hFTPInternet) {
		//InternetErrorOut(hWin->GetSafeHwnd(), GetLastError(), TEXT("Connect Failed"));
		return;
	}
	if (hFTPInternet) {
		wstring sip = wide_string(_ipAddr, loc);
		LPCWSTR ip = sip.c_str();
		wstring sun = wide_string(_userName, loc);
		LPCWSTR un = sun.c_str();
		wstring sup = wide_string(_userPassword, loc);
		LPCWSTR up = sup.c_str();
		hFTPConnect = InternetConnect(hFTPInternet, ip, INTERNET_DEFAULT_FTP_PORT, un, up, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		/*FTP соединения бывают 2-х видов: пассивные и активные, в данной функции в качестве 7-го параметра надо использовать флаг INTERNET_FLAG_PASSIVE,
		то есть включить пассивное соединение, по тестам оно работает на активных и пассивных клиентах;
		если же не использовать флаг (поставить 0 в качестве параметра), будет использовано активное соединение, которое работает только на активных клиентах*/
		if (!hFTPConnect) {
			//InternetErrorOut(hWin->GetSafeHwnd(), GetLastError(), TEXT("FTP Failed"));
			return;
		}
	}

	return;
}

CFtpConnect::~CFtpConnect()
{
	InternetCloseHandle(hHTTPConnect);
	InternetCloseHandle(hHTTPInternet);
	InternetCloseHandle(hFTPConnect);
	InternetCloseHandle(hFTPInternet);
	hWin = nullptr;
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	lastTime = 0;
	lastDownload = 0;
}

void CFtpConnect::closeHTTPConnect()
{
	InternetCloseHandle(hHTTPConnect);
	InternetCloseHandle(hHTTPInternet);
}

void CFtpConnect::closeFTPConnect()
{
	InternetCloseHandle(hFTPConnect);
	InternetCloseHandle(hFTPInternet);
}

BOOL CFtpConnect::checkInternetConnect()
{
	BOOL res = InternetCheckConnection(_T("http://www.microsoft.com"), FLAG_ICC_FORCE_CONNECTION, 0);
	return res;
}

bool WINAPI CFtpConnect::GetHttpFileToCreate(LPCWSTR file, DWORDLONG fileSize, DWORDLONG totalSize)
{
	LPCWSTR mainFile = file;
	
	string s = "\\";
	string t = "/";
	string str = narrow(file);

	string fn = narrow(file);
	string dn;
	getFileNameAndDirName(fn, dn);
	wstring wfn = wide_string(fn, loc);
	file = wfn.c_str();

	replace(str.begin(), str.end(), '\\', '/');
	wstring wstr = wide_string(str, loc);
    LPCWSTR	rfile = wstr.c_str();

	//используется для прокачивания загрузки!
	unsigned long iBytesRead;
	unsigned long currentFileSize = 0;//скачивание текущенго файла

	HINTERNET HttpRequestDownload = HttpOpenRequest(hHTTPConnect, TEXT("GET"), rfile, NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
	
	if (HttpRequestDownload == NULL) {
		//MessageBox(hWin->GetSafeHwnd(), file, TEXT("Файл не найден"), 0);
		//InternetErrorOut(hWin->GetSafeHwnd(), GetLastError(), TEXT("FtpOpenFile"));
		InternetCloseHandle(HttpRequestDownload);
		return false;
	}
	
	wstring strHeader = _T("Accept: */*");
	BOOL bSend = HttpAddRequestHeaders(HttpRequestDownload, strHeader.c_str(), strHeader.length(), HTTP_ADDREQ_FLAG_ADD);
	if (!bSend) {
		//MessageBox(hWin->GetSafeHwnd(), strHeader.c_str(), TEXT("http-заголовок"), 0);
		return false;
	}
	strHeader = _T("Content-Type: application/x-www-form-urlencoded");
	bSend = HttpAddRequestHeaders(HttpRequestDownload, strHeader.c_str(), strHeader.length(), HTTP_ADDREQ_FLAG_ADD);
	if (!bSend) {
		//MessageBox(hWin->GetSafeHwnd(), strHeader.c_str(), TEXT("http-заголовок"), 0);
		return false;
	}
	bSend = HttpSendRequest(HttpRequestDownload, NULL, 0, NULL, 0);
	if (!bSend) {
		//MessageBox(hWin->GetSafeHwnd(), strHeader.c_str(), TEXT("http-заголовок"), 0);
		return false;
	}
	
	if (HttpRequestDownload) {
		DWORD	dwContentLength;
		//unsigned long	iBytesRead; --- объявление перенесено выше!
		BYTE	Buffer[8192];
		ZeroMemory(Buffer, sizeof(Buffer));
		dwContentLength = 4 * 1024;

		wstring wideFile = getFileNameWithoutDomen(wfn, _domen);
		LPCWSTR writeFile = wideFile.c_str();
		FILE *pFile = _tfopen(writeFile, TEXT("wb"));
		if (pFile == NULL) {
			//MessageBox(hWin->GetSafeHwnd(), writeFile, TEXT("Cannot create a file"), 0);
			return false;
		}
		do
		{
			if (InternetReadFile(HttpRequestDownload, Buffer, dwContentLength, &iBytesRead))
			{
				currentFileSize += iBytesRead;
				//выводим шкалу общего скачивания (new)
				if (_TotalSkinProgress && totalSize > 0) {
					showPercentTotalStr(iBytesRead, totalSize);
				}
				fwrite(Buffer, iBytesRead, 1, pFile);
				ZeroMemory(Buffer, sizeof(Buffer));
			}
		} while (iBytesRead != 0);
		fclose(pFile);
	}

	InternetCloseHandle(HttpRequestDownload);

	bool result = false;
	if (fileSize == 0 && totalSize == 0)
		result = true;
	else
		result = (currentFileSize == fileSize);

	return result;
}

bool WINAPI CFtpConnect::GetFtpFileToCreate(LPCWSTR file, DWORDLONG fileSize, DWORDLONG totalSize, bool isTxt)
{
	LPCWSTR mainFile = file;

	string s = "\\";
	string t = "/";
	string str = narrow(file);

	string fn = narrow(file);
	string dn;
	getFileNameAndDirName(fn, dn);
	wstring wfn = wide_string(fn, loc);
	file = wfn.c_str();

	replace(str.begin(), str.end(), '\\', '/');
	wstring wstr = wide_string(str, loc);
	LPCWSTR	rfile = wstr.c_str();

	wstring wstr2 = getFileNameWithoutDomen(file, _ftp_folder);
	str = narrow(wstr2);
	replace(str.begin(), str.end(), '/', '\\');
	wstr2 = wide_string(str, loc);
	file = wstr2.c_str();

	//используется для прокачивания загрузки!
	unsigned long iBytesRead;
	unsigned long currentFileSize = 0;//скачивание текущенго файла

	HINTERNET FTPRequestDownload = FtpOpenFile(hFTPConnect, rfile, GENERIC_READ, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 0);

	if (FTPRequestDownload == NULL) {
		//MessageBox(hWin->GetSafeHwnd(), file, TEXT("Файл не найден"), 0);
		//InternetErrorOut(hWin->GetSafeHwnd(), GetLastError(), TEXT("FtpOpenFile"));
		InternetCloseHandle(FTPRequestDownload);
		return false;
	}

	if (FTPRequestDownload)
	{
		DWORD	dwContentLength;
		//unsigned long	iBytesRead; --- объявление перенесено выше!
		BYTE	Buffer[8192];
		ZeroMemory(Buffer, sizeof(Buffer));
		dwContentLength = 4 * 1024;

		FILE *pFile = _tfopen(file, TEXT("wb"));
		if (pFile == NULL)
			return false;
		do
		{
			if (InternetReadFile(FTPRequestDownload, Buffer, dwContentLength, &iBytesRead))
			{
				currentFileSize += iBytesRead;
				//выводим шкалу общего скачивания (new)
				if (_TotalSkinProgress && totalSize > 0) {
					showPercentTotalStr(iBytesRead, totalSize);
				}
				fwrite(Buffer, iBytesRead, 1, pFile);
				ZeroMemory(Buffer, sizeof(Buffer));
			}
		} while (iBytesRead != 0);
		fclose(pFile);
	}

	InternetCloseHandle(FTPRequestDownload);

	bool result = false;
	if ((fileSize == 0 && totalSize == 0) || isTxt)
		result = true;
	else
		result = (currentFileSize == fileSize);

	return result;
}

void CFtpConnect::showPercentTotalStr(unsigned long iBytesRead, LONG64 totalSize)
{
	g_downSize += iBytesRead;
	double ts = (double)totalSize;
	double ds = (double)g_downSize;
	double num = ds / ts;
	int proc = (int)round(num * 100);

	LONG64 nowTime = GetTickCount();
	if (lastTime == 0) {
		lastTime = GetTickCount();
	}
	if (nowTime - lastTime > 1000) {
		string str = to_string(proc) + "%";
		wstring ws = wide_string(str, loc);
		LPCWSTR lp = ws.c_str();
		_percentText->SetWindowTextW(lp);
		LONG64 diff = totalSize / 1024 - g_downSize / 1024;
		diff = diff / 1024;
		if (_TotalSkinProgress) {
			int limit = _TotalSkinProgress->GetUpper();
			limit = limit - (int)diff;
			_TotalSkinProgress->SetPos(limit);
		}
		str = "Осталось загрузить " + to_string(diff) + _MB;
		LONG64 kb_in_s = lastDownload / 1024;//Кб в секунду
		string s = " (скорость " + to_string(kb_in_s) + _KB_S + ")";
		str = str + s;
		ws = wide_string(str, loc);
		lp = ws.c_str();
		_speedText->SetWindowTextW(lp);
		lastTime = 0;
		lastDownload = 0;
	}
	else lastDownload += iBytesRead;
}

void CFtpConnect::setTotalProgress(CSkinProgress *totalProg)
{
	g_downSize = 0;
	_TotalSkinProgress = totalProg;
}

DWORDLONG CFtpConnect::getDownSize()
{
	return g_downSize;
}