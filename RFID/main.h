#pragma once

#include <Windows.h>
#include <string>
#include "definitions.h"
#include "controller.h" 

static TCHAR Name[] = TEXT("COMP3980 A2 RFID Device Reader");
LPTSTR lpszCommName = TEXT("com1");

HANDLE hcomm;
Controller controller;

HWND tagDisplay;
HWND tagHistDisplay;
HWND hConnectButton;
HWND hDisconnectButton;

const DWORD WIN_LENGTH			  = 500; 
const DWORD WIN_WIDTH			  = 650; 
const DWORD BUFF_SIZE			  = 256;
const DWORD TEXTBOX_HEIGTH		  = 200;
const DWORD TEXTBOX_WIDTH		  = 400;
const DWORD TAG_DISPLAY_X		  = 190;
const DWORD TAG_DISPLAY_Y		  = 10;
const DWORD TAG_HISTORY_DISPLAY_X = 190;
const DWORD TAG_HISTORY_DISPLAY_Y = 220;
const DWORD BUTTON_WIDTH		  = 100;
const DWORD BUTTON_HEIGHT	      = 30;
const DWORD DISCONNECT_BUTTON_X   = 50;
const DWORD DISCONNECT_BUTTON_Y   = 300;
const DWORD CONNECT_BUTTON_X	  = 50;
const DWORD CONNECT_BUTTON_Y	  = 250;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool setConfigParams(HWND, UINT, WPARAM, LPARAM);
void createGUI(HWND);