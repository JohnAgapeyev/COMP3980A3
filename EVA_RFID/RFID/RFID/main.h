/*------------------------------------------------------------------------------
-- SOURCE FILE: main.h - header file for RFID COMP 3980 Assignment
--
-- PROGRAM: RFID.exe
--
-- FUNCTIONS:
----------------------------------------------------
~~~~~~COMMAND FUNCTIONS~~~~~~
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool 			 setConfigParams(HWND, UINT, WPARAM, LPARAM);
void		 	 createGUI(HWND);

~~~~~~DISPLAY FUNCTIONS~~~~~~
void		 	 switchButtonEnabled(HWND,HWND);
void		 	 displayReader(std::string);	
void		 	 displayTag(std::string);       
void		 	 addTagStrToHist(std::string);  
void		 	 repaintDisplayHist();			
void		 	 clearDisplay(HWND, DWORD *);	 
void		 	 displayErrorMessageBox(LPCTSTR); 
void		 	 showHelp();					
void		 	 connectPort(); 
void		 	 disconnectPort(); 

~~~~~~CONNECTED FUNCTIONS~~~~~~
BOOLEAN 		 connect();						
BOOLEAN 		 connectRFID();				
DWORD WINAPI	 readLoop(LPVOID);		
unsigned char	 tagRead(LPSKYETEK_TAG, void* ); 
SKYETEK_STATUS   ReadTagData(LPSKYETEK_TAG);	 
void 			 disconnect();				   

~~~~~~DISPLAY HELP FUNCTIONS~~~~~~
std::string  GetData(LPSKYETEK_TAG); 
std::string  tagTypeToString(LPSKYETEK_TAG);
std::string  friendlyToString(LPSKYETEK_TAG);
std::string  dataToString(LPSKYETEK_DATA);
----------------------------------------------------
-- DATE: NOV. 16, 2016
--
-- REVISIONS: 
-- Version 1.0 -- attempting to implement without Skyetek API
-- Version 2.0 -- revised to use with Skyetek API, Event driven, w/ classes 
-- Version 3.0 -- revised to use with Skyetek API, Async, removed classes 
--
-- DESIGNER: Eva Yu, John Agapeyev
--
-- PROGRAMMER: Eva Yu
--
-- NOTES:
-- see source code for detailed function descriptions
------------------------------------------------------------------------------*/
#pragma once

/****************************************/
/************** Headers *****************/
/****************************************/
/*~~~~~~~~ WINDOWS/C++ ~~~~~~~*/
#include <Windows.h>
#include <string>
#include <sstream>
#include <deque>
#include <tchar.h>

/*~~~~~~~~~ SKYETEK ~~~~~~~~~~*/
#include <Platform.h>
#include <SkyetekProtocol.h>
#include <SkyeTekAPI.h>
/*~~~~~~~~~~ LOCAL ~~~~~~~~~~*/
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

static BOOLEAN readLoopOn = false;
HANDLE hThread;

/*~~~~~~~~ WINDOWS/VIEWS ~~~~~~~~*/
HWND hwnd;
HWND tagDisplay;
HWND tagHistDisplay;
HWND hConnectButton;
HWND hDisconnectButton;

DWORD  tagDisplay_yPos = 0;
DWORD  hwnd_yPos	   = 1;

/*~~~~~~~~ SKYTEK DATA MEMBERS ~~~~~~~~*/
LPSKYETEK_DEVICE myDevice;
LPSKYETEK_DEVICE * devices = nullptr;
LPSKYETEK_READER * readers = nullptr;
SKYETEK_STATUS st;
DWORD numOfDevices = 0, numOfReaders = 0;

/****************************************/
/*********  MEMBER FUNCTIONS ************/
/****************************************/

/*~~~~~~~~~~~~~~~~~~~~~~~~ COMMAND ~~~~~~~~~~~~~~~~~~~~~~~~*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool setConfigParams(HWND, UINT, WPARAM, LPARAM);
void createGUI(HWND);

/*~~~~~~~~~~~~~~~~~~~~~~~~ DISPLAY ~~~~~~~~~~~~~~~~~~~~~~~~*/
void switchButtonEnabled(HWND,HWND);

void displayReader(std::string);	/* displays reader status */
void displayTag(std::string);       /* displays a single tag */
void addTagStrToHist(std::string);  /* displays a set of tags */
void repaintDisplayHist();			/* clears a display and resets the Y value */
void clearDisplay(HWND, DWORD *);	/* Clears the window and resets the 
									value of the yPos(if not nullptr)*/
void displayErrorMessageBox(LPCTSTR); /* displays an error message */

/*~~~~~~~~~~~~~~~~~~~~~~~~ HELP ~~~~~~~~~~~~~~~~~~~~~~~~*/
void showHelp();					/* for help menu button */

/*~~~~~~~~~~~~~~~~~~~~~~~~ CONNECTED ~~~~~~~~~~~~~~~~~~~~~~~~*/
void connectPort(); /*Called on button clck to connects to
					the device, changes the buttons*/
void disconnectPort(); /*changes the flag to disconnect from
					   the port and changes the buttons*/
BOOLEAN connect();						/*Connects to the starts thread*/
BOOLEAN connectRFID();				/*Connects to RFID device*/
DWORD WINAPI readLoop(LPVOID);		/*the ASYNC thread loop to read tags*/
unsigned char tagRead(LPSKYETEK_TAG, void* ); /*reads tags and outputs 
											  tag info and ID */
SKYETEK_STATUS ReadTagData(LPSKYETEK_TAG);	 /*Outputs tagdata from 
										      block 1 - 10 */
void disconnect();				   /*frees the reader and device*/

/*~~~~~~~~~~~~ TO STRING DISPLAY HELPERS ~~~~~~~~~~~~~~~~~~~~*/
//To String functions to parse the 16-bit uncode null characters 
std::string  GetData(LPSKYETEK_TAG); 
std::string  tagTypeToString(LPSKYETEK_TAG);
std::string  friendlyToString(LPSKYETEK_TAG);
std::string  dataToString(LPSKYETEK_DATA);