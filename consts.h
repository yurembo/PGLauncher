// author: Yuriy Yazev, july 2015
#pragma once

#include <string>

using namespace std;

const string _appName = "PGLauncher";
const string _ipAddr = "";
const string _domen_name = "";

const string _userName = ""; 

const string _userPassword = ""; 

const string _domen = "";

const string _ftp_folder = ""; 

const string _dirName = "";

const string _infoFileName = "info.txt";
const string _programName = "FileData.exe";
//название файла, содержащего путь к папке игры
const string _directoryGame = "directory.txt";
//ключевое слово-идентификатор пути
const string _word_path = "dir_path";
//строка для вывода надписи
const string _folderToInstall = "Папка для установки";
//строки обозначения места на диске
const string _needSpace = "Требуется места: ";
const string _freeSpace = "Свободное место: ";
const string _MB = "МБ";
const string _KB_S = "Кб/сек";
//задержка между попытками скачивания файлов 
const int _timeToDelay = 3000;
//задержка между проверкоами переменной пути
const int _timeBetweenCheck = 300;

const int winWidth = 800;
const int winHeight = 600;

const int progressWidth = 737;
const int progressHeight = 23;

//для загрузки для кнопок текстур русского или английского языка, надо раскомментировать соответствующий идентификатор и закомментировать второй
//#define _ENG 355
#define _RUS 356

//строка для вывода в диалоге выбора папки
const wstring _lineChooseFolder = L"Выбери папку для установки игры";