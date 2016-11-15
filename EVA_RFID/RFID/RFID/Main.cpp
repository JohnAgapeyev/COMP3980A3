/*------------------------------------------------------------------------------
-- SOURCE FILE: main.cpp - header file for RFID COMP 3980 Assignment
--
-- PROGRAM: RFID.exe
--
-- FUNCTIONS:
-- *list all functions here!* 
----------------------------------------------------
~~~~~~COMMAND FUNCTIONS~~~~~~
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool 			 setConfigParams(HWND, UINT, WPARAM, LPARAM);
void		 	 createGUI(HWND);

~~~~~~DISPLAY FUNCTIONS~~~~~~
void		 	 switchButtonEnabled(HWND,HWND);
void		 	 displayReader(std::string);	
void		 	 displayTag(std::string);       
void		 	 addTagStrToHist(std::string);  
void		 	 repaintDisplayHist();			
void		 	 clearDisplay(HWND, DWORD *);	 
void		 	 displayErrorMessageBox(LPCTSTR); 
void		 	 showHelp();					
void		 	 connectPort(); 
void		 	 disconnectPort(); 

~~~~~~CONNECTED FUNCTIONS~~~~~~
BOOLEAN 		 connect();						
BOOLEAN 		 connectRFID();				
DWORD WINAPI	 readLoop(LPVOID);		
unsigned char	 tagRead(LPSKYETEK_TAG, void* ); 
SKYETEK_STATUS   ReadTagData(LPSKYETEK_TAG);	 
void 			 disconnect();				   

~~~~~~DISPLAY HELP FUNCTIONS~~~~~~
std::string  GetData(LPSKYETEK_TAG); 
std::string  tagTypeToString(LPSKYETEK_TAG);
std::string  friendlyToString(LPSKYETEK_TAG);
std::string  dataToString(LPSKYETEK_DATA);
----------------------------------------------------
--
-- DATE: NOV. 16, 2016
--
-- REVISIONS: 
-- Version 1.0 -- attempting to implement without Skyetek API
-- Version 2.0 -- revised to use with Skyetek API, Event driven, w/ classes 
-- Version 3.0 -- revised to use with Skyetek API, Async, removed classes 
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- NOTES:
-- see source code for detailed function descriptions
------------------------------------------------------------------------------*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "main.h"

using namespace std;
deque<string>  tagHist; //holds a history of tags

/*--------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int)
-- refer to WIN32 API documentation for details
--
-- NOTES:
-- Retrieved and changed from Assignment 1
--------------------------------------------------------------------------*/

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

/*--------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.5
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: LRESULT CALLBACK WndProc(HWND, UNIT, WPARAM, LPARAM)
-- refer to WIN32 API documentation for details
--
-- NOTES:
-- Retrieved and adopted from Assignment 1
--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------
-- FUNCTION: showHelp
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: 
-- Set Version 1.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void showHelp (void)
--
-- NOTES:
-- shows a pop up help page
--------------------------------------------------------------------------*/
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
  4) Click Disconnect or the ESC key to disconnect \n\
  5) Exit Menu Item to Close Program\n\
  6) Help Menu Item to Open Help";

	MessageBox(NULL, help, TEXT("HELP"), MB_OK);
}

/*--------------------------------------------------------------------------
-- FUNCTION: createGui
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.5
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void [fucntionName] (HWND)
-- HWND HandleTotheParentWindow
--
-- NOTES:
-- Adopted and Adjusted from A1
--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------
-- FUNCTION: switchButtonEnabled
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void switchButtonEnabled (HWND, HWND)
-- HWND handle to the Button that is currently enabled
-- HWND handle to the Button that is currently disabled
--
-- NOTES:
-- swaps two buttons so that the former 
-- becomes diasabled and latter is enabled
--------------------------------------------------------------------------*/
void switchButtonEnabled(HWND enabledButton,
						 HWND disabledButton)
{
	EnableWindow(enabledButton, false);
	EnableWindow(disabledButton, true);
}

/*--------------------------------------------------------------------------
-- FUNCTION: connectPort
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void connectPort (void)
-- 
-- NOTES:
-- the initial actor when "connect" button is clicked  
--------------------------------------------------------------------------*/
void connectPort()
{
	clearDisplay(hwnd, &hwnd_yPos);
	if ( connect() )
	{
		switchButtonEnabled(hConnectButton, hDisconnectButton);
	}
}

/*--------------------------------------------------------------------------
-- FUNCTION: disconnectPort
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void diconnectPort (void)
--
-- NOTES:
-- the initial actor when the disconnect button is clicked
-- function handles clearing displays and setting the flag to end
-- the thread loop. 
-- function will wait up to 3s for read thread to 
-- complete before closing the handle
--------------------------------------------------------------------------*/
void disconnectPort() {
	readLoopOn = false;
	displayReader("Disconecting ... ")

	clearDisplay(hwnd, &hwnd_yPos);
	clearDisplay(tagDisplay,&tagDisplay_yPos);
	clearDisplay(tagHistDisplay, nullptr);
	UpdateWindow(hwnd);

	WaitForSingleObject(hThread, 3000);
	CloseHandle(hThread);
	switchButtonEnabled(hDisconnectButton, hConnectButton);
}

/*--------------------------------------------------------------------------
-- FUNCTION: displayTag
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.5
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void displayTag (string)
-- string 		the string to display into the "tags discovered" window
--
-- NOTES:
-- displays a tag or string into the tags 
-- discovered area with proper formatting
--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------
-- FUNCTION: displayReader
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void displayReader (string)
-- string	takes the string to insert into main window
--
-- NOTES:
-- Displays readers status
--------------------------------------------------------------------------*/	
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

/*--------------------------------------------------------------------------
-- FUNCTION: addTagStrToHist
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 1.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void addTagStrToHist(string tag) 
-- string	string that represents a  tag in the format tag_id[tag_type]
--
-- NOTES:
-- adds a tag to history of tags queue to be printed in history display
-- stores up to 15 tags in "history" queue
--------------------------------------------------------------------------*/
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
	DWORD yPos = 0, xPos = 0;
	TEXTMETRIC tm;

	InvalidateRect(tagHistDisplay, NULL, TRUE);
	UpdateWindow(tagHistDisplay);

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

BOOLEAN connect()
{
	if (connectRFID()) {
		DWORD threadID;
		if (readers[0] != NULL)
		{
			readLoopOn = true;
			hThread = CreateThread(NULL, 0, readLoop, nullptr, 0, &threadID);
			return true;
		}
		return false;
	}
	else
	{
		return false;
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
			displayReader("Cannot detect reader.");
			return false;
		}
		displayReader("Reader Ready!");
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
			clearDisplay(tagDisplay, &tagDisplay_yPos);
			string str = "Total Tags Found: " + numOfTags;
			displayTag(str);
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
	displayTag("Reading Tag...");
	if (lptag->type != NULL)
	{
		friendly = friendlyToString(lptag);
		tagType = tagTypeToString(lptag);
		ss << "Tag :" << friendly << " [" << tagType << "]";
	}
	else
	{
		displayTag("Error encountered reading tag");
	}
	
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
