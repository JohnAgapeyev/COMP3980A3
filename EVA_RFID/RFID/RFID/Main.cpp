#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "main.h"

using namespace std;

int WINAPI WinMain(HINSTANCE hInst,
				   HINSTANCE hPrevInst,
				   LPSTR lspszCmdParam,
				   int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASSEX Wcl;

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL;                          // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);   // cursor style

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = "RFIDMENU"; // no class menu 
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl)) {
		return 0;
	}

	hwnd = CreateWindow(Name, // name of application
						Name, // title bar text
						WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, // not resizeable 
						CW_USEDEFAULT, CW_USEDEFAULT, // starting x ,y positions
						WIN_WIDTH, WIN_LENGTH, // 600 by 400 px 
						NULL,
						NULL, //menu bar
						hInst, // first aparam in WinMain
						NULL);


	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
						 UINT Message,
						 WPARAM wParam,
						 LPARAM lParam) {
	int response = 0;

	switch (Message) {
	case WM_CREATE:
		createGUI(hwnd);
		break;

	case WM_COMMAND:
		if ((HWND)lParam == hConnectButton) {
			controller.connectPort(hConnectButton, hDisconnectButton);
			break;
		}
		if ((HWND)lParam == hDisconnectButton) {
			controller.disconnectPort(hConnectButton, hDisconnectButton);
			break;
		}
		else {
			controller.configCommPortParams(wParam);
		}
		break;

	case WM_SYSKEYDOWN:
	case WM_CHAR:
	case WM_KEYDOWN:
		if ((char)wParam == 27) // if ESC is pressed
		{
			controller.disconnectPort(hConnectButton, hDisconnectButton);
		}
		break;
	case WM_PAINT:
		//controller.repaintDisplay();
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return response;
}

void createGUI(HWND hwnd) {
	tagDisplay = CreateWindow("STATIC",
							  NULL,
							  WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED,
							  TAG_DISPLAY_X,
							  TAG_DISPLAY_Y,
							  TEXTBOX_WIDTH,
							  TEXTBOX_HEIGTH,
							  hwnd,
							  NULL,
							  NULL,
							  NULL
							  );

	tagHistDisplay = CreateWindow("STATIC",
								  NULL,
								  WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED,
								  TAG_HISTORY_DISPLAY_X,
								  TAG_HISTORY_DISPLAY_Y,
								  TEXTBOX_WIDTH,
								  TEXTBOX_HEIGTH,
								  hwnd,
								  NULL,
								  NULL,
								  NULL
								  );

	hConnectButton = CreateWindow("BUTTON",
								  "CONNECT",
								  WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
								  CONNECT_BUTTON_X,
								  CONNECT_BUTTON_Y,
								  BUTTON_WIDTH,
								  BUTTON_HEIGHT,
								  hwnd,
								  NULL,
								  NULL,
								  NULL
								  );

	hDisconnectButton = CreateWindow("BUTTON",
									 "DISCONNECT",
									 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_DISABLED,
									 DISCONNECT_BUTTON_X,
									 DISCONNECT_BUTTON_Y,
									 BUTTON_WIDTH,
									 BUTTON_HEIGHT,
									 hwnd,
									 NULL,
									 NULL,
									 NULL
									 );
}
