// author: Yuriy Yazev, july 2015
#include "stdafx.h"
#include "FileReader.h"
#include "functions.h"
#include <fstream>
#include <boost/crc.hpp>  // for boost::crc_32_type

streamsize const buffer_size = 4194304;//4mb

CFileReader::CFileReader()
{
	g_totalSize = 0;
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	m_UpdateText = nullptr;
	m_EnterButton = nullptr;
}

CFileReader::CFileReader(CWnd *hwin)
{
	hWin = hwin;
	g_totalSize = 0;
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	m_UpdateText = nullptr;
	m_EnterButton = nullptr;
}


CFileReader::~CFileReader()
{
	g_totalSize = 0;
	_TotalSkinProgress = nullptr;
	_percentText = nullptr;
	_speedText = nullptr;
	m_UpdateText = nullptr;
	m_EnterButton = nullptr;
}


bool CFileReader::readTotalFilesSize(string file)
{
	g_totalSize = getTotalSize(file);
	if (g_totalSize == 0) return false;
	return true;
}

DWORDLONG CFileReader::getTotalSize(string file)
{
	DWORDLONG totalSizeNum = 0;
	ifstream f;
	f.open(file, ios::in);

	if (file != "" && f.is_open()) {
		while (!f.eof()) {
			string filePath;
			f >> filePath;
			getWholeStr(f, filePath);//результат во втором параметре
			string fms;
			f >> fms;
			string fsize;
			f >> fsize;
			if (filePath == "Files_count:") {
				string totalSize;
				f >> totalSize;
				totalSizeNum = stringToDWORDLONG(totalSize);
				break;
			}
		}
	}
	else {
		f.close();
		return 0;
	}

	f.close();
	return totalSizeNum;
}

void CFileReader::getWholeStr(ifstream &fin, string &res)
{
	int pos = 0;
	if ((pos = res.find(':')) != string::npos) {
		return;
	}
	res.erase(0, 1);
	if ((pos = res.find('"')) != string::npos) {
		res.erase(pos, 1);
		return;
	}

	while (!fin.eof()) {
		bool exit = false;
		string word;
		fin >> word;
		pos = 0;
		if ((pos = word.find('"')) != string::npos) {
			word.erase(pos, 1);
			exit = true;
		}
		res = res + ' ' + word;
		if (exit) break;
	}
}

void CFileReader::readLocalFile(CFtpConnect& connect)
{
	bool reCheck = false;//надо перезапустить скачивание файла
	wstring lpath;
	string s = _infoFileName;
	if (!readTotalFilesSize(s)) 
		return;
	if (_TotalSkinProgress != nullptr && g_totalSize > 0) {
		int sizeMB = int(g_totalSize / 1024 / 1024);
		if (sizeMB > 0)
			_TotalSkinProgress->SetRange(0, sizeMB, 1);
	}
	ifstream f;
	f.open(s, ios::in);
	if (s != "" && f.is_open()) {
		while (!f.eof()) {
			SetCurrentDirectory(_CurrentFolder.c_str());
			string filePath;
			f >> filePath;
			getWholeStr(f, filePath);//результат во втором параметре
			string fsize;
			f >> fsize;
			string fcrc;
			f >> fcrc;
			// обработка файлового пути
			string fileName = filePath;
			string dirName = "";
			getFileNameAndDirName(fileName, dirName);
		
			if (filePath == "Files_count:") {
				break;
			}

			//обработка файлового размера
			DWORDLONG dwlsize = stringToDWORDLONG(fsize);
			//обработка контрольной суммы
			DWORDLONG dwlcrc = stringToDWORDLONG(fcrc);
			//обработка каталога и файла
			wstring stemp = wide_string(dirName, loc);
			LPCWSTR dir = stemp.c_str();
			wstring stemp1 = wide_string(fileName, loc);
			LPCWSTR file = stemp1.c_str();
			wstring stemp2 = wide_string(filePath, loc);
			LPCWSTR path = stemp2.c_str();
			if (!fileSearch(dir, file, path, connect, dwlcrc, dwlsize)) {
				reCheck = true;
				break;
			}
			lpath = path;
			//MessageBox(hWin->GetSafeHwnd(), lpath, _T(""), MB_OK);
		}
		f.close();
		
//		if (g_totalSize > connect.getDownSize()) {
			// - вывод сообщени€: количество байт которое надо скачать и количество скаченных байт
// 			ULONGLONG global_TotalSize = g_totalSize;
// 			ULONGLONG global_DownSize = connect.getDownSize();
// 			string s = to_string(global_TotalSize) + " == " + to_string(global_DownSize);
// 			wstring ws = wide_string(s, loc);
// 			LPCWSTR lp = ws.c_str();
// 			MessageBox(hWin->GetSafeHwnd(), lpath.c_str(), lp, MB_OK);
// 			 
// 			global_Download = true;//передаем выполнение дл€ другого потока
//		}
		if (reCheck) {
			global_Download = true;//передаем выполнение дл€ другого потока
		} else
			if (_TotalSkinProgress != nullptr){// && g_totalSize == connect.getDownSize()) {
				HideLabelsShowButton();
			}
	}
}

void CFileReader::HideLabelsShowButton()
{
	_TotalSkinProgress->SetPos(100);
	_percentText->SetWindowTextW(_T("100%"));
	_percentText->ShowWindow(SW_HIDE);
	_speedText->SetWindowTextW(_T(""));
	_speedText->ShowWindow(SW_HIDE);
	//скрываем прогресс-бар
	_TotalSkinProgress->ShowWindow(SW_HIDE);
	//скрываем текст
	m_UpdateText->ShowWindow(SW_HIDE);
	//показываем кнопку
	m_EnterButton->ShowWindow(SW_SHOW);
}

bool CFileReader::fileSearch(LPCWSTR dir, LPCWSTR file, LPCWSTR path, CFtpConnect& connect, DWORDLONG fileCRC /*= 0*/, DWORDLONG fileSize /*= 0*/)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;

	hf = FindFirstFile(path, &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		do {
				// использование структуры ULARGE_INTEGER дл€ нахождени€ 64-х битных значений, состо€щих из двух 32-х битных частей
				ULARGE_INTEGER us;
				us.HighPart = FindFileData.nFileSizeHigh;
				us.LowPart = FindFileData.nFileSizeLow;
				ULONGLONG lfileSize = us.QuadPart;
				UINT32 crc = calcCtrlSum(path);
				//сравнить, удалить и обновить
				if ((crc != fileCRC || lfileSize < fileSize) || (fileCRC == 0 && fileSize == 0)) {
					if (DeleteFile(path)) {
						BOOL boo = SetCurrentDirectory(dir);
						if (!downloadFile(path, connect, fileSize, g_totalSize))
							return false;
					}
					//else MessageBox(hWin->GetSafeHwnd(), _T("Ќе удалось удалить файл"), _T("Error"), 0);
				} 
				else
					if (fileSize > 0) {
						connect.showPercentTotalStr((long)fileSize, g_totalSize);
					}
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
		return true;
	}
	else {//если объект не найден
		if (dir != _T(""))
			createDirSeq(dir);
		if (!downloadFile(path, connect, fileSize, g_totalSize))
			return false;
	}
	return true;
}

UINT32 CFileReader::calcCtrlSum(LPCWSTR path)
{
	//вычисление контрольной суммы
	boost::crc_32_type crc_sum;
	ifstream ifs(path, ios_base::binary);
	if (ifs.is_open()) {
		char *buffer = new char[buffer_size];
		ifs.read(buffer, buffer_size);
		crc_sum.process_bytes(buffer, ifs.gcount());
		delete[] buffer;
	}
	//***
	UINT32 crc = crc_sum.checksum();

	return crc;
}

bool CFileReader::downloadFile(LPCWSTR path, CFtpConnect& connect, DWORDLONG fileSize, DWORDLONG totalSize)
{
	wstring ppath = path;
	bool isTXT = isThisTXT(ppath);
	bool isLauncherDir = (fileSize == 0 && totalSize == 0);
	bool res = false;
	if (_isWin10) {
		if (!isTXT) {
			ppath = insertFolderName(ppath, isLauncherDir);
			path = ppath.c_str();
			res = connect.GetFtpFileToCreate(path, fileSize, g_totalSize, isTXT);
		}
		else
		{
			ppath = insertFolderName(ppath, isLauncherDir);
			path = ppath.c_str();
			res = connect.GetFtpFileToCreate(path, fileSize, totalSize, isTXT);
		}
	}
	else 
	{
		if (!isTXT) {
			ppath = insertDomenName(ppath, isLauncherDir);
			path = ppath.c_str();
			res = connect.GetHttpFileToCreate(path, fileSize, g_totalSize);
		}
		else 
		{
			ppath = insertFolderName(ppath, isLauncherDir);
			path = ppath.c_str();
			res = connect.GetFtpFileToCreate(path, fileSize, totalSize);
		}
	}
	if (!res) {
 		//MessageBox(hWin->GetSafeHwnd(), path, _T("Ќеудача"), NULL);
	}
	return res;
}


void CFileReader::setTotalProgress(CSkinProgress* prog)
{
	_TotalSkinProgress = prog;
}

bool CFileReader::createDirSeq(LPCWSTR directory)//создать последовательность директорий
{
	string s = "\\";
	string str = narrow(directory);
	string::size_type start_pos = 0;
	string::size_type pos = 0;
	bool dirCreated = false;//была ли создана папка
	bool firstCall = true;//первый вызов
	while ((pos = str.find(s, start_pos)) != string::npos)
	{
		if (!firstCall) {
			pos++;
			start_pos = pos;
			pos = str.find(s, start_pos);
		}
		if (pos == string::npos)
			pos = str.length();
		else
			pos = pos - start_pos;
		string dir = str.substr(start_pos, pos);
		wstring ws = wide_string(dir, loc);
		createDir(ws.c_str());
		dirCreated = true;
		firstCall = false;
	}
	if (!dirCreated) {
		createDir(directory);
	}

	return true;
}

bool CFileReader::createDir(LPCWSTR directory)//создать каталог
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;

	hf = FindFirstFile(directory, &FindFileData);
	if (hf != INVALID_HANDLE_VALUE) {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			SetCurrentDirectory(directory);
		}
	}
	else {
		CreateDirectory(directory, NULL);
		SetCurrentDirectory(directory);
	}

	return true;
}

void CFileReader::setUpdateText(CStatic *upText)
{
	m_UpdateText = upText;
}

void CFileReader::setEnterButton(CClickImage *enterBut)
{
	m_EnterButton = enterBut;
}