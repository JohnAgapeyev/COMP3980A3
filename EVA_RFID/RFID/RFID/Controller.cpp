#include "controller.h"

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