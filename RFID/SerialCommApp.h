#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <string>
#include <sstream>
#include "SerialCommPort.h"


using std::string;

class SerialCommApp
{
public:
    SerialCommApp();
    ~SerialCommApp();

    void setConnect(HWND);
    void setDisconnect();

    HWND getConnectButton();
    HWND getDisconnectButton();
    SerialCommPort * getSerialCommPort();
    void repaintWindow(HWND hwnd);

    void init(HWND);
    void processKeyboardInput(HWND, UINT, WPARAM);
    LPCSTR processComConfigSettings(HWND hwnd, WPARAM wParam);

private:
    BOOL createUIWindows(HWND hwnd);
    void displayChar(HWND hwnd, WPARAM wParam);

    void processChar(HWND hwnd, WPARAM wParam);
    void writeInputCharToComPort(HWND hwnd, WPARAM wParam);
    void displayErrorMessageBox(LPCTSTR text);

    //DATA MEMBERS
    static const DWORD BUFF_SIZE = 256;
    static const DWORD TEXTBOX_HEIGTH = 200;
    static const DWORD TEXTBOX_WIDTH = 400;
    static const DWORD USERINPUT_TEXTBOX_START_X = 190;
    static const DWORD USERINPUT_TEXTBOX_START_Y = 10;
    static const DWORD READINPUT_TEXTBOX_START_X = 190;
    static const DWORD READINPUT_TEXTBOX_START_Y = 220;
    static const DWORD BUTTON_WIDTH = 100;
    static const DWORD BUTTON_HEIGHT = 30;
    static const DWORD DISCONNECT_BUTTON_X = 50;
    static const DWORD DISCONNECT_BUTTON_Y = 150;
    static const DWORD CONNECT_BUTTON_X = 50;
    static const DWORD CONNECT_BUTTON_Y = 250;

    SerialCommPort serialCommPort;
    HDC hdc;
    TEXTMETRIC tm;
    HWND userInputTextBox;
    HWND readInputTextBox;
    HWND hConnectButton;
    HWND hDisconnectButton;
    HWND hComConfigsButton;

    int char_pos_userInputTextBox[2];
    char input[BUFF_SIZE];
    char inputStr[BUFF_SIZE];

};