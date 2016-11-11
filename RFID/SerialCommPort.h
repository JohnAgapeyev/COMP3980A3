#pragma once

#include <Windows.h>
#include <string>
#include <sstream>

class SerialCommPort
{
public:
    SerialCommPort();
    ~SerialCommPort();

    void init(HWND, HWND);
    BOOL connectPort(void);
    BOOL setDCBConfig(LPCSTR settings);
    BOOL disconnectPort(void);

    static DWORD WINAPI threadFuncCaller(LPVOID hwnd);
    void write(char *);
    BOOL read(void);

    HANDLE*    getCommHandle(void);
    std::stringstream * getReadStringStream(void);
    BOOL  isConnected(void);
    DWORD getBaudRate(void);
    DWORD getStopBit(void);
    CHAR  getParity(void);
    DWORD getDataBits(void);

    std::stringstream ssin;

    BOOL setBaudRate(DWORD);
    BOOL setStopBit(DWORD);
    BOOL setParity(DWORD);
    BOOL setDataBits(DWORD);

private:
    //Private Functions
    BOOL    setDefaultDCB(void);
    BOOL    setDCBConfig(void);
    BOOL    initComProp(void);
    BOOL    initTimeout(void);
    BOOL    initReadThread(void);
    BOOL    setReadWriteEventMask(void);

    void    displayIncomingInput(char * c);
    void    displayErrorMessageBox(LPCTSTR text);

    //CONST DATA MEMEBERS:
    static const DWORD BYTE_SIZE = 8;
    static const DWORD INPUT_BUFFSIZE = 256;
    static const DWORD READ_WRITE_BUFFSIZE = 1;
    static const DWORD OUTPUT_BUFFSIZE = 256;
    static const DWORD DW_SECONDS_CONST = 1000000;
    LPCTSTR ERR = TEXT("ERROR");
    LPTSTR lpszCommName = TEXT("com1");

    //OBJECTS / STRUCTS
    HWND         mainHwnd;
    HANDLE        hcomm; // handle to the UART serial port
    HANDLE        readThread;
    HWND         incomingHwnd;
    DCB            portDCB;
    COMMTIMEOUTS comTimeOut;
    OVERLAPPED    overlappedEvent;
    COMMPROP    commprop;
    DWORD        dwEventMask;

    //OVERLAPPED I/O Read Write DATA MEMEBERS
    OVERLAPPED    osReader;
    OVERLAPPED    osWriter;
    DWORD        dwRes;
    BOOL        fWaitingOnRead;
    BOOL        fRes;
    DWORD        dwBytesWritten;
    DWORD        toWriteBuffSize;
    DWORD        dwBytesRead;
    DWORD        dwSize;

    // user set DCB settings
    DWORD baudRate;
    DWORD stopBit;
    CHAR  parity;
    DWORD databits;

    //Other settings:
    BOOL connected;

    //char inputBuffer[INPUT_BUFFSIZE];
    char inputRead;
    char outputBuffer[OUTPUT_BUFFSIZE];
    int char_pos_readInputTextBox[2];
};