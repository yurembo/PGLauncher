// author: Yuriy Yazev, july 2015
// PGLauncherDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "PGLauncher.h"
#include "PGLauncherDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <sstream>
#include "functions.h"
//#include "FileDownData.h"

//#include <vld.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "user32.lib")

using namespace std;

#ifdef dCSP_DIALOG_PROGRESS

// диалоговое окно CPGLauncherDlg
CPGLauncherDlg::CPGLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPGLauncherDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_PGLICON);

	pParent->GetDesktopWindow()->GetWindowRect(parentRect);

	m_TotalSkinProgress = nullptr;
}


CPGLauncherDlg::~CPGLauncherDlg()
{
	if (m_TotalSkinProgress != nullptr) {
		delete m_TotalSkinProgress;
		m_TotalSkinProgress = nullptr;
	}

	if (updateNameFont) {
		delete updateNameFont;
		updateNameFont = nullptr;
	}
	if (updateNameLogFont) {
		delete updateNameLogFont;
		updateNameLogFont = nullptr;
	}
	if (versionFont) {
		delete versionFont;
		versionFont = nullptr;
	}
	if (versionLogFont) {
		delete versionLogFont;
		versionLogFont = nullptr;
	}
	if (infoStringFont) {
		delete infoStringFont;
		infoStringFont = nullptr;
	}
	if (infoStringLogFont) {
		delete infoStringLogFont;
		infoStringLogFont = nullptr;
	}
	if (insFolderFont) {
		delete insFolderFont;
		insFolderFont = nullptr;
	}
	if (insFolderLogFont) {
		delete insFolderLogFont;
		insFolderLogFont = nullptr;
	}
	if (needSpaceFont) {
		delete needSpaceFont;
		needSpaceFont = nullptr;
	}
	if (needSpaceLogFont) {
		delete needSpaceLogFont;
		needSpaceLogFont = nullptr;
	}
	if (freeSpaceFont) {
		delete freeSpaceFont;
		freeSpaceFont = nullptr;
	}
	if (freeSpaceLogFont) {
		delete freeSpaceLogFont;
		freeSpaceLogFont = nullptr;
	}

	if (m_underBitmap != NULL) {
		delete m_underBitmap;
		m_underBitmap = nullptr;
	}
}


void CPGLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LINKIMAGE, _linkImage);//связывать контрол с переменной можно здесь или через получение указателя
	DDX_Control(pDX, IDC_ButtonClose, _butClose);
	DDX_Control(pDX, IDC_ButtonFold, _butFold);
	DDX_Control(pDX, IDC_Button_Enter, _butEnter);
	DDX_Control(pDX, IDC_SimpleProgressBar, m_SimpleBar);
	DDX_Control(pDX, IDC_Button_Change, _butChoose);
	DDX_Control(pDX, IDC_Button_Install, _butInstall);
//	DDX_Control(pDX, IDC_FILE_PROGRESS, _fileProgress);
//	DDX_Control(pDX, IDC_TOTAL_PROGRESS, _totalProgress);
	//для CStatic тут прописывать не надо
}

BEGIN_MESSAGE_MAP(CPGLauncherDlg, CDialogEx)//здесь надо объявить события
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DOWNLOAD_BUTTON, &CPGLauncherDlg::OnBnClickedDownloadButton)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()//сообщение перерисовки CStatic
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// обработчики сообщений CPGLauncherDlg

BOOL CPGLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	windowScreenCenter();
	//создание текстового поля
	RichEdit_Create();
	buildProgressBar();
	getLauncherFiles();
	PercentText_Create();
	SpeedText_Create();

	m_UpdateText = (CStatic*)GetDlgItem(IDC_UpdateName);
	m_EnterButton = (CClickImage*)GetDlgItem(IDC_Button_Enter);

	_isWin10 = (getWindowsVersion() >= 10);

	setTimer(_timeToDelay);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CPGLauncherDlg::setTimer(const int ms)
{
	timer = SetTimer(1, ms, NULL);//задержка перед запуском скачивания файлов
}

void CPGLauncherDlg::setTimerToCheck(const int ms)
{
	timerToCheck = SetTimer(2, ms, NULL);//задержка перед запуском скачивания файлов
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CPGLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CPGLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPGLauncherDlg::windowScreenCenter()
{
	//настойка окна
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	int x = parentRect.Width() / 2 - winWidth / 2;
	int y = parentRect.Height() / 2 - winHeight / 2;
	m_hWnd->MoveWindow(x, y, winWidth, winHeight);
	//настройка фона
	SetBackgroundImage(IDB_BACKGROUND);
}

void CPGLauncherDlg::OnBnClickedDownloadButton()
{
	_CurrentFolder = wide_string(_dirGameInstall, loc);

	showLabels();
	destroyProgressBar();
	buildProgressBar();
	
	_onEnterClicked = false;

	CWnd *windlg = GetForegroundWindow();
	if (windlg == nullptr) return;
	connect.createHTTPConnect();//создание HTTP соединения
	connect.createFTPConnect();//создание FTP соединения
	if (!connect.checkInternetConnect()) {
		global_Download = true;
		return;
	}
	connect.setWin(windlg);

	SetCurrentDirectory(_CurrentFolder.c_str());

	connect.setTotalProgress(m_TotalSkinProgress);
	connect.setPercentText(&m_PercentText);
	connect.setSpeedText(&m_SpeedText);
	wstring wfile = getWideFileName(_infoFileName);
	LPCWSTR file = wfile.c_str();
	if (connect.GetFtpFileToCreate(file, 0, 0)) {
		reader.setWin(windlg);
		reader.setTotalProgress(m_TotalSkinProgress);
		reader.setPercentText(&m_PercentText);
		reader.setSpeedText(&m_SpeedText);
		reader.setUpdateText(m_UpdateText);
		reader.setEnterButton(m_EnterButton);
		thread t(&CFileReader::readLocalFile, &reader, ref(connect));
		t.detach();
	}
}

void CPGLauncherDlg::showLabels()
{
	if (_doInstall) {
		m_PercentText.ShowWindow(SW_SHOW);
		m_SpeedText.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UpdateName)->ShowWindow(SW_SHOW);
	}
}

BEGIN_EVENTSINK_MAP(CPGLauncherDlg, CDialogEx)

END_EVENTSINK_MAP()

BOOL CPGLauncherDlg::PreTranslateMessage(MSG* pMsg)
{
 	return CWnd::PreTranslateMessage(pMsg);
}

//считать из потока ввода (файла) всю строку
wstring CPGLauncherDlg::getWholeLine(ifstream &f, string &fileName1)
{
	fileName1.erase(0, 1);
	wstring fullStr = wide_string(fileName1, loc);
	for (int i_time = 0; i_time < 100; ++i_time) {
		bool exit = false;
		f >> fileName1;
		int pos = 0;
		if ((pos = fileName1.find('"')) != string::npos) {
			fileName1.erase(pos, 1);
			exit = true;
		}
		fullStr = fullStr + WCHAR(' ') + wide_string(fileName1, loc);
		if (exit) break;
	}

	return fullStr;
}

void CPGLauncherDlg::getLauncherFiles()
{
	CWnd *hWin = GetForegroundWindow();
	if (hWin == nullptr) return;
	connect.setWin(hWin);
	connect.createHTTPConnect();
	connect.createFTPConnect();
	wstring path = insertFolderName(_T("config.txt"), true);
	LPCWSTR file = path.c_str();
	if (connect.GetFtpFileToCreate(file, 0, 0)) {
		reader.setWin(hWin);
		wstring wideFile = getFileNameWithoutDomen(file, _ftp_folder);
		LPCWSTR readFile = wideFile.c_str();
		readDirFile();
		//расположение кнопок
		turnButtons();
		//*****
		readLauncherFiles(connect, readFile);
	}
}

void CPGLauncherDlg::readDirFile()
{
	//проверить на наличие файла, содержащего путь к папке игры
	if (is_file_exist(_directoryGame)) {
		ifstream f;
		f.open(wide_string(_directoryGame, loc), ios::in);
		if (f.is_open()) {
			string word;
			f >> word;
			if (word == _word_path) {
				string dir_path = "";
				_dirGameInstall = "";
				while (!f.eof()) {
					f >> dir_path;
					_dirGameInstall += dir_path;
				}
				_doInstall = true;//передаем сигнал о начале скачивания/установки
			} else prepareSelectDir();
		} else prepareSelectDir();
	}
	else {
		prepareSelectDir();//отобразить гуи-элементы для выбора каталога
	}//else
}

void CPGLauncherDlg::prepareSelectDir()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	_dirGameInstall = narrow(buffer);
	_dirHomeExe = _dirGameInstall;
	makePathText(buffer);
	makeFolderToInstallLabel(wide_string(_folderToInstall, loc));
	makeNeedSpaceText();
	makeFreeSpaceText();
	calcDiskSpace();
	setTimerToCheck(_timeBetweenCheck);
}

void CPGLauncherDlg::calcDiskSpace()
{
	//скачивание файла для определения необходимого места
	string space = "";
	wstring wfile = getWideFileName(_infoFileName);
	LPCWSTR file = wfile.c_str();
	if (connect.GetFtpFileToCreate(file, 0, 0)) {
		DWORDLONG size = reader.getTotalSize(_infoFileName);
		size = size / 1024 / 1024;//MB
		if (size > 0) space = to_string(size);
	}
	wstring str = wide_string(_needSpace + space + _MB, loc);
	LPCWSTR lp = str.c_str();
	m_NeedSpace.SetWindowText(lp);
	//свободное место на диске
	space = "";
	__int64 size = getFreeSpace(_dirGameInstall);
	size = size / 1024 / 1024;
	if (size > 0) space = to_string(size);
	str = wide_string(_freeSpace + space + _MB, loc);
	lp = str.c_str();
	m_FreeSpace.SetWindowText(lp);
}

wstring CPGLauncherDlg::getWideFileName(string fn)
{
	wstring ffile = wide_string(fn, loc);
	LPCWSTR file = ffile.c_str();
	if (is_file_exist(fn)) {
		DeleteFile(file);
	}
	wstring path = insertFolderName(ffile, false);
	return path;
}

void CPGLauncherDlg::readLauncherFiles(CFtpConnect& connect, LPCWSTR fileTxt)
{
	ifstream f;
	f.open(fileTxt, ios::in);
	if (f.is_open()) {
		while (!f.eof()) {
			string fileMode;
			f >> fileMode;
			string fileName1;
			f >> fileName1;
			//чтение конфинурационного файла
			if (fileMode == "UpdateName") {
				makeUpdateNameLabel(getWholeLine(f, fileName1));
				continue;
			} else 
				if (fileMode == "Version") {
					makeVersionLabel(wide_string(fileName1, loc));
					continue;
				} else
					if (fileMode == "infoString") {
						makeInfoString(getWholeLine(f, fileName1));
						continue;
					} else
						if (fileMode == "exeFilePath") {
							_exeFilePath = wide_string(fileName1, loc);
							continue;
						}
			//обработка внешнего файла
			//fileName1 = "/~pgenomru/Launcher/" + fileName1;
			wstring stemp = wide_string(fileName1, loc);
			LPCWSTR file = stemp.c_str();
			reader.fileSearch(_T(""), file, file, connect, 0, 0);//значение 0 сигнализирует о том, что файл надо заменить
			if (fileMode == "main_image") {
				string url = "";
				f >> url;
				buildLinkImage(file, url);
			} else
			if (fileMode == "message") {
				ReadFile(file);
				RichEdit_DoTitle();//настройка заголовка
			} 

		}

	}
}

void CPGLauncherDlg::buildLinkImage(LPCWSTR file, string url)
{
	_linkImage.setImage(file);
	CImage img_link;
	img_link.Load(file);
	int width = img_link.GetWidth();
	int height = img_link.GetHeight();
	_linkImage.MoveWindow(20, 90, width, height);
	_linkImage.Url(url);
}

void CPGLauncherDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
	if (point.y > 0 && point.y < 25) {
		ReleaseCapture();
		::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);//перетаскивание окна
	}
}

void CPGLauncherDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
 	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPGLauncherDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (_butClose.Over_flag()) {
		modifyObj(&_butClose);
	}
	if (_butFold.Over_flag()) {
		modifyObj(&_butFold);
	}
	if (_butEnter.Over_flag()) {
		modifyObj(&_butEnter);
	}
	if (_butChoose.Over_flag()) {
		modifyObj(&_butChoose);
	}
	if (_butInstall.Over_flag()) {
		modifyObj(&_butInstall);
	}
}

void CPGLauncherDlg::modifyObj(CClickImage *img)
{
	img->setImage(img->Up_img());
	img->Over_flag(false);
}

void CPGLauncherDlg::turnButtons()
{
	_butClose.setImage(IDB_BUTCLOSEUP);
	CImage image;
	image.LoadFromResource(AfxGetInstanceHandle(), IDB_BUTCLOSEUP);
	int w = image.GetWidth();
	int h = image.GetHeight();
	_butClose.MoveWindow(winWidth - w, 0, w, h);
	_butClose.Up_img(IDB_BUTCLOSEUP);
	_butClose.Over_img(IDB_BUTCLOSEOVER);

	_butFold.setImage(IDB_BUTFOLDUP);
	image.Destroy();
	image.LoadFromResource(AfxGetInstanceHandle(), IDB_BUTFOLDUP);
	w = image.GetWidth();
	h = image.GetHeight();
	_butFold.MoveWindow(winWidth - w * 2, h / 2, w, h);
	_butFold.Up_img(IDB_BUTFOLDUP);
	_butFold.Over_img(IDB_BUTFOLDOVER);	

	_butEnter.setImage(IDB_BUTTON_ENTER);
	image.Destroy();
	image.LoadFromResource(AfxGetInstanceHandle(), IDB_BUTTON_ENTER);
	w = image.GetWidth();
	h = image.GetHeight();
	_butEnter.MoveWindow(winWidth / 2 - w / 2, int(winHeight - h * 1.3), w, h);
	_butEnter.Up_img(IDB_BUTTON_ENTER);
	_butEnter.Over_img(IDB_BUTTON_ENTER_OVER);
	_butEnter.ShowWindow(SW_HIDE);

	if (!_doInstall) {
		image.Destroy();
#ifdef _ENG 
		_butChoose.setImage(IDB_Button_Choose_Up_Eng);
		image.LoadFromResource(AfxGetInstanceHandle(), IDB_Button_Choose_Up_Eng);
		_butChoose.Up_img(IDB_Button_Choose_Up_Eng);
		_butChoose.Over_img(IDB_Button_Choose_Over_Eng);
		_butChoose.Press_img(IDB_Button_Choose_Down_Eng);
#endif
#ifdef _RUS
		_butChoose.setImage(IDB_Button_Choose_Up_Rus);
		image.LoadFromResource(AfxGetInstanceHandle(), IDB_Button_Choose_Up_Rus);
		_butChoose.Up_img(IDB_Button_Choose_Up_Rus);
		_butChoose.Over_img(IDB_Button_Choose_Over_Rus);
		_butChoose.Press_img(IDB_Button_Choose_Down_Rus);
#endif
		w = image.GetWidth();
		h = image.GetHeight();
		_butChoose.MoveWindow(winWidth / 2 + w / 2, winHeight - 87, w, h);
		_butChoose.ShowWindow(SW_SHOW);

		image.Destroy();
#ifdef _ENG 
		_butInstall.setImage(IDB_Button_Install_Up_Eng);
		image.LoadFromResource(AfxGetInstanceHandle(), IDB_Button_Install_Up_Eng);
		_butInstall.Up_img(IDB_Button_Install_Up_Eng);
		_butInstall.Over_img(IDB_Button_Install_Over_Eng);
		_butInstall.Press_img(IDB_Button_Install_Down_Eng);
#endif
#ifdef _RUS
		_butInstall.setImage(IDB_Button_Install_Up_Rus);
		image.LoadFromResource(AfxGetInstanceHandle(), IDB_Button_Install_Up_Rus);
		_butInstall.Up_img(IDB_Button_Install_Up_Rus);
		_butInstall.Over_img(IDB_Button_Install_Over_Rus);
		_butInstall.Press_img(IDB_Button_Install_Down_Rus);
#endif
		w = image.GetWidth();
		h = image.GetHeight();
		_butInstall.MoveWindow(winWidth / 2 + w * 2 + 10, winHeight - 87, w, h);
		_butInstall.ShowWindow(SW_SHOW);
	}
}

void CPGLauncherDlg::RichEdit_Create()
{
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	int x = parentRect.Width() / 2 - winWidth / 2;
	int y = parentRect.Height() / 2 - winHeight / 2;

	//ностройка текстового поля
	if (!m_RichEdit.Create(WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL /*| ES_READONLY*/,
		CRect(winWidth / 2 - 65, winHeight / 2 - 210, winWidth - 30, winHeight - 130), m_hWnd, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create RichEdit window\n");
		return;
	}
	//SetWindowLong(m_RichEdit.GetSafeHwnd(), GWL_EXSTYLE, WS_EX_CLIENTEDGE); //создадим рамку
	m_RichEdit.SetBackgroundColor(0, RGB(0, 0, 0));
	//настройка шрифта
	//Set the default font
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED;
	cfDefault.dwMask = CFM_BOLD | CFM_FACE | CFM_SIZE | CFM_CHARSET | CFM_PROTECTED | CFM_COLOR;
	cfDefault.yHeight = 180;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(186, 186, 186);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));
	m_RichEdit.SetDefaultCharFormat(cfDefault);

	//настройка скролл-бара
	CBitmap bmp;
	bmp.LoadBitmap(IDB_SCROLLBAR);
	BITMAP bm;
	bmp.GetBitmap(&bm);
	m_hBmpScrollBar = (HBITMAP)bmp.Detach();

	SkinWndScroll(&m_RichEdit, m_hBmpScrollBar);
}

void CPGLauncherDlg::RichEdit_DoTitle()//выделяет заголовок, который обременен кавычками
{
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED;
	cfDefault.dwMask = CFM_BOLD | CFM_FACE | CFM_SIZE | CFM_CHARSET | CFM_PROTECTED | CFM_COLOR;
	cfDefault.yHeight = 400;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(108, 191, 204);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));

	long pos1 = -1;
	long pos2 = -1;

	do {
		FINDTEXTEX ft1;
		ft1.chrg.cpMin = 0;
		ft1.chrg.cpMax = -1;
		ft1.lpstrText = _T("\"");

		pos1 = m_RichEdit.FindText(FR_DOWN | FR_FINDNEXT, &ft1);

		if (pos1 == -1) return;
		m_RichEdit.SetSel(ft1.chrgText);
		m_RichEdit.Clear();

		FINDTEXTEX ft2;
		ft2.chrg.cpMin = pos1;
		ft2.chrg.cpMax = -1;
		ft2.lpstrText = _T("\"");

		pos2 = m_RichEdit.FindText(FR_DOWN | FR_FINDNEXT, &ft2);

		if (pos2 == -1) return;
		m_RichEdit.SetSel(ft2.chrgText);
		m_RichEdit.Clear();

		if (pos1 > -1 && pos2 > -1) {
			m_RichEdit.SetSel(pos1, pos2);
			m_RichEdit.SetSelectionCharFormat(cfDefault);
		}
	} while (pos1 > -1 && pos2 > -1);

	m_RichEdit.SetReadOnly();
}

void CPGLauncherDlg::PercentText_Create()
{
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	int x = parentRect.Width() / 2 - winWidth / 2;
	int y = parentRect.Height() / 2 - winHeight / 2;

	//ностройка текстового поля
	if (!m_PercentText.CreateEx(WS_EX_TRANSPARENT, //делаем фон прозрачным
		WS_CHILD | WS_VISIBLE | ES_READONLY,
		CRect(winWidth - 75, winHeight - 105, winWidth - 37, winHeight - 90), m_hWnd, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create PercentText window\n");
		return;
	}
	m_PercentText.SetBackgroundColor(0, RGB(0, 0, 0));
	//настройка шрифта
	//Set the default font
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED;
	cfDefault.dwMask = CFM_BOLD | CFM_COLOR;
	cfDefault.yHeight = 180;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(108, 191, 204);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));
	m_PercentText.SetDefaultCharFormat(cfDefault);

	PARAFORMAT Pfm;
	Pfm.cbSize = sizeof(PARAFORMAT);
	Pfm.dwMask = PFM_ALIGNMENT;
	Pfm.wAlignment = PFA_RIGHT;
	m_PercentText.SetParaFormat(Pfm);

	if (_doInstall)
		m_PercentText.SetWindowTextW(_T("0%"));
}

void CPGLauncherDlg::SpeedText_Create()
{
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	int x = parentRect.Width() / 2 - winWidth / 2;
	int y = parentRect.Height() / 2 - winHeight / 2;

	//ностройка текстового поля
	if (!m_SpeedText.CreateEx(WS_EX_TRANSPARENT, //делаем фон прозрачным
		WS_CHILD | WS_VISIBLE | ES_READONLY,
		CRect(40, winHeight - 60, 500, winHeight - 30), m_hWnd, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create PercentText window\n");
		return;
	}
	m_SpeedText.SetBackgroundColor(0, RGB(0, 0, 0));
	//настройка шрифта
	//Set the default font
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED;
	cfDefault.dwMask = CFM_COLOR;
	cfDefault.yHeight = 170;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(108, 191, 204);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));
	m_SpeedText.SetDefaultCharFormat(cfDefault);

	PARAFORMAT Pfm;
	Pfm.cbSize = sizeof(PARAFORMAT);
	Pfm.dwMask = PFM_ALIGNMENT;
	Pfm.wAlignment = PFA_LEFT;
	m_SpeedText.SetParaFormat(Pfm);

	m_SpeedText.SetWindowTextW(_T("Осталось загрузить"));

	m_SpeedText.ShowWindow(SW_HIDE);
}


void CPGLauncherDlg::ReadFile(LPCWSTR FileName)
{
	// The file from which to load the contents of the rich edit control.
	CFile cFile(FileName, CFile::modeRead);
	EDITSTREAM es;

	es.dwCookie = (DWORD)&cFile;
	es.pfnCallback = (EDITSTREAMCALLBACK)MyStreamInCallback;

	m_RichEdit.StreamIn(SF_TEXT, es); // Perform the streaming
}

void CPGLauncherDlg::buildProgressBar()
{
	if (m_TotalSkinProgress == nullptr) {
		m_underBitmap = new CBitmap();
		m_underBitmap->LoadBitmapW(IDB_ProgressBar);
		CRect *rect = new CRect(winWidth / 2 - progressWidth / 2, winHeight - 85, winWidth / 2 - progressWidth / 2 + progressWidth, winHeight - 85 + progressHeight);
		m_SimpleBar.MoveWindow(rect);
		CWnd *wnd = &m_SimpleBar;
		m_TotalSkinProgress = new CSkinProgress(wnd, 500, m_underBitmap);
		m_TotalSkinProgress->MoveWindow(rect);
		m_TotalSkinProgress->ShowWindow(SW_HIDE);
		m_TotalSkinProgress->ModifyStyleEx(WS_EX_STATICEDGE, NULL, SWP_FRAMECHANGED);
		delete rect;
	}
	else {
		if (_doInstall)
			m_TotalSkinProgress->ShowWindow(SW_SHOW);
		else
			m_TotalSkinProgress->ShowWindow(SW_HIDE);
		m_TotalSkinProgress->SetPos(0);
		m_TotalSkinProgress->SetStep(1);
		m_TotalSkinProgress->SetStart(0);
	}
}

void CPGLauncherDlg::destroyProgressBar()
{
	//скрыть кнопку
	GetDlgItem(IDC_Button_Enter)->ShowWindow(SW_HIDE);
	//скрыть Progress Bar
	m_TotalSkinProgress->ShowWindow(SW_HIDE);
}

void CPGLauncherDlg::makeUpdateNameLabel(const wstring name)
{
	updateNameFont = new CFont();
	updateNameLogFont = new LOGFONT();

	memset(updateNameLogFont, 0, sizeof(updateNameLogFont));

	updateNameLogFont->lfHeight = 18;                    // 18-pixel-height
	updateNameLogFont->lfWeight = FW_BOLD;               // Bold
	updateNameLogFont->lfUnderline = FALSE;              // Underlined

	wcscpy(updateNameLogFont->lfFaceName, _T("Tahoma"));

	updateNameFont->CreateFontIndirect(updateNameLogFont);

	GetDlgItem(IDC_UpdateName)->SetFont(updateNameFont);

	LPCWSTR lp = name.c_str();
	GetDlgItem(IDC_UpdateName)->SetWindowTextW(lp);

	GetDlgItem(IDC_UpdateName)->MoveWindow(40, winHeight - 110, 300, 30);

	if (_doInstall)
		GetDlgItem(IDC_UpdateName)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_UpdateName)->ShowWindow(SW_HIDE);
}

void CPGLauncherDlg::makeVersionLabel(const wstring version)
{
	versionFont = new CFont();
	versionLogFont = new LOGFONT();

	memset(versionLogFont, 0, sizeof(versionLogFont));

	versionLogFont->lfHeight = 13;
	versionLogFont->lfUnderline = FALSE;  

	wcscpy(versionLogFont->lfFaceName, _T("Tahoma"));

	versionFont->CreateFontIndirect(versionLogFont);

	GetDlgItem(IDC_Version)->SetFont(versionFont);

	wstring ws = _T("Версия ") + version;
	LPCWSTR lp = ws.c_str();
	GetDlgItem(IDC_Version)->SetWindowTextW(lp);

	GetDlgItem(IDC_Version)->MoveWindow(winWidth - 80, winHeight - 16, 100, 30);
}

void CPGLauncherDlg::makeInfoString(const wstring info)
{
	infoStringFont = new CFont();
	infoStringLogFont = new LOGFONT();

	memset(infoStringLogFont, 0, sizeof(infoStringLogFont));

	infoStringLogFont->lfHeight = 11;

	wcscpy(infoStringLogFont->lfFaceName, _T("Tahoma"));

	infoStringFont->CreateFontIndirect(infoStringLogFont);

	GetDlgItem(IDC_InfoString)->SetFont(infoStringFont);

	LPCWSTR lp = info.c_str();
	GetDlgItem(IDC_InfoString)->SetWindowTextW(lp);

	GetDlgItem(IDC_InfoString)->MoveWindow(winWidth / 2 - 120, winHeight - 15, 400, 30);
}

void CPGLauncherDlg::makeFolderToInstallLabel(const wstring str)
{
	insFolderFont = new CFont();
	insFolderLogFont = new LOGFONT();

	memset(insFolderLogFont, 0, sizeof(insFolderLogFont));

	insFolderLogFont->lfHeight = 17;
	insFolderLogFont->lfWeight = FW_BOLD;

	wcscpy(insFolderLogFont->lfFaceName, _T("Tahoma"));

	insFolderFont->CreateFontIndirect(insFolderLogFont);

	GetDlgItem(IDC_FolderToInstall)->SetFont(insFolderFont);

	LPCWSTR lp = str.c_str();
	GetDlgItem(IDC_FolderToInstall)->SetWindowTextW(lp);

	GetDlgItem(IDC_FolderToInstall)->MoveWindow(20, winHeight - 110, 300, 50);
	GetDlgItem(IDC_FolderToInstall)->ShowWindow(SW_SHOW);
}

void CPGLauncherDlg::makeNeedSpaceText()
{
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	//ностройка текстового поля
	if (!m_NeedSpace.CreateEx(WS_EX_TRANSPARENT, //делаем фон прозрачным
		WS_CHILD | WS_VISIBLE | ES_READONLY,
		CRect(20, winHeight - 57, winWidth - 320, winHeight - 27), m_hWnd, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create m_NeedSpace window\n");
		return;
	}
	m_NeedSpace.SetBackgroundColor(0, RGB(0, 0, 0));
	//настройка шрифта
	//Set the default font
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED;
	cfDefault.dwMask = CFM_COLOR;
	cfDefault.yHeight = 140;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(179, 179, 179);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));
	m_NeedSpace.SetDefaultCharFormat(cfDefault);

	PARAFORMAT Pfm;
	Pfm.cbSize = sizeof(PARAFORMAT);
	Pfm.dwMask = PFM_ALIGNMENT;
	Pfm.wAlignment = PFA_LEFT;
	m_NeedSpace.SetParaFormat(Pfm);
}

void CPGLauncherDlg::makeFreeSpaceText()
{
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	//ностройка текстового поля
	if (!m_FreeSpace.CreateEx(WS_EX_TRANSPARENT, //делаем фон прозрачным
		WS_CHILD | WS_VISIBLE | ES_READONLY,
		CRect(20, winHeight - 38, 300, winHeight - 8), m_hWnd, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create m_NeedSpace window\n");
		return;
	}
	m_FreeSpace.SetBackgroundColor(0, RGB(0, 0, 0));
	//настройка шрифта
	//Set the default font
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED;
	cfDefault.dwMask = CFM_COLOR;
	cfDefault.yHeight = 140;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(179, 179, 179);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));
	m_FreeSpace.SetDefaultCharFormat(cfDefault);

	PARAFORMAT Pfm;
	Pfm.cbSize = sizeof(PARAFORMAT);
	Pfm.dwMask = PFM_ALIGNMENT;
	Pfm.wAlignment = PFA_LEFT;
	m_FreeSpace.SetParaFormat(Pfm);
}

void CPGLauncherDlg::makePathText(const wstring path)
{
	CWnd* m_hWnd = AfxGetApp()->GetMainWnd();
	int x = parentRect.Width() / 2 - winWidth / 2;
	int y = parentRect.Height() / 2 - winHeight / 2;

	//ностройка текстового поля
	if (!m_PathText.CreateEx(0,	WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY,
		CRect(20, winHeight - 85, 450, winHeight - 65), m_hWnd, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create PathText window\n");
		return;
	}
	m_PathText.SetBackgroundColor(0, RGB(176, 176, 176));
	//настройка шрифта
	//Set the default font
	CHARFORMAT cfDefault;
	cfDefault.cbSize = sizeof(cfDefault);
	cfDefault.dwEffects = CFE_PROTECTED | CFE_BOLD;
	cfDefault.dwMask = CFM_COLOR | CFM_BOLD | CFM_SIZE;
	cfDefault.yHeight = 200;
	cfDefault.bCharSet = 0x00;
	cfDefault.crTextColor = RGB(0,0,0);
	wcscpy(cfDefault.szFaceName, _T("Tahoma"));
	m_PathText.SetDefaultCharFormat(cfDefault);

	PARAFORMAT Pfm;
	Pfm.cbSize = sizeof(PARAFORMAT);
	Pfm.dwMask = PFM_ALIGNMENT;
	Pfm.wAlignment = PFA_LEFT;
	m_PathText.SetParaFormat(Pfm);

	LPCWSTR wide_path = path.c_str();
	m_PathText.SetWindowTextW(wide_path);

	//m_PathText.ShowWindow(SW_HIDE);
}

DWORD CALLBACK MyStreamInCallback(CFile* dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// Required for StreamIn
	CFile* pFile = (CFile*)dwCookie;

	*pcb = pFile->Read(pbBuff, cb);

	return 0;
}

//перерисовка надписей : изменение цвета : color change
HBRUSH CPGLauncherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	CBrush m_brush = m_brush.CreateStockObject(HOLLOW_BRUSH);
	
	switch (pWnd->GetDlgCtrlID())
	{
		case IDC_UpdateName :
			pDC->SetTextColor(RGB(108, 191, 204));
			pDC->SetBkMode(TRANSPARENT);
		break;
		case IDC_Version : 
			pDC->SetTextColor(RGB(179, 179, 179));
			pDC->SetBkMode(TRANSPARENT);
		break;
		case IDC_InfoString :
			pDC->SetTextColor(RGB(179, 179, 179));
			pDC->SetBkMode(TRANSPARENT);
		break;
		case IDC_FolderToInstall :
			pDC->SetTextColor(RGB(108, 191, 204));
			pDC->SetBkMode(TRANSPARENT);
		break;
	}

	DeleteObject(m_brush);

	return hbr;
}

void CPGLauncherDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {//скачивание файлов
		if (global_Download) {
			global_Download = false;
			if (_doInstall)
				OnBnClickedDownloadButton();
		}
		setTimer(_timeToDelay);
	} else
		if (nIDEvent == 2) {//проверка: заполнен ли путь
			if (!_doInstall) {
				CString dirIns;
				m_PathText.GetWindowTextW(dirIns);
				CString cstr(_dirGameInstall.c_str());
				if (cstr != dirIns) {
					wstring wstr = wide_string(_dirGameInstall, loc);
					m_PathText.SetWindowTextW(wstr.c_str());
					calcDiskSpace();
				}
			}
			else {
				KillTimer(timerToCheck);
				beginInstallAfterFolderSelect();
			}
		}
	CDialogEx::OnTimer(nIDEvent);
}

void CPGLauncherDlg::beginInstallAfterFolderSelect()
{
	_butChoose.ShowWindow(SW_HIDE);
	_butInstall.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FolderToInstall)->ShowWindow(SW_HIDE);
	m_NeedSpace.ShowWindow(SW_HIDE);
	m_FreeSpace.ShowWindow(SW_HIDE);
	m_PathText.ShowWindow(SW_HIDE);
	showLabels();
	m_PercentText.SetWindowTextW(_T("0%"));
	GetDlgItem(IDC_UpdateName)->ShowWindow(SW_SHOW);
	m_TotalSkinProgress->ShowWindow(SW_SHOW);
}

BOOL CPGLauncherDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialogEx::PreCreateWindow(cs);
}

//возвращает доступное для текущего пользователя свободное место на диске в байтах
__int64 CPGLauncherDlg::getFreeSpace(string path)
{
	if (path != "" && path.length() > 0) {
		char letter = path[0];
		stringstream ss;
		ss << letter;
		string drive = "";
		ss >> drive;
		drive = drive + ":\\";
		wstring disk = wide_string(drive, loc);
		LPCWSTR dir = disk.c_str();
		ULARGE_INTEGER bytes;
		bytes.QuadPart = 0;
		BOOL res = GetDiskFreeSpaceEx(dir, (PULARGE_INTEGER)&bytes, NULL, NULL);
		if (!res) return 0;
		return bytes.QuadPart;
	}
	return 0;
}

#endif // dCSP_DIALOG_PROGRESS