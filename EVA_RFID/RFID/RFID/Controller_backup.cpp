#include "controller.h"
#include "RFID.h"

#include <string>
#include <iostream>
#include <algorithm>


static BOOLEAN readLoopOn;
static LPCSTR lpszCommName = "xHCI";
HANDLE hComm, hCommWrite;
HANDLE hThread;
LPCSTR dcbSettings = "baud=9600 parity=N data=8 stop=1";

LPSKYETEK_DEVICE myDevice;

LPSKYETEK_DEVICE * devices = nullptr;
LPSKYETEK_READER * readers = nullptr;
SKYETEK_STATUS st;

DWORD numOfDevices = 0, numOfReaders = 0;

void Controller::switchButtonEnabled(HWND enabledButton,
									 HWND disabledButton)
{
	EnableWindow(enabledButton, false);
	EnableWindow(disabledButton, true);
}

void Controller::connectPort( /*pointer to the object that will connect to port*/
							 HWND connectButton,
							 HWND disconnectButton)
{
	connect();
	switchButtonEnabled(connectButton, disconnectButton);
}

void Controller::disconnectPort( /*pointer to the object that will disconnect form port*/
								HWND connectButton,
								HWND disconnectButton) {
	disconnect();
	switchButtonEnabled(disconnectButton, connectButton);
}

LPCSTR Controller::configCommPortParams(WPARAM wParam) {
	//FIX THIS FUNCTION!!! 
	return NULL;
}

void Controller::displayTag(HWND hDisplay,
							DWORD yPos,
							string tag)
{
	DWORD xPos = 1;
	HDC hdc = GetDC(hDisplay);
	TextOut(hdc, xPos, yPos, tag.c_str(), tag.length()); // output character    
	ReleaseDC(hDisplay, hdc);
}

void Controller::repaintDisplay(HWND display,
								string tags[],
								DWORD NumOfTags)
{
	InvalidateRect(display, NULL, TRUE);
	DWORD yPos = 1;
	TEXTMETRIC tm;
	HDC hdc = GetDC(display);
	GetTextMetrics(hdc, &tm);
	DWORD yDiff = tm.tmHeight + tm.tmExternalLeading;
	ReleaseDC(display, hdc);

	for (DWORD i = 0; i < NumOfTags; ++i)
	{
		displayTag(display, yPos, tags[i]);
		yPos += yDiff;
	}
}


void Controller::displayErrorMessageBox(LPCTSTR text)
{
	MessageBox(NULL, text, TEXT("ERROR"), MB_OK);
}

BOOLEAN connectPort()
{
	
	numOfDevices = SkyeTek_DiscoverDevices(&devices);
	if (numOfDevices)
	{
		numOfReaders = SkyeTek_DiscoverReaders(devices, numOfDevices, &readers);
		if (numOfReaders == 0)
		{
			SkyeTek_FreeDevices(devices, numOfDevices);
			numOfDevices = 0;
			return false;
		}

		readers[0]->lpDevice = devices[0];
		SkyeTek_LoadDefaults(readers[0]);
		st = SkyeTek_OpenDevice(readers[0]->lpDevice);
		if (st != SKYETEK_SUCCESS)
		{
			return false;
		}
		return true;
	}
	return false;
	
}

BOOL configDCB() {
	DCB portDCB;
	GetCommState(hComm, &portDCB);
	portDCB.DCBlength = sizeof(DCB);
	DWORD bcdcb = BuildCommDCB(dcbSettings, &portDCB);
	if (bcdcb == 0) {
		MessageBox(NULL, "Unable to configure the build the DCB", TEXT("ERROR"), MB_OK);
		return false;
	}
	return true;
}
DWORD WINAPI readLoop(LPVOID p) {
	DWORD devtmask;
	DWORD dwStoredFlags;
	dwStoredFlags = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | \
		EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY;

	LPSKYETEK_TAG * lptags = nullptr;
	unsigned short numOfTags = 0;
    std::string selectMessage = selectTagMessageFactory();
    OVERLAPPED over = {0};
	std::string buf;

	over.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetCommMask(readers[0]->lpDevice->readFD, dwStoredFlags);
	
    for (;;) {
        buf = "";
		SkyeTek_GetTags(readers[0], AUTO_DETECT, &lptags, &numOfTags);
		//SkyeTek_SelectTags(readers[0], AUTO_DETECT, tagRead,  0, 1, NULL);
        if (WaitCommEvent(readers[0]->lpDevice->readFD, &devtmask, NULL/*&over*/)) {
            //2122 is the maximum packet size in ascii mode
            //ReadFile(hComm, &buf, 2122, nullptr, &over);
            size_t endPos;
            size_t packetCount = std::count(buf.begin(), buf.end(), '\n');
            for (size_t i = 0; i < packetCount; ++i) {
                if (endPos = buf.find('\n') != std::string::npos) {
					SkyeTek_GetTags(readers[0], AUTO_DETECT, &lptags, &numOfTags);
					//SkyeTek_SelectTags(readers[0], AUTO_DETECT, tagRead,  0, 1, NULL);
					std::cout << TEXT(lptags[0]->friendly);
					SkyeTek_FreeTags(readers[0], lptags, numOfTags);
					numOfTags = 0;
					lptags = nullptr;
					/*
					std::string response = buf.substr(0, endPos);
                    //Strip the control characters from the response
                    response = response.substr(1, response.length() - 3);
                    std::cout << response << std::endl;
                    buf.erase(0, endPos);
					*/
                }
            }
        }
		else
		{
			auto test = GetLastError();
			SkyeTek_FreeTags(readers[0], lptags, numOfTags);
		}
    }
	CloseHandle(over.hEvent);
}


void connect()
{
	if (connectPort()) {
		if (readers[0] != NULL)
		{
			LPSKYETEK_TAG * lptags = nullptr;
			unsigned short numOfTags = 0;
			DWORD threadID;

			hComm = readers[0]->lpDevice->readFD;
			hCommWrite = readers[0]->lpDevice->writeFD;
			
			
			SkyeTek_GetTags(readers[0], AUTO_DETECT, &lptags, &numOfTags);
			//SkyeTek_SelectTags(readers[0], AUTO_DETECT, tagRead,  0, 1, NULL);
			std::cout << TEXT(lptags[0]->friendly);
			SkyeTek_FreeTags(readers[0], lptags, numOfTags);
			
			readLoopOn = true;
			
			hThread = CreateThread(NULL, 0, readLoop, nullptr, 0, &threadID);
		}
	}

}

unsigned char tagRead(LPSKYETEK_TAG lptag, void* user)
{
	//Controller controller;
	string tag;
	//LPSKYETEK_MEMORY mem;
	//SkyeTek_GetTagInfo(readers[0],lptag, mem);
	//controller.displayTag();
	return 0;
}
void disconnect()
{
	readLoopOn = false;
	SkyeTek_FreeReaders(readers, numOfReaders);
}