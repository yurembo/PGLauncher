// author: Yuriy Yazev, july 2015
//структура для хранения сведений о скачиваемом файле
#pragma once

#include <windows.h>
#include "functions.h"

struct FileDownData
{
	string filePath;
	ULARGE_INTEGER fileSizeBytes;
};