#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "main.h"

using namespace std;
deque<string>  tagHist; //holds a history of tags

int WINAPI WinMain(HINSTANCE hInst,
				   HINSTANCE hPrevInst,
				   LPSTR lspszCmdParam,
				   int nCmdShow) {
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
			connectPort();
			break;
		}
		if ((HWND)lParam == hDisconnectButton) {
			disconnectPort();
			break;
		}
		if (LOWORD(wParam) == IDM_HELP)
		{
			showHelp();
		}
		if (LOWORD(wParam) == IDM_EXIT)
		{
			if (readLoopOn)
			{
				disconnectPort();
			}
			PostQuitMessage(0);
		}
		break;
	case WM_SYSKEYDOWN:
	case WM_CHAR:
	case WM_KEYDOWN:
		if ((char)wParam == 27) // if ESC is pressed
		{
			disconnectPort();
		}
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
void showHelp()
{
	LPCSTR help = 
"Hardware Requirements:\n\
  - Windows 10 64- bit OS\n\
  - Skyetek RFID M7 or M9 device.\n\
\n\
Set Up : \n\
  1) Connect the skeytek device via USB a port.\n\
  2) Click Connect to start reading tags\n\
  3) Scan the RFID tags \n\
  4) Click Disconnect to disconnect \n\
  5) Exit Menu Item to Close Program\n\
  6) Help Menu Item to Open Help";

	MessageBox(NULL, help, TEXT("HELP"), MB_OK);
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

void switchButtonEnabled(HWND enabledButton,
									 HWND disabledButton)
{
	EnableWindow(enabledButton, false);
	EnableWindow(disabledButton, true);
}

void connectPort()
{
	connect();
	switchButtonEnabled(hConnectButton, hDisconnectButton);
}

void disconnectPort() {
	readLoopOn = false;
	clearDisplay(hwnd, &hwnd_yPos);
	clearDisplay(tagDisplay,&tagDisplay_yPos);
	clearDisplay(tagHistDisplay, nullptr);
	UpdateWindow(hwnd);
	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	switchButtonEnabled(hDisconnectButton, hConnectButton);
}

void displayTag(string tag)
{
	DWORD xPos = 1;
	TEXTMETRIC tm;
	HDC hdc = GetDC(tagDisplay);
	SetBkMode(hdc, TRANSPARENT);
	GetTextMetrics(hdc, &tm);
	DWORD yDiff = tm.tmHeight + tm.tmExternalLeading;
	TextOut(hdc, xPos, tagDisplay_yPos, tag.c_str(), tag.length()); // output character    
	ReleaseDC(tagDisplay, hdc);
	tagDisplay_yPos += (tm.tmHeight + tm.tmExternalLeading);
}

void displayReader(string tag)
{
	DWORD xPos = 3;
	TEXTMETRIC tm;
	HDC hdc = GetDC(hwnd);
	SetBkMode(hdc, TRANSPARENT);
	GetTextMetrics(hdc, &tm);
	DWORD yDiff = tm.tmHeight + tm.tmExternalLeading;
	TextOut(hdc, xPos, hwnd_yPos, tag.c_str(), tag.length()); // output character    
	ReleaseDC(tagDisplay, hdc);
	hwnd_yPos += (tm.tmHeight + tm.tmExternalLeading);
}

void addTagStrToHist(string tag) 
{
	if (tagHist.size() >= 15)
	{
		tagHist.pop_front();
	}
	tagHist.push_back(tag);
	
}

void repaintDisplayHist()
{
	InvalidateRect(tagHistDisplay, NULL, TRUE);
	DWORD yPos = 0, xPos = 0;
	TEXTMETRIC tm;
	HDC hdc = GetDC(tagHistDisplay);
	SetBkMode(hdc, TRANSPARENT);
	GetTextMetrics(hdc, &tm);
	DWORD yDiff = tm.tmHeight + tm.tmExternalLeading;
	for (DWORD i = 0; i < tagHist.size() ; ++i)
	{
		TextOut(hdc, xPos, yPos, tagHist.at(i).c_str(), tagHist.at(i).length());
		yPos += yDiff;
	}
	ReleaseDC(tagHistDisplay, hdc);
}

void clearDisplay(HWND wnd, DWORD * yPos)
{
	InvalidateRect(wnd, NULL, TRUE);
	UpdateWindow(wnd);
	if (yPos != nullptr)
	{
		*yPos = 0;
	}

}

void displayErrorMessageBox(LPCTSTR text)
{
	MessageBox(NULL, text, TEXT("ERROR"), MB_OK);
}

void connect()
{
	if (connectRFID()) {
		DWORD threadID;
		if (readers[0] != NULL)
		{
			readLoopOn = true;
			hThread = CreateThread(NULL, 0, readLoop, nullptr, 0, &threadID);
		}
	}

}

BOOLEAN connectRFID()
{
	numOfDevices = SkyeTek_DiscoverDevices(&devices);
	displayReader("Discovering device...");
	if (numOfDevices)
	{
		numOfReaders = SkyeTek_DiscoverReaders(devices, numOfDevices, &readers);
		if (numOfReaders == 0)
		{
			SkyeTek_FreeDevices(devices, numOfDevices);
			numOfDevices = 0;
			return false;
			displayReader("Cannot detect reader.");
		}
		displayReader("Device Ready!");
		return true; 
	}
	displayReader("Cannot detect device.");
	return false;
}



DWORD WINAPI readLoop(LPVOID)
{
	while (readLoopOn)
	{
		SKYETEK_STATUS status;
		LPSKYETEK_TAG * lptags = nullptr;
		unsigned short numOfTags = 0;

		status = SkyeTek_GetTags(readers[0], AUTO_DETECT, &lptags, &numOfTags);
		if (numOfTags == 0 || status == SKYETEK_FAILURE )
		{
			clearDisplay(tagDisplay, &tagDisplay_yPos);
		}
		else
		{
			status = SkyeTek_SelectTags(readers[0], AUTO_DETECT, tagRead, 0, 1, NULL);
		}
		status = SkyeTek_FreeTags(readers[0], lptags, numOfTags);
	}
	disconnect();
	return readLoopOn;
}
unsigned char tagRead(LPSKYETEK_TAG lptag, void* user) 
{
	stringstream ss;
	string tagType = "";
	string friendly = "";
	friendly = friendlyToString(lptag);
	tagType  = tagTypeToString(lptag);
	
	ss << "TAG :" <<  friendly  << " [" << tagType << "]";
	
	displayTag(ss.str());
	addTagStrToHist(ss.str());
	repaintDisplayHist();
	ReadTagData(lptag);
	return 0;
}

SKYETEK_STATUS ReadTagData(LPSKYETEK_TAG lpTag)
{
	unsigned short numOfTags = 0;
	string tag;
	stringstream ss;


	displayTag("Reading tag data ...");

	string dataStr = GetData(lpTag);
	if (!dataStr.empty())
	{
		ss << dataStr;
		tag = ss.str();
		displayTag(tag);
		return SKYETEK_SUCCESS;
	}
	ss << " read failure.";
	displayTag(ss.str());

	return SKYETEK_FAILURE;
}


string GetData(LPSKYETEK_TAG lptag)
{
	DWORD start = 1;
	SKYETEK_STATUS status;
	LPSKYETEK_STRING str = nullptr;
	LPSKYETEK_DATA lpdata = NULL;
	SKYETEK_ADDRESS addr;
	stringstream ss;

	addr.start  = start;
	addr.blocks = 1;
	while (addr.start < 10)
	{
		addr.start = ++start;
		status = SkyeTek_ReadTagData(readers[0], lptag, &addr, 0, 0, &lpdata);
		if (status == SKYETEK_SUCCESS)
		{
			str = SkyeTek_GetStringFromData(lpdata);
			
			if (str != nullptr)
			{
				string dstring = dataToString(lpdata);
				ss << " Block " << start << ": " << str;
			}
		}
	}
	SkyeTek_FreeData(lpdata);
	return ss.str();
}

string dataToString(LPSKYETEK_DATA lpdata)
{
	stringstream ss;
	for (int i = 1; i < (sizeof(SkyeTek_GetStringFromData(lpdata)) * 8); i += 2)
		{
			ss << SkyeTek_GetStringFromData(lpdata) + i;
		}
	
	return ss.str();
}

string friendlyToString(LPSKYETEK_TAG lpTag)
{
	const DWORD friendlyLen = 128;
	stringstream ss;
	
	for (int i = 1; i < friendlyLen; i+=2)
	{
		ss << lpTag->friendly + i;
	}
	return ss.str();
}

string tagTypeToString(LPSKYETEK_TAG lpTag)
{
	stringstream ss;
	for (int i = 0; i < (sizeof(SkyeTek_GetTagTypeNameFromType(lpTag->type)) * 16); i+=2)
	{
		ss << SkyeTek_GetTagTypeNameFromType(lpTag->type) + i;
	}
	return ss.str();
}

void disconnect()
{
	tagHist.clear();
	SkyeTek_FreeReaders(readers, numOfReaders);
	SkyeTek_FreeDevices(devices, numOfDevices);
}


