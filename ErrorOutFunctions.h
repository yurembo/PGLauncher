#pragma once

#define  INET_ERR_OUT_MSG_BOX_BUFFER_SIZE      512
#define  INET_ERR_OUT_FORMAT_BUFFER_SIZE       256

#ifdef  _UNICODE
#define _itot _itow_s
#else
#define _itot _itoa_s
#endif

// Forward declaration of helper function
void WINAPI addLastErrorToMsg(LPTSTR szMsgBuffer, DWORD dwSize);

// Function for displaying Internet Error information in a message box
BOOL WINAPI InternetErrorOut(
	HWND hWnd,
	DWORD dwError,
	LPCTSTR szFailingFunctionName)
{
	HANDLE hProcHeap;
	TCHAR  szMsgBoxBuffer[INET_ERR_OUT_MSG_BOX_BUFFER_SIZE],
		szFormatBuffer[INET_ERR_OUT_FORMAT_BUFFER_SIZE],
		szConnectiveText[] = { TEXT("\nAdditional Information: ") },
		*szExtErrMsg = NULL,
		*szCombinedErrMsg = NULL;
	DWORD  dwInetError,
		dwBaseLength,
		dwExtLength = 0;

	if ((hProcHeap = GetProcessHeap()) == NULL)
	{
		StringCchCopy(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
			TEXT("Call to GetProcessHeap( ) failed..."));
		goto InetErrorOutError_1;
	}

	dwBaseLength = FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE,             // dwFlags
		GetModuleHandle(TEXT("wininet.dll")),  // lpSource
		dwError,                                 // dwMessageId
		0,                                       // dwLanguageId
		(LPTSTR)szFormatBuffer,                  // lpBuffer
		INET_ERR_OUT_FORMAT_BUFFER_SIZE,         // nSize
		NULL);                                  // * arguments

	if (!dwBaseLength)
	{
		StringCchCopy(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
			TEXT("Call to FormatMessage( ) failed..."));
		addLastErrorToMsg(szMsgBoxBuffer,
			INET_ERR_OUT_MSG_BOX_BUFFER_SIZE);
		goto InetErrorOutError_1;
	}

	if (FAILED(StringCchPrintf(szMsgBoxBuffer,
		INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
		TEXT("%s error; code: %d\nDescription: %s\n"),
		szFailingFunctionName, dwError, szFormatBuffer)))
	{
		StringCchCopy(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
			TEXT("Call to StringCchPrintf( ) failed...\n"));
		goto InetErrorOutError_1;
	}

	StringCchLength(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
		(size_t*)&dwBaseLength);
	// Adjust base-length value to count the number of bytes:
	dwBaseLength *= sizeof(TCHAR);

	if (dwError == ERROR_INTERNET_EXTENDED_ERROR)
	{
		InternetGetLastResponseInfo(&dwInetError, NULL, &dwExtLength);
		// Adjust the extended-length value to a byte count 
		// that includes the terminating null:
		++dwExtLength *= sizeof(TCHAR);
		if ((szExtErrMsg = (TCHAR*)HeapAlloc(hProcHeap, 0, dwExtLength))
			== NULL)
		{
			StringCchCat(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
				TEXT("\nFailure: Could not allocate buffer for addional details."));
			addLastErrorToMsg(szMsgBoxBuffer,
				INET_ERR_OUT_MSG_BOX_BUFFER_SIZE);
			goto InetErrorOutError_1;
		}

		if (!InternetGetLastResponseInfo(&dwInetError,
			szExtErrMsg,
			&dwExtLength))
		{
			StringCchCat(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
				TEXT("\nCall to InternetGetLastResponseInfo( ) failed--"));
			addLastErrorToMsg(szMsgBoxBuffer,
				INET_ERR_OUT_MSG_BOX_BUFFER_SIZE);
			goto InetErrorOutError_2;
		}
		dwBaseLength += dwExtLength + sizeof(szConnectiveText);
	}

	if ((szCombinedErrMsg = (TCHAR*)HeapAlloc(hProcHeap, 0,
		dwBaseLength + sizeof(TCHAR)))
		== NULL)
	{
		StringCchCat(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
			TEXT("\nFailure: Could not allocate final output buffer."));
		addLastErrorToMsg(szMsgBoxBuffer,
			INET_ERR_OUT_MSG_BOX_BUFFER_SIZE);
		goto InetErrorOutError_2;
	}

	if (FAILED(StringCchCopy(szCombinedErrMsg,
		dwBaseLength, szMsgBoxBuffer)) ||
		(dwExtLength &&
		(FAILED(StringCchCat(szCombinedErrMsg,
		dwBaseLength, szConnectiveText)) ||
		FAILED(StringCchCat(szCombinedErrMsg,
		dwBaseLength, szExtErrMsg)))))
	{
		StringCchCat(szMsgBoxBuffer, INET_ERR_OUT_MSG_BOX_BUFFER_SIZE,
			TEXT("\nFailure: Could not assemble final message."));
		goto InetErrorOutError_3;
	}

	MessageBox(hWnd,
		szCombinedErrMsg,
		TEXT("Internet Error Message"),
		MB_OK | MB_ICONERROR);
	HeapFree(hProcHeap, 0, szExtErrMsg);
	HeapFree(hProcHeap, 0, szCombinedErrMsg);

	return(TRUE);

InetErrorOutError_3:
	HeapFree(hProcHeap, 0, szCombinedErrMsg);
InetErrorOutError_2:
	HeapFree(hProcHeap, 0, szExtErrMsg);
InetErrorOutError_1:
	MessageBox(hWnd,
		(LPCTSTR)szMsgBoxBuffer,
		TEXT("InternetErrorOut( ) Failed"),
		MB_OK | MB_ICONERROR);
	return(FALSE);
}


void WINAPI addLastErrorToMsg(LPTSTR szMsgBuffer, DWORD dwSize)
{
	TCHAR szNumberBuffer[32];

	_itot(GetLastError(), szNumberBuffer, 10);
	StringCchCat(szMsgBuffer, dwSize,
		TEXT("\n   System Error number: "));
	StringCchCat(szMsgBuffer, dwSize, szNumberBuffer);
	StringCchCat(szMsgBuffer, dwSize, TEXT(".\n"));
}

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}