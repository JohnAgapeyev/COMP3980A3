#pragma once
#include <Windows.h>
#include <string>
#include "DummyTerm_Definitions.h"
#include "SerialCommApp.h"
#define _CRT_SECURE_NO_WARNINGS 

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool setConfigParams(HWND, UINT, WPARAM, LPARAM);

static const int WIN_LENGTH = 500;          // Main window length
static const int WIN_WIDTH = 650;          // Main window height

static TCHAR Name[] = TEXT("COMP3980 A1 DUMMY TERMINAL COMM SHELL"); // 
HANDLE hcomm;
HWND hSettingsConfigWin;
LPTSTR lpszCommName = TEXT("com1");

SerialCommApp serialCommApp;

int WINAPI WinMain(HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lspszCmdParam,
    int nCmdShow) {
    HWND hwnd;
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

    Wcl.lpszMenuName = "DUMMY_TERM_MENU"; // no class menu 
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
        NULL, //parent of window 
        NULL, //menu bar
        hInst, // first aparam in WinMain
        NULL); //rarely used


    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
    UINT Message,
    WPARAM wParam,
    LPARAM lParam) {
    int response = 0;


    switch (Message) {
        case WM_CREATE:
            serialCommApp.init(hwnd);
            break;
        case WM_COMMAND:
            if ((HWND)lParam == serialCommApp.getConnectButton()) {
                serialCommApp.setConnect(hwnd);
                break;
            }
            if ((HWND)lParam == serialCommApp.getDisconnectButton()) {
                serialCommApp.setDisconnect();
                break;
            } else {
                serialCommApp.processComConfigSettings(hwnd, wParam);
            }

            break;
        case WM_SYSKEYDOWN:
        case WM_CHAR:
        case WM_KEYDOWN:
            serialCommApp.processKeyboardInput(hwnd, Message, wParam);
            break;
        case WM_PAINT:
            serialCommApp.repaintWindow(hwnd);
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


