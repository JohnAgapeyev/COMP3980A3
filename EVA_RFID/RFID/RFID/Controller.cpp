#include "controller.h"
#include "RFID.h"

#include <string>
#include <iostream>
#include <algorithm>

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
	//Object to Connect.connectPort();
	switchButtonEnabled(connectButton, disconnectButton);

}

void Controller::disconnectPort( /*pointer to the object that will disconnect form port*/
								HWND connectButton,
								HWND disconnectButton) {
	//Object to Connect.disconnectPort();
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

void readLoop(HANDLE hComm) {
    std::string selectMessage = selectTagMessageFactory();
    OVERLAPPED over = {0};
    over.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    SetCommMask(hComm, EV_RXCHAR);
    WriteFile(hComm, &selectMessage, selectMessage.length(), nullptr, &over);
    std::string buf;
    for (;;) {
        buf = "";
        if (WaitCommEvent(hComm, nullptr, &over)) {
            //2122 is the maximum packet size in ascii mode
            ReadFile(hComm, &buf, 2122, nullptr, &over);
            size_t endPos;
            size_t packetCount = std::count(buf.begin(), buf.end(), '\n');
            for (size_t i = 0; i < packetCount; ++i) {
                if (endPos = buf.find('\n') != std::string::npos) {
                    std::string response = buf.substr(0, endPos);
                    //Strip the control characters from the response
                    response = response.substr(1, response.length() - 3);
                    std::cout << response << std::endl;
                    buf.erase(0, endPos);
                }
            }
        }
    }
}