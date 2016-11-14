#pragma once

/****************************************/
/************** Headers *****************/
/****************************************/
#include <Windows.h>
#include <string>
#include <sstream>
#include <deque>
#include <tchar.h>


#include <platform.h>
#include <SkyetekProtocol.h>
#include <SkyeTekAPI.h>

#include "definitions.h"

/*****************************************/
/*******  CONSTANT DATA MEMBERS **********/
/*****************************************/
const TCHAR Name[]					= TEXT("COMP3980 A2 RFID Device Reader");
const DWORD WIN_LENGTH				= 500;
const DWORD WIN_WIDTH				= 900;
const DWORD BUFF_SIZE				= 256;
const DWORD TEXTBOX_HEIGTH			= 200;
const DWORD TEXTBOX_WIDTH			= 650;
const DWORD TAG_DISPLAY_X			= 190;
const DWORD TAG_DISPLAY_Y			= 10;
const DWORD TAG_HISTORY_DISPLAY_X	= 190;
const DWORD TAG_HISTORY_DISPLAY_Y	= 220;
const DWORD BUTTON_WIDTH			= 100;
const DWORD BUTTON_HEIGHT			= 30;
const DWORD DISCONNECT_BUTTON_X		= 50;
const DWORD DISCONNECT_BUTTON_Y		= 300;
const DWORD CONNECT_BUTTON_X		= 50;
const DWORD CONNECT_BUTTON_Y		= 250;

/****************************************/
/************* DATA MEMBERS *************/
/****************************************/
static BOOLEAN readLoopOn;
HANDLE hThread;

HWND hwnd;
HWND tagDisplay;
HWND tagHistDisplay;
HWND hConnectButton;
HWND hDisconnectButton;

DWORD  tagDisplay_yPos = 0;
DWORD  hwnd_yPos	   = 1;
/****************************************/
/*******  SKYETEK DATA MEMBERS **********/
/****************************************/

LPSKYETEK_DEVICE myDevice;
LPSKYETEK_DEVICE * devices = nullptr;
LPSKYETEK_READER * readers = nullptr;
SKYETEK_STATUS st;
DWORD numOfDevices = 0, numOfReaders = 0;


/****************************************/
/*********  MEMBER FUNCTIONS ************/
/****************************************/

/*************************************/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool setConfigParams(HWND, UINT, WPARAM, LPARAM);
void createGUI(HWND);
void switchButtonEnabled(HWND,HWND);
void connectPort(); /*connects to the port and changes the buttons*/
void disconnectPort(); /*changes the flag to disconnect from 
						 the port and changes the buttons*/

void displayReader(std::string);
void displayTag(std::string); /*displays a single tag*/
void addTagStrToHist(std::string); /*displays a set of tags*/
void repaintDisplayHist(); /*clears a display and resets the Y value*/
void clearDisplay(HWND, DWORD *);
void displayErrorMessageBox(LPCTSTR); /** displays an error message **/

void connect();
BOOLEAN connectRFID();
DWORD WINAPI readLoop(LPVOID);
unsigned char tagRead(LPSKYETEK_TAG, void* );
SKYETEK_STATUS ReadTagData(LPSKYETEK_TAG);
void disconnect();

//To String functions to parse the 16-bit uncode null characters 
std::string  GetData(LPSKYETEK_TAG);
std::string  tagTypeToString(LPSKYETEK_TAG);
std::string  friendlyToString(LPSKYETEK_TAG);
std::string  dataToString(LPSKYETEK_DATA);