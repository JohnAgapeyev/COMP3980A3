#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <string>
#include <sstream>

using std::string;

class Controller {

private:
    void switchButtonEnabled (HWND enabledButton, 
                              HWND disabledButton);

public:
    Controller();

    LPCSTR configCommPortParams (WPARAM wParam); 
    void connectPort        ( /*pointer to the object that will connect to port*/ 
                              HWND connectButton, 
                              HWND disconnectButton );
    
    void disconnectPort     ( /*pointer to the object that is connected to port*/
                              HWND MainWin, 
                              HWND conenctButton, 
                              HWND disconnectButton );
    
    void displayTag         ( HWND hDisplay,
                              DWORD xPos,
                              string tag );
	void repaintDisplay    ( HWND display, 
							 string * tags[],
							 DWORD NumOfTags);

	void displayErrorMessageBox (LPCTSTR text);
};