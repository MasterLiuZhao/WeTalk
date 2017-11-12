#include <iostream>
#include <string>
//#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "resource.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_IP "192.168.0.209"
#define DEFAULT_PORT "2222"

SOCKET ConnectSocket = INVALID_SOCKET;

HINSTANCE g_hInstance = 0;
HWND registerHWnd;
HWND mainHWnd;
HANDLE g_hOutput = 0;
wchar_t* output_error_message = { 0 };
DWORD newThreadId = 0;
HANDLE hThread1 = NULL;

wchar_t username[100] = {};

void char_to_wchar(char* c_char, wchar_t* wchar){
	int length = strlen(c_char) + 1;
	wchar = (wchar_t*)malloc(sizeof(wchar_t) * length);
	memset(wchar, 0, length * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, c_char, strlen(c_char), wchar, length);
}

void wchar_to_char(wchar_t* wchar, char* c_char){
	/*
	size_t len = wcslen(wchar) + 1;
	size_t converted = 0;
	c_char = (char*)malloc(len*sizeof(char));
	wcstombs_s(&converted, c_char, len, wchar, _TRUNCATE);
	*/
	
	size_t len = wcslen(wchar) + 1;
	size_t converted = 0;
	//char* send_char;
	c_char = (char*)malloc(len * sizeof(char));
	wcstombs_s(&converted, c_char, len, wchar, _TRUNCATE);
}


LRESULT CALLBACK WindowProcess_Register(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam){

	switch (nMsg){
		case WM_CREATE:
			{
				HWND hUsername = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER , 100, 100, 200, 40, hWnd, (HMENU)1003, g_hInstance, NULL);//ES_PASSWORD | ES_AUTOHSCROLL | | ES_AUTOVSCROLL  | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_READONLY

				CreateWindowEx(0, L"BUTTON", L"Login", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 150, 80, 30, hWnd, (HMENU)1004, g_hInstance, NULL);
			}
			break;
		case WM_SIZE:
			//on_size(hWnd, wParam, lParam);
			break;
		case WM_COMMAND:
			{
				switch (wParam){
					case 1004:
						if (HIWORD(wParam) == BN_CLICKED){
							HWND hUsername = GetDlgItem(registerHWnd, 1003);

							LRESULT nLen = SendMessage(hUsername, WM_GETTEXTLENGTH, 0, 0);

							wchar_t* text = new wchar_t[nLen + 256];

							SendMessage(hUsername, WM_GETTEXT, nLen + 1, (LPARAM)text);

							wcscpy_s(username, 100, text);

							DestroyWindow(registerHWnd);

							//WriteConsole(g_hOutput, text, wcslen(text), NULL, NULL);
						}
						break;
				}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

int register_function(){
	WNDCLASSEX wce = { 0 };
	wce.cbSize = sizeof(wce);
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.hCursor = NULL;
	wce.hIcon = NULL;
	wce.hIconSm = NULL;
	wce.hInstance = g_hInstance;
	wce.lpfnWndProc = WindowProcess_Register;
	wce.lpszClassName = L"Register";
	wce.lpszMenuName = NULL;
	wce.style = CS_HREDRAW | CS_VREDRAW;

	ATOM nAtom = RegisterClassEx(&wce);
	if (nAtom == 0){
		MessageBox(NULL, L"Register Error", L"Error", MB_OK);
		return -1;
	}

	registerHWnd = CreateWindowEx(0, L"Register", L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);

	ShowWindow(registerHWnd, SW_SHOW);

	UpdateWindow(registerHWnd);


	MSG nMsg = { 0 };
	while (GetMessage(&nMsg, NULL, 0, 0)){
		TranslateMessage(&nMsg);
		DispatchMessage(&nMsg);
	}

}

bool Create_Socket(){
	WSADATA wsaData;

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	//char* sendbuf = new char[DEFAULT_BUFLEN];
	//char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	//int recvbuflen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0){
		//printf("WSAStartup failed with error: %d\n", iResult);
		output_error_message = L"WSAStartup failed with error: %d\n";
		WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0){
		//printf("getaddrinfo failed with error: %d\n", iResult);
		output_error_message = L"getaddrinfo failed with error: %d\n";
		WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		WSACleanup();
		return false;
	}

	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET){
		//printf("socket failed with error: %ld\n", WSAGetLastError());
		output_error_message = L"socket failed with error: %ld\n";
		WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		WSACleanup();
		return false;
	}

	iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR){
		//printf("Unable to connect to server!\n");
		output_error_message = L"Unable to connect to server!\n";
		WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	return true;
}

DWORD CALLBACK ReceiveMessageFromService(LPVOID pParam){
	char* pszText = (char*)pParam;

	Create_Socket();

	while (true){

		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;

		int iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0){
			recvbuf[iResult - 1] = '\0';
			//printf("bytes received: %d\n", iResult);
			//printf("Message come from Server: %s\n", recvbuf);

			size_t len = strlen(recvbuf) + 1;
			size_t converted = 0;
			wchar_t* recvbuf_wchar;
			recvbuf_wchar = (wchar_t*)malloc(len * sizeof(wchar_t));
			mbstowcs_s(&converted, recvbuf_wchar, len, recvbuf, _TRUNCATE);

			HWND hView = GetDlgItem(mainHWnd, 1001);

			LRESULT nLen = SendMessage(hView, WM_GETTEXTLENGTH, 0, 0);

			wchar_t* text = new wchar_t[nLen + 256];

			SendMessage(hView, WM_GETTEXT, nLen + 1, (LPARAM)text);

			wchar_t return_key[] = L"\r\n";

			wcscat_s(text, nLen + 256, return_key);

			wcscat_s(text, nLen + 256, recvbuf_wchar);

			SendMessage(hView, WM_SETTEXT, 0, (LPARAM)text);


			delete[] text;
			text = NULL;

			/*
			output_error_message = L"bytes received: %d\n";
			WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);

			output_error_message = L"Message come from Server: %s\n";
			WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
			WriteConsole(g_hOutput, recvbuf_wchar, wcslen(recvbuf_wchar), NULL, NULL);
			*/
		}else if (iResult == 0){
			//printf("connect closed...\n");
			output_error_message = L"connect closed...\n";
			WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		}else{
			//printf("recv failed with error: %d\n", WSAGetLastError());
			output_error_message = L"recv failed with error: %d\n";
			WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		}

		Sleep(1000);
	}

	ExitThread(0);

	printf("I'm finish \n");

	//return 0;
}

void on_create(HWND hWnd, WPARAM wParam, LPARAM lParam){
	HWND hView = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_READONLY, 0, 0, 200, 200, hWnd, (HMENU)1001, g_hInstance, NULL);//ES_PASSWORD | ES_AUTOHSCROLL | | ES_AUTOVSCROLL 

	HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE, 0, 0, 200, 200, hWnd, (HMENU)1002, g_hInstance, NULL);//ES_PASSWORD | ES_AUTOHSCROLL | | ES_AUTOVSCROLL 

	//HFONT hFont = CreateFont(100, 0, 0, 0, 900, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"»ªÎÄ²ÊÔÆ");

	//SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, 1);
}

void on_size(HWND hWnd, WPARAM wParam, LPARAM lParam){
	int nWidth = LOWORD(lParam);
	int nHight = HIWORD(lParam);

	HWND hView = GetDlgItem(hWnd, 1001);
	HWND hEdit = GetDlgItem(hWnd, 1002);

	MoveWindow(hView, 0, 0, nWidth, nHight - (nHight / 4), true);
	MoveWindow(hEdit, 0, nHight - 100, nWidth, nHight / 4, true);
}

void on_command(HWND hWnd, WPARAM wParam, LPARAM lParam){
	switch (LOWORD(wParam)){
		case EN_CHANGE:

			break;
	}
}

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam){

	switch (nMsg){
		case WM_CREATE:
			on_create(hWnd, wParam, lParam);
			break;
		case WM_SIZE:
			on_size(hWnd, wParam, lParam);
			break;
		case WM_CTLCOLOREDIT:
			{
				/*
				HWND hEdit = GetDlgItem(hWnd, 1002);

				LRESULT nLen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);

				wchar_t text[256] = { 0 };
				SendMessage(hEdit, WM_GETTEXT, nLen + 1, (LPARAM)text);

				size_t len = wcslen(text) + 1;
				size_t converted = 0;
				char* c_char;
				c_char = (char*)malloc(len * sizeof(char));
				wcstombs_s(&converted, c_char, len, text, _TRUNCATE);

				//WriteConsole(g_hOutput, c_char, strlen(c_char), NULL, NULL);
				*/
			}
			break;
		case WM_COMMAND:
			on_command(hWnd, wParam, lParam);
			break;
		case WM_DESTROY:

			//char send_char[256] = {};
			wchar_t* send_wchar = new wchar_t[256];

			wcscpy_s(send_wchar, 256, username);
			wcscat_s(send_wchar, 256, L"#");
			wcscat_s(send_wchar, 256, L"exit");

			size_t len = wcslen(send_wchar) + 1;
			size_t converted = 0;
			char* send_char;
			send_char = (char*)malloc(len * sizeof(char));
			wcstombs_s(&converted, send_char, len, send_wchar, _TRUNCATE);

			int iResult = send(ConnectSocket, send_char, strlen(send_char), 0);
			if (iResult == SOCKET_ERROR){
				//printf("send failed with error: %d\n", WSAGetLastError());
				output_error_message = L"send failed with error: %d\n";
				WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
				closesocket(ConnectSocket);
				WSACleanup();
			}

			delete[] send_wchar;

			closesocket(ConnectSocket);
			WSACleanup();
			CloseHandle(hThread1);

			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	g_hInstance = hInstance;

	/*
	bool result = AllocConsole();
	if (result == false){
		FreeConsole();
		AllocConsole();
	}

	g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	*/

	bool result = register_function();
	if (result == -1){
		return result;
	}
	

	WNDCLASSEX wce = { 0 };
	wce.cbSize = sizeof(wce);
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.hCursor = NULL;
	wce.hIcon = NULL;
	wce.hIconSm = NULL;
	wce.hInstance = hInstance;
	wce.lpfnWndProc = WindowProcess;
	wce.lpszClassName = L"Main";
	wce.lpszMenuName = NULL;
	wce.style = CS_HREDRAW | CS_VREDRAW;

	ATOM nAtom = RegisterClassEx(&wce);
	if (nAtom == 0){
		MessageBox(NULL, L"Register Error", L"Error", MB_OK);
		return -1;
	}

	mainHWnd = CreateWindowEx(0, L"Main", L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(mainHWnd, SW_SHOW);

	UpdateWindow(mainHWnd);

	//WriteConsole(g_hOutput, username, wcslen(username), NULL, NULL);

	
	hThread1 = CreateThread(NULL, 0, ReceiveMessageFromService, "No Parm", 0, &newThreadId);
	
	MSG nMsg = { 0 };
	while (GetMessage(&nMsg, NULL, 0, 0)){

		if (nMsg.message == WM_KEYDOWN && VK_RETURN == nMsg.wParam){
			HWND hEdit = GetDlgItem(mainHWnd, 1002);

			LRESULT nLen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);

			wchar_t* send_wchar = new wchar_t[nLen + 256];
			wchar_t* text = new wchar_t[nLen + 256];
			SendMessage(hEdit, WM_GETTEXT, nLen + 1, (LPARAM)text);

			wcscpy_s(send_wchar, nLen + 256, username);
			wcscat_s(send_wchar, nLen + 256, L"#");
			wcscat_s(send_wchar, nLen + 256, text);

			WriteConsole(g_hOutput, send_wchar, wcslen(send_wchar), NULL, NULL);

			//SendMessage(hEdit, EM_UNDO, 0, 0);

			SetWindowText(hEdit, L"");

			size_t len = wcslen(send_wchar) + 1;
			size_t converted = 0;
			char* send_char;
			send_char = (char*)malloc(len * sizeof(char));
			wcstombs_s(&converted, send_char, len, send_wchar, _TRUNCATE);


			int iResult = send(ConnectSocket, send_char, strlen(send_char), 0);
			if (iResult == SOCKET_ERROR){
				//printf("send failed with error: %d\n", WSAGetLastError());
				output_error_message = L"send failed with error: %d\n";
				WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}

			delete[] send_wchar;
			delete[] text;

		}

		TranslateMessage(&nMsg);
		DispatchMessage(&nMsg);
	}

	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR){
		//printf("shutdown failed with error: %d\n", WSAGetLastError());
		output_error_message = L"shutdown failed with error: %d\n";
		WriteConsole(g_hOutput, output_error_message, wcslen(output_error_message), NULL, NULL);
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ConnectSocket);
	WSACleanup();
	CloseHandle(hThread1);

	return 0;
}