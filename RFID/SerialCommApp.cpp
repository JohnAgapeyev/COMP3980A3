#include "SerialCommApp.h"
#include "DummyTerm_Definitions.h"

/*--------------------------------------------------------------------------
-- FUNCTION: SerialCommApp Constructor
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void SerialCommApp (void)
--
--
-- NOTES:
-- constructor for Serical Communication Application Class  
--------------------------------------------------------------------------*/

SerialCommApp::SerialCommApp()
{
    for (int i = 0; i < 2; ++i)
    {
        char_pos_userInputTextBox[i] = 0;
    }
}

/*--------------------------------------------------------------------------
-- FUNCTION: SerialCommApp Destructor
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void ~SerialCommApp (void)
--
--
-- NOTES:
-- desstructor for Serical Communication Application Class  
--------------------------------------------------------------------------*/
SerialCommApp::~SerialCommApp()
{
}

/*--------------------------------------------------------------------------
-- FUNCTION: setConnect Constructor
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void setConnect (HWND)
--
--
-- NOTES:
-- sets up the user to be connected to the port for the session
-- this is the representative of the ending of the "session" layer 
-- of the OSI approach
--------------------------------------------------------------------------*/

void SerialCommApp::setConnect(HWND hwnd)
{
    //serialCommPort.init(hwnd, readInputTextBox);
    serialCommPort.connectPort();
    if (serialCommPort.isConnected())
    {
        EnableWindow(hConnectButton, false);
        EnableWindow(hDisconnectButton, true);
    }
}

/*--------------------------------------------------------------------------
-- FUNCTION: getConnectButton
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: HWND getConnectButton (void)
--
-- RETURNS: 
-- handle to the window instance of the button that controls 
-- the connection to the Port
--
-- NOTES:
-- just a getter call for resusability / encapsulation
--------------------------------------------------------------------------*/

void SerialCommApp::setDisconnect()
{
    serialCommPort.disconnectPort();
    if (!serialCommPort.isConnected())
    {
        EnableWindow(hConnectButton, true);
        EnableWindow(hDisconnectButton, false);
        UpdateWindow(hConnectButton);
        UpdateWindow(hDisconnectButton);
    }
}

/*--------------------------------------------------------------------------
-- FUNCTION: getSerialCommPort
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: [returntype] getSerialCommPort (void)
--
-- RETURNS: 
-- Pointer to the SerialCommApp object / instance
--
-- NOTES:
-- just a getter call for resusability / encapsulation
--------------------------------------------------------------------------*/

HWND SerialCommApp::getConnectButton()
{
    return hConnectButton;

}

HWND SerialCommApp::getDisconnectButton()
{
    return hDisconnectButton;
}
SerialCommPort * SerialCommApp::getSerialCommPort()
{
    return & this->serialCommPort;
}
void SerialCommApp::init(HWND hwnd)
{
    if (this->createUIWindows(hwnd))
    {
        this->serialCommPort.init(hwnd, this->readInputTextBox);
    }
    else
    {
        displayErrorMessageBox(TEXT("error initation com port"));
    }
}

/*--------------------------------------------------------------------------
-- FUNCTION: processKeyboardInput
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void processKeyboardInput (HWND hwnd, UINT Message, WPARAM wParam)
-- HWND handle to the parent window
-- UNIT holds informations to the message
-- WPARAM holds information to the key
--
-- NOTES:
-- simple function that delegates the keyboard input. 
-- further / future versions may expand in this function 
--------------------------------------------------------------------------*/

void SerialCommApp::processKeyboardInput(HWND hwnd, UINT Message, WPARAM wParam)
{
    switch (Message)
    {
    case WM_CHAR:
        processChar(hwnd, wParam);
        break;

    }
}

LPCSTR SerialCommApp::processComConfigSettings(HWND hwnd, WPARAM wParam){
    if (serialCommPort.isConnected()) {
        displayErrorMessageBox(TEXT("You Must disconnect first"));
        return NULL;
    }
    string dcbSettingsStr = "";
    LPCSTR dcbSettings;
    switch (LOWORD(wParam))
    {
    case IDM_BAUD_RATE_110:
        dcbSettingsStr += "baud=110 ";
        serialCommPort.setBaudRate(110);
        break;
    case IDM_BAUD_RATE_300:
        dcbSettingsStr += "baud=300 ";
        serialCommPort.setBaudRate(300);
        break;
    case IDM_BAUD_RATE_600:
        dcbSettingsStr += "baud=600 ";
        serialCommPort.setBaudRate(600);
        break;
    case IDM_BAUD_RATE_1200:
        dcbSettingsStr += "baud=1200";
        serialCommPort.setBaudRate(1200);
        break;
    case IDM_BAUD_RATE_4800:
        dcbSettingsStr += "baud=4800 ";
        serialCommPort.setBaudRate(4800);
        break;
    case IDM_BAUD_RATE_9600:
        dcbSettingsStr += "baud=9600 ";
        serialCommPort.setBaudRate(9600);
        break;
    case IDM_PARITY_NO:
        dcbSettingsStr += "parity=N ";
        serialCommPort.setParity(0);
        break;
    case IDM_PARITY_MARK:
        dcbSettingsStr += "parity=M ";
        serialCommPort.setParity(3);
        break;
    case IDM_PARITY_SPACE:
        dcbSettingsStr += "parity=S ";
        serialCommPort.setParity(4);
        break;
    case IDM_PARITY_EVEN:
        dcbSettingsStr += "parity=E ";
        serialCommPort.setParity(2);
        break;
    case IDM_PARITY_ODD:
        dcbSettingsStr += "parity=O ";
        serialCommPort.setParity(1);
        break;
    case IDM_DATA_BITS_8:
        dcbSettingsStr += "data=8 ";
        serialCommPort.setDataBits(8);
        break;
    case IDM_DATA_BITS_6:
        dcbSettingsStr += "data=6 ";
        serialCommPort.setDataBits(6);
        break;
    case IDM_DATA_BITS_7:
        dcbSettingsStr += "data=7 ";
        serialCommPort.setDataBits(7);
        break;
    case IDM_STOP_BIT_1:
        dcbSettingsStr += "stop=1 ";
        serialCommPort.setStopBit(0);
        break;
    case IDM_STOP_BIT_2:
        dcbSettingsStr += "data=2 ";
        serialCommPort.setStopBit(2);
    default:
        displayErrorMessageBox("Settings cannot be set");
        break;
    }
    serialCommPort.setDCBConfig(dcbSettingsStr.c_str());
    return dcbSettingsStr.c_str();
}
/*--------------------------------------------------------------------------
-- FUNCTION: createUIWindows
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: BOOL createUIWindows (HWND)
-- HWND handle to the parent window
--
-- RETURNS: 
-- boolean indicating success of window creation
--
-- NOTES:
-- function creates all the sub windows of the parent window
-- this is where the UI is generated
--------------------------------------------------------------------------*/

BOOL SerialCommApp::createUIWindows(HWND hwnd)
{
    userInputTextBox = CreateWindow("STATIC",
                                    NULL,
                                    WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED,
                                    USERINPUT_TEXTBOX_START_X,
                                    USERINPUT_TEXTBOX_START_Y,
                                    TEXTBOX_WIDTH,
                                    TEXTBOX_HEIGTH,
                                    hwnd,
                                    NULL,
                                    NULL,
                                    NULL
                                    );

    readInputTextBox = CreateWindow("STATIC",
                                    NULL,
                                    WS_BORDER | WS_CHILD | WS_VISIBLE | WS_DISABLED,
                                    READINPUT_TEXTBOX_START_X,
                                    READINPUT_TEXTBOX_START_Y,
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
                                  50,
                                  200,
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
                                     50,
                                     300,
                                     BUTTON_WIDTH,
                                     BUTTON_HEIGHT,
                                     hwnd,
                                     NULL,
                                     NULL,
                                     NULL
                                     );
    return true;
}

/*--------------------------------------------------------------------------
-- FUNCTION: displayChar
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: [returntype]  displayChar 
--
-- NOTES:
-- displays characters
--------------------------------------------------------------------------*/
void SerialCommApp::displayChar(HWND hwnd, WPARAM wParam)
{
    this->hdc = GetDC(userInputTextBox);
    GetTextMetrics(this->hdc, &this->tm);

    TextOut(hdc, char_pos_userInputTextBox[0], char_pos_userInputTextBox[1], inputStr, strlen(inputStr)); // output character    

    char_pos_userInputTextBox[0] += tm.tmAveCharWidth; // increment the screen x-coordinate
    if (char_pos_userInputTextBox[0] + tm.tmAveCharWidth >= TEXTBOX_WIDTH) {
        char_pos_userInputTextBox[0] = 0;
        char_pos_userInputTextBox[1] = char_pos_userInputTextBox[1] + tm.tmHeight + tm.tmExternalLeading;
    }

    ReleaseDC(userInputTextBox, hdc); // Release device context
}

void SerialCommApp::processChar(HWND hwnd, WPARAM wParam)
{
    if ((char)wParam == 27) // if ESC is pressed
    {
        serialCommPort.disconnectPort();
        return;
    }
    sprintf(inputStr, "%c", (char)wParam); // Convert char to string

    writeInputCharToComPort(hwnd, wParam);
    displayChar(hwnd, wParam);

    sprintf(inputStr, "%c", '\0');
}
/*--------------------------------------------------------------------------
-- FUNCTION: writeInputCharToComPort
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void writeInputCharToComPort (HWND, WPARAM)
-- hwnd handle from the parent window
-- wParam contains information on the character to be processed
--
-- NOTES:
-- this is one of the connection functions that link the physical layer
-- with the application layer 
--------------------------------------------------------------------------*/

void SerialCommApp::writeInputCharToComPort(HWND hwnd, WPARAM wParam)
{
    this->serialCommPort.write(inputStr);
}
/*--------------------------------------------------------------------------
-- FUNCTION: displayErrorMessageBox
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void displayErrorMessageBox (LPCTSTR text)
-- LPCSTR const text string representing an erroe message
--
-- NOTES:
-- all message boxes using this function has the title "ERROR"
-- all message boxes using this function has an OK button
-- i just made it so it is easier to write cleaner code 
--------------------------------------------------------------------------*/

void SerialCommApp::displayErrorMessageBox(LPCTSTR text)
{
    MessageBox(NULL, text, TEXT("ERROR"), MB_OK);
}

void SerialCommApp::repaintWindow(HWND hwnd) 
{
    PAINTSTRUCT ps;
    hdc = BeginPaint(hwnd, &ps);
    TextOut(hdc, 0, 0, "", 0);
    EndPaint(hwnd, &ps);
}