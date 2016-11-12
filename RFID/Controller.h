#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <string>
#include <sstream>

using std::string;

class Controller {

private:
	/*swtiches around the buttons from enabled to disabled*/
    void switchButtonEnabled (HWND enabledButton, 
                              HWND disabledButton);

public:
    //Controller();

    LPCSTR configCommPortParams (WPARAM wParam); 

	/*connects to the port and changes the buttons*/
    void connectPort        ( /*pointer to the object that will connect to port*/ 
                              HWND connectButton, 
                              HWND disconnectButton );
    
	/*disconnects the port and changes the buttons*/
    void disconnectPort     ( /*pointer to the object that is connected to port*/
                              HWND connectButton, 
                              HWND disconnectButton );
    
	/*displays a single tag*/
    void displayTag         ( HWND hDisplay,
                              DWORD yPos,
                              string tag ); 

	/*displays a set of tags*/
	void repaintDisplay    ( HWND display, 
							 string tags[],
							 DWORD NumOfTags);
	
	/** displays an error message **/
	void displayErrorMessageBox (LPCTSTR text);
};