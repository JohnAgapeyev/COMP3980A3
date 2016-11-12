#define STRICT

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "ass1.h"

char Name[] = TEXT("COMP3980 Assignment 1");
char str[80] = "";
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#pragma warning (disable: 4096)

LPCSTR lpszCommName = TEXT("com0");
COMMCONFIG cc;
HANDLE hComm;
DWORD BaudRate = -1;
BYTE ByteSize = 0xFF;
BYTE StopBits = 0xFF;
BYTE Parity = 0xFF;
bool writeKeys = false;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
    LPSTR lspszCmdParam, int nCmdShow) {
    HWND hwnd;
    MSG Msg;
    WNDCLASSEX Wcl;

    Wcl.cbSize = sizeof(WNDCLASSEX);
    Wcl.style = CS_HREDRAW | CS_VREDRAW;
    Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
    Wcl.hIconSm = NULL; // use small version of large icon
    Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

    Wcl.lpfnWndProc = WndProc;
    Wcl.hInstance = hInst;
    Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
    Wcl.lpszClassName = Name;

    Wcl.lpszMenuName = TEXT("MYMENU"); // The menu Class
    Wcl.cbClsExtra = 0;      // no extra memory needed
    Wcl.cbWndExtra = 0;

    if (!RegisterClassEx(&Wcl)) {
        return 0;
    }

    hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
        600, 400, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HMENU hmenu = GetSubMenu(GetMenu(hwnd), 0);
    CheckMenuItem(hmenu, 0, MF_BYPOSITION | MF_CHECKED);

    while (GetMessage(&Msg, NULL, 0, 0)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
    WPARAM wParam, LPARAM lParam) {
    int response;
    HDC hdc;
    HMENU hmodemenu = GetSubMenu(GetMenu(hwnd), 0);
    PAINTSTRUCT paintstruct;
    OVERLAPPED over = {0};
    static unsigned k = 0;

    switch (Message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_COM1:
                case IDM_COM2:
                case IDM_COM3:
                case IDM_COM4:
                    changeCOMPort(hwnd, wParam);
                    break;
                case IDM_110:
                case IDM_300:
                case IDM_600:
                case IDM_1200:
                case IDM_2400:
                case IDM_4800:
                case IDM_9600:
                case IDM_14400:
                case IDM_19200:
                case IDM_38400:
                case IDM_57600:
                case IDM_115200:
                case IDM_128000:
                case IDM_256000:
                    setBaudRate(hwnd, wParam);
                    break;
                case IDM_DATA4:
                case IDM_DATA5:
                case IDM_DATA6:
                case IDM_DATA7:
                case IDM_DATA8:
                    setByteSize(hwnd, wParam);
                    break;
                case IDM_STOP0:
                case IDM_STOP1:
                case IDM_STOP2:
                    setStopBits(hwnd, wParam);
                    break;
                case IDM_PARITY0:
                case IDM_PARITY1:
                case IDM_PARITY2:
                case IDM_PARITY3:
                case IDM_PARITY4:
                    setParityMode(hwnd, wParam);
                    break;
                case IDM_COMMAND:
                    CheckMenuItem(hmodemenu, IDM_COMMAND, MF_CHECKED);
                    CheckMenuItem(hmodemenu, IDM_CONNECT, MF_UNCHECKED);
                    stopWriting();
                    break;
                case IDM_CONNECT:
                    CheckMenuItem(hmodemenu, IDM_COMMAND, MF_UNCHECKED);
                    CheckMenuItem(hmodemenu, IDM_CONNECT, MF_CHECKED);
                    if (!saveSettings(hwnd)) {
                        break;
                    }
                    startWriting();
                    startReading(hwnd);
                    break;
                case IDM_Save:
                    saveSettings(hwnd);
                    break;
                case IDM_HELP:
                    showHelp();
                    break;
                case IDM_EXIT:
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_CHAR:    // Process keystroke
            if (wParam == VK_ESCAPE) {
                CheckMenuItem(hmodemenu, IDM_COMMAND, MF_CHECKED);
                CheckMenuItem(hmodemenu, IDM_CONNECT, MF_UNCHECKED);
                stopWriting();
            }
            if (writeKeys) {
                HANDLE overlapResult = CreateEvent(NULL, TRUE, FALSE, NULL);
                if (!overlapResult) {
                    break;
                }
                over.hEvent = overlapResult;
                sprintf_s(str, "%c", (char)wParam);
                WriteFile(hComm, str, strlen(str), NULL, &over);
            }
            break;

        case WM_PAINT:        // Process a repaint message
            hdc = BeginPaint(hwnd, &paintstruct); // Acquire DC
            TextOut(hdc, 0, 0, str, strlen(str)); // output character
            EndPaint(hwnd, &paintstruct); // Release DC
            break;

        case WM_DESTROY:    // Terminate program
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

bool saveSettings(HWND hwnd) {
    if (!hComm && TEXT(lpszCommName) == "com0") {
        MessageBox(NULL, "COM Port has not been selected", "", MB_OK);
        return false;
    }

    if (hComm && hComm != INVALID_HANDLE_VALUE && !CloseHandle(hComm)) {
        MessageBox(NULL, "Error closing COM port", "", MB_OK);
        return false;
    }

    if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
        NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
        == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, "Error opening COM port", "", MB_OK);
        return false;
    }

    cc.dwSize = sizeof(COMMCONFIG);
    cc.wVersion = 0x100;
    if (!GetCommConfig(hComm, &cc, &cc.dwSize)) {
        return false;
    }

    if (BaudRate == -1) {
        MessageBox(NULL, "Baud Rate has not been set", "", MB_OK);
        return false;
    }
    cc.dcb.BaudRate = BaudRate;

    if (ByteSize == 0xFF) {
        MessageBox(NULL, "Data size has not been set", "", MB_OK);
        return false;
    }
    cc.dcb.ByteSize = ByteSize;

    if (Parity == 0xFF) {
        MessageBox(NULL, "Parity has not been set", "", MB_OK);
        return false;
    }
    cc.dcb.Parity = Parity;

    if (StopBits == 0xFF) {
        MessageBox(NULL, "Number of stop bits has not been set", "", MB_OK);
        return false;
    }
    cc.dcb.StopBits = StopBits;

    SetCommState(hComm, &cc.dcb);
    return true;
}

void setBaudRate(HWND hwnd, WPARAM chosenRate) {
    HMENU hmenu = GetSubMenu(GetSubMenu(GetMenu(hwnd), 1), 1);
    int len = GetMenuItemCount(hmenu);

    for (int i = 0; i < len; ++i) {
        CheckMenuItem(hmenu, i, MF_BYPOSITION | MF_UNCHECKED);
    }

    switch (LOWORD(chosenRate)) {
        case IDM_110:
            BaudRate = CBR_110;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_300:
            BaudRate = CBR_300;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_600:
            BaudRate = CBR_600;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_1200:
            BaudRate = CBR_1200;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_2400:
            BaudRate = CBR_2400;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_4800:
            BaudRate = CBR_4800;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_9600:
            BaudRate = CBR_9600;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_14400:
            BaudRate = CBR_14400;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_19200:
            BaudRate = CBR_19200;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_38400:
            BaudRate = CBR_38400;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_57600:
            BaudRate = CBR_57600;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_115200:
            BaudRate = CBR_115200;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_128000:
            BaudRate = CBR_128000;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
        case IDM_256000:
            BaudRate = CBR_256000;
            CheckMenuItem(hmenu, LOWORD(chosenRate), MF_CHECKED);
            break;
    }
}

void changeCOMPort(HWND hwnd, WPARAM comSetting) {
    HMENU hmenu = GetSubMenu(GetSubMenu(GetMenu(hwnd), 1), 0);
    int len = GetMenuItemCount(hmenu);

    for (int i = 0; i < len; ++i) {
        CheckMenuItem(hmenu, i, MF_BYPOSITION | MF_UNCHECKED);
    }

    switch (LOWORD(comSetting)) {
        case IDM_COM1:
            lpszCommName = TEXT("com1");
            CheckMenuItem(hmenu, LOWORD(comSetting), MF_CHECKED);
            break;
        case IDM_COM2:
            lpszCommName = TEXT("com2");
            CheckMenuItem(hmenu, LOWORD(comSetting), MF_CHECKED);
            break;
        case IDM_COM3:
            lpszCommName = TEXT("com3");
            CheckMenuItem(hmenu, LOWORD(comSetting), MF_CHECKED);
            break;
        case IDM_COM4:
            lpszCommName = TEXT("com4");
            CheckMenuItem(hmenu, LOWORD(comSetting), MF_CHECKED);
            break;
    }
}

void setByteSize(HWND hwnd, WPARAM chosenSize) {
    HMENU hmenu = GetSubMenu(GetSubMenu(GetMenu(hwnd), 1), 2);
    int len = GetMenuItemCount(hmenu);

    for (int i = 0; i < len; ++i) {
        CheckMenuItem(hmenu, i, MF_BYPOSITION | MF_UNCHECKED);
    }

    switch (LOWORD(chosenSize)) {
        case IDM_DATA4:
            ByteSize = 4;
            CheckMenuItem(hmenu, LOWORD(chosenSize), MF_CHECKED);
            break;
        case IDM_DATA5:
            ByteSize = 5;
            CheckMenuItem(hmenu, LOWORD(chosenSize), MF_CHECKED);
            break;
        case IDM_DATA6:
            ByteSize = 6;
            CheckMenuItem(hmenu, LOWORD(chosenSize), MF_CHECKED);
            break;
        case IDM_DATA7:
            ByteSize = 7;
            CheckMenuItem(hmenu, LOWORD(chosenSize), MF_CHECKED);
            break;
        case IDM_DATA8:
            ByteSize = 8;
            CheckMenuItem(hmenu, LOWORD(chosenSize), MF_CHECKED);
            break;
    }
}

void setStopBits(HWND hwnd, WPARAM stopChoice) {
    HMENU hmenu = GetSubMenu(GetSubMenu(GetMenu(hwnd), 1), 3);
    int len = GetMenuItemCount(hmenu);

    for (int i = 0; i < len; ++i) {
        CheckMenuItem(hmenu, i, MF_BYPOSITION | MF_UNCHECKED);
    }

    switch (LOWORD(stopChoice)) {
        case IDM_STOP0:
            StopBits = ONESTOPBIT;
            CheckMenuItem(hmenu, LOWORD(stopChoice), MF_CHECKED);
            break;
        case IDM_STOP1:
            StopBits = ONE5STOPBITS;
            CheckMenuItem(hmenu, LOWORD(stopChoice), MF_CHECKED);
            break;
        case IDM_STOP2:
            StopBits = TWOSTOPBITS;
            CheckMenuItem(hmenu, LOWORD(stopChoice), MF_CHECKED);
            break;
    }
}

void setParityMode(HWND hwnd, WPARAM parityChoice) {
    HMENU hmenu = GetSubMenu(GetSubMenu(GetMenu(hwnd), 1), 4);
    int len = GetMenuItemCount(hmenu);

    for (int i = 0; i < len; ++i) {
        CheckMenuItem(hmenu, i, MF_BYPOSITION | MF_UNCHECKED);
    }

    switch (LOWORD(parityChoice)) {
        case IDM_PARITY0:
            Parity = NOPARITY;
            CheckMenuItem(hmenu, LOWORD(parityChoice), MF_CHECKED);
            break;
        case IDM_PARITY1:
            Parity = ODDPARITY;
            CheckMenuItem(hmenu, LOWORD(parityChoice), MF_CHECKED);
            break;
        case IDM_PARITY2:
            Parity = EVENPARITY;
            CheckMenuItem(hmenu, LOWORD(parityChoice), MF_CHECKED);
            break;
        case IDM_PARITY3:
            Parity = MARKPARITY;
            CheckMenuItem(hmenu, LOWORD(parityChoice), MF_CHECKED);
            break;
        case IDM_PARITY4:
            Parity = SPACEPARITY;
            CheckMenuItem(hmenu, LOWORD(parityChoice), MF_CHECKED);
            break;
    }
}

DWORD WINAPI readListen(LPVOID hwnd) {
    DWORD input = 0;
    DWORD success;
    HDC hdc;
    static unsigned k = 0;
    OVERLAPPED over = {0};
    over.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    SetCommMask(hComm, EV_RXCHAR);
    bool readPending = false;
    while (writeKeys) {
        WaitCommEvent(hComm, nullptr, &over);
        if (ReadFile(hComm, str, 1, &success, &over)) {
            if (success) {
                hdc = GetDC((HWND)hwnd);             // get device context
                TextOut(hdc, 10 * k, 0, str, strlen(str)); // output character
                k++; // increment the screen x-coordinate
                ReleaseDC((HWND)hwnd, hdc); // Release device context
            }
        }
    }
    return 0;
}

void startWriting() {
    writeKeys = true;
}

void stopWriting() {
    writeKeys = false;
}

void startReading(HWND hwnd) {
    HANDLE hThread;
    DWORD threadID;
    hThread = CreateThread(NULL, 0, readListen, (LPVOID)hwnd, 0, &threadID);
}

void showHelp() {
    MessageBox(NULL, "Welcome to John agapeyev's Comp3980 Assignment 1!\nTo start,\
set the connection settings under the settings tab in the menu.\nEnsure one \
of each setting are selected to prevent errors.\nNext, under operation mode, \
select Connect.\nWith luck, you should be able to send and receive characters \
through the serial port!", "", MB_OK);
}

