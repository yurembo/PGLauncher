// author: Yuriy Yazev, july 2015
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h> 
#include "consts.h"
//#include <Shlobj.h>//для использования SHBrowseForFolder

using namespace std;

__declspec(selectany) bool _onEnterClicked = false;//была ли нажата кнопка Enter
__declspec(selectany) wstring _exeFilePath = L"";//переменная для хранения пути к выполняемому файлу
__declspec(selectany) wstring _CurrentFolder = L"";//папка, из которой все началось
__declspec(selectany) BOOL global_Download = true;//флаг, означающий необходимость скачивания файлов
__declspec(selectany) string _dirHomeExe = "";//папка расположения экзешника
__declspec(selectany) string _dirGameInstall = "";//путь к папке игры
__declspec(selectany) bool _doInstall = false;//можно ли начать установку
__declspec(selectany) bool _isWin10 = false;//флаг, если положительный, выполнение происходит на Windows 10 или выше

static char const *russian_locale_designator = "rus";
static locale loc(russian_locale_designator);

//функция преобразования wstring в string
static string narrow(wstring const& text)
{
	locale const loc("");
	wchar_t const* from = text.c_str();
	size_t const len = text.size();
	vector<char> buffer(len + 1);
	use_facet<ctype<wchar_t> >(loc).narrow(from, from + len, '_', &buffer[0]);
	string s = string(&buffer[0], &buffer[len]);
	return s;
}

//функция преобразования string в ULONGLONG
static DWORDLONG stringToDWORDLONG(const string cnum)
{
	return _strtoui64(cnum.c_str(), NULL, 0);
}

//функция для преобразования string в wstring
static std::wstring wide_string(std::string const &s, std::locale const &loc)
{
	if (s.empty())
		return std::wstring();
	std::ctype<wchar_t> const &facet = std::use_facet<std::ctype<wchar_t> >(loc);
	char const *first = s.c_str();
	char const *last = first + s.size();
	std::vector<wchar_t> result(s.size());

	facet.widen(first, last, &result[0]);

	return std::wstring(result.begin(), result.end());
}

//получить имя файла и папки
static void getFileNameAndDirName(string& fileName, string& dirName) 
{
	char sep = '\\';

	size_t i = fileName.rfind(sep, fileName.length());
	if (i != string::npos) {
		dirName = fileName.substr(0, i);
		fileName = fileName.substr(i + 1, fileName.length() - i);
	}
}

//получить путь и имя файла без доменного имени
static wstring getFileNameWithoutDomen(wstring wide_path, const string domen)
{
	
	int domen_length = domen.length();
	string path = narrow(wide_path);
	int symPos = path.find(domen, 0);//ищем доменное имя или папку
	if (symPos != string::npos) {
		int symCount = path.length() - domen_length;
		symPos = path.find("/", domen_length + 1);//ищем следующую папку, так как от нее тоже надо избавиться
		if (symPos != string::npos) {
			symCount = path.length() - symPos;
			path = path.substr(symPos + 1, symCount);
		}
	}
	wide_path = wide_string(path, loc);
	return wide_path;
}

//добавить в начало пути к файлу доменное имя с папкой (для HTTP подключения)
static wstring insertDomenName(wstring path, const bool bLauncher)
{
	string str = narrow(path);
	if (!bLauncher)
		str = _domen + _dirName + "/" + str;
	else 
		str = _domen + "Launcher" + "/" + str;
	path = wide_string(str, loc);
	return path;
}

//добавить в начало пути к файлу серверную папку (для FTP подключения)
static wstring insertFolderName(wstring path, const bool bLauncher)
{
	string str = narrow(path);
	if (!bLauncher)
		str = _ftp_folder + _dirName + "/" + str;
	else 
		str = _ftp_folder + "Launcher" + "/" + str;
	path = wide_string(str, loc);
	return path;
}

//по расширению файла определяет: текстовой, html он или нет
static bool isThisTXT(wstring fileName)
{
	bool res = false;
	size_t i = fileName.rfind('.', fileName.length());
	if (i != string::npos) {
		string ext;
		ext = narrow(fileName.substr(i + 1, fileName.length() - i));
		if (ext == "txt" || ext == "html" || ext == "htm")
			res = true;
	}
	else
		res = false;

	return res;
}

//существует ли указанный файл?
static bool is_file_exist(string fileName)
{
	ifstream infile(fileName);
	return infile.good();
}

//запустить программу
static void runApp()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	SetCurrentDirectory(_CurrentFolder.c_str());
	SetCurrentDirectory(_T(".."));//Look out!!!

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	LPCWSTR lp = _exeFilePath.c_str();
	// Start the child process. 
	if (!CreateProcess(lp,   //  module name
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		//DWORD dw = GetLastError();
		TRACE1("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

//открыть диалог выбора директории
static wstring chooseFolder(HWND *win)
{
	BROWSEINFO   bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR   szDisplayName[MAX_PATH];
	szDisplayName[0] = L'';

	bi.hwndOwner = *win;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _lineChooseFolder.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
	TCHAR   szPathName[MAX_PATH];
	if (NULL != pidl)
	{
		BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
		if (FALSE == bRet)
			return wide_string(_dirGameInstall, loc);
	}
	else return wide_string(_dirGameInstall, loc);

	wstring res = CT2W(szPathName);
	return res;
}

static DWORD getWindowsVersion()
{
	RTL_OSVERSIONINFOEXW *pk_OsVer = new RTL_OSVERSIONINFOEXW;
	typedef LONG(WINAPI* tRtlGetVersion)(RTL_OSVERSIONINFOEXW*);

	memset(pk_OsVer, 0, sizeof(RTL_OSVERSIONINFOEXW));
	pk_OsVer->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

	HMODULE h_NtDll = GetModuleHandleW(L"ntdll.dll");
	tRtlGetVersion f_RtlGetVersion = (tRtlGetVersion)GetProcAddress(h_NtDll, "RtlGetVersion");

	if (!f_RtlGetVersion)
		return FALSE; // This will never happen (all processes load ntdll.dll)

	LONG Status = f_RtlGetVersion(pk_OsVer);

	int ret = 0;

	if (Status == 0)
		ret = pk_OsVer->dwMajorVersion;

	delete pk_OsVer;

	return ret;
}