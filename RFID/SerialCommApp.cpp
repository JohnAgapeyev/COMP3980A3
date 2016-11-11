#include "SerialCommApp.h"
#include "DummyTerm_Definitions.h"

SerialCommApp::SerialCommApp() {
    for (int i = 0; i < 2; ++i) {
        char_pos_userInputTextBox[i] = 0;
    }
}

void SerialCommApp::setConnect(HWND hwnd) {
    //serialCommPort.init(hwnd, readInputTextBox);
    serialCommPort.connectPort();
    if (serialCommPort.isConnected()) {
        EnableWindow(hConnectButton, false);
        EnableWindow(hDisconnectButton, true);
    }
}

void SerialCommApp::setDisconnect() {
    serialCommPort.disconnectPort();
    if (!serialCommPort.isConnected()) {
        EnableWindow(hConnectButton, true);
        EnableWindow(hDisconnectButton, false);
        UpdateWindow(hConnectButton);
        UpdateWindow(hDisconnectButton);
    }
}

HWND SerialCommApp::getConnectButton() {
    return hConnectButton;
}

HWND SerialCommApp::getDisconnectButton() {
    return hDisconnectButton;
}
SerialCommPort* SerialCommApp::getSerialCommPort() {
    return &serialCommPort;
}
void SerialCommApp::init(HWND hwnd) {
    if (createUIWindows(hwnd)) {
        serialCommPort.init(hwnd, readInputTextBox);
    } else {
        displayErrorMessageBox(TEXT("error initation com port"));
    }
}

void SerialCommApp::processKeyboardInput(HWND hwnd, UINT Message, WPARAM wParam) {
    switch (Message) {
        case WM_CHAR:
            processChar(hwnd, wParam);
            break;
    }
}

LPCSTR SerialCommApp::processComConfigSettings(HWND hwnd, WPARAM wParam) {
    if (serialCommPort.isConnected()) {
        displayErrorMessageBox(TEXT("You Must disconnect first"));
        return NULL;
    }
    string dcbSettingsStr = "";
    LPCSTR dcbSettings;
    switch (LOWORD(wParam)) {
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

BOOL SerialCommApp::createUIWindows(HWND hwnd) {
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

void SerialCommApp::displayChar(HWND hwnd, WPARAM wParam) {
    hdc = GetDC(userInputTextBox);
    GetTextMetrics(hdc, &tm);

    TextOut(hdc, char_pos_userInputTextBox[0], char_pos_userInputTextBox[1], inputStr, strlen(inputStr)); // output character    

    char_pos_userInputTextBox[0] += tm.tmAveCharWidth; // increment the screen x-coordinate
    if (char_pos_userInputTextBox[0] + tm.tmAveCharWidth >= TEXTBOX_WIDTH) {
        char_pos_userInputTextBox[0] = 0;
        char_pos_userInputTextBox[1] = char_pos_userInputTextBox[1] + tm.tmHeight + tm.tmExternalLeading;
    }

    ReleaseDC(userInputTextBox, hdc); // Release device context
}

void SerialCommApp::processChar(HWND hwnd, WPARAM wParam) {
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

void SerialCommApp::writeInputCharToComPort(HWND hwnd, WPARAM wParam) {
    serialCommPort.write(inputStr);
}

void SerialCommApp::displayErrorMessageBox(LPCTSTR text) {
    MessageBox(NULL, text, TEXT("ERROR"), MB_OK);
}

void SerialCommApp::repaintWindow(HWND hwnd) {
    PAINTSTRUCT ps;
    hdc = BeginPaint(hwnd, &ps);
    TextOut(hdc, 0, 0, "", 0);
    EndPaint(hwnd, &ps);
}