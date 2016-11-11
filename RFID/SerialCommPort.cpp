#include "SerialCommPort.h"

SerialCommPort::SerialCommPort() {
    mainHwnd = nullptr;
    incomingHwnd = nullptr;
    connected = false;
    baudRate = 2400;
    stopBit = 1;
    parity = 'N';
    databits = 8;
    dwRes = 0;
    fWaitingOnRead = false;
    fRes = false;
    dwBytesWritten = 0;
    toWriteBuffSize = 1;
    dwBytesRead = 0;
    dwSize = 0;
}

SerialCommPort::~SerialCommPort() {
    if (connected) {
        disconnectPort();
    }
    CloseHandle(hcomm);
    mainHwnd = nullptr;
    incomingHwnd = nullptr;
    connected = false;
    baudRate = 0;
    stopBit = 0;
    parity = '\0';
    databits = 0;
    dwRes = 0;
    fWaitingOnRead = false;
    fRes = false;
    dwBytesWritten = 0;
    toWriteBuffSize = 1;
    dwBytesRead = 0;
    dwSize = 0;
}

void SerialCommPort::init(HWND hwnd, HWND inHwnd) {
    mainHwnd = hwnd;
    incomingHwnd = inHwnd;

    hcomm = CreateFile(lpszCommName,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED, //to have nonoverlapped: 0
        NULL);

    if (hcomm == INVALID_HANDLE_VALUE) {
        displayErrorMessageBox(TEXT("Error occured when opening port"));
        return;
    }
    initComProp();
}

BOOL SerialCommPort::connectPort(void) {
    if (IDYES == MessageBox(NULL,
        TEXT("Would you like to use default settings?"),
        TEXT("CONNECT"),
        MB_YESNO)) {
        setDefaultDCB();
    }
    if (
        initTimeout()
        && setReadWriteEventMask()
        && initReadThread()
        ) {
        connected = true;
        return true;
    }
    return false;
}
BOOL SerialCommPort::setDCBConfig(LPCSTR settings) {
    GetCommState(hcomm, &portDCB);
    portDCB.DCBlength = sizeof(DCB);
    DWORD bcdcb = BuildCommDCB(settings, &portDCB);
    if (bcdcb == 0) {
        displayErrorMessageBox(TEXT("Unable to configure the build the DCB"));
        return false;
    }
    return true;
}

BOOL SerialCommPort::disconnectPort(void) {
    if (GetCommState(hcomm, &portDCB)) {
        if (IDYES == MessageBox(NULL,
            TEXT("Are you sure you want to disconnect?"),
            TEXT("DISCONNECT"),
            MB_YESNO)) {
            CloseHandle(readThread);
            PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_TXCLEAR);
            connected = false;
            return true;
        }
    }
    return false;
}

DWORD SerialCommPort::threadFuncCaller(LPVOID hwnd) {
    SerialCommPort* thisPortInstance = static_cast<SerialCommPort*>(hwnd);
    thisPortInstance->read();
    return 0;
}

void SerialCommPort::write(char * lpOutBuff) {
    osWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osWriter.hEvent == NULL) {
        MessageBox(NULL,
            TEXT("Unable to configure the overlapped write read handle. "),
            TEXT("Error"),
            MB_OK);
        return;
    }

    if (connected) {
        DWORD dwBytesSent = 0;
        if (!WriteFile(hcomm, lpOutBuff, strlen(lpOutBuff), &dwBytesWritten, &osWriter)) {
            displayErrorMessageBox(TEXT("Unable to write string to port"));
            fRes = FALSE;
        } else {
            if (!GetOverlappedResult(hcomm, &osWriter, &dwBytesWritten, TRUE)) {
                fRes = FALSE;
            } else {
                // Write operation completed successfully.
                fRes = TRUE;
            }
        }
    } else {
        fRes = TRUE;
    }
    CloseHandle(osWriter.hEvent);
}

HANDLE * SerialCommPort::getCommHandle(void) {
    return &hcomm;
}

std::stringstream * SerialCommPort::getReadStringStream(void) {
    return &ssin;
}

BOOL SerialCommPort::isConnected() {
    return connected;
}

DWORD SerialCommPort::getBaudRate(void) {
    return baudRate;
}

DWORD SerialCommPort::getStopBit(void) {
    return stopBit;
}

CHAR SerialCommPort::getParity(void) {
    return parity;
}

DWORD SerialCommPort::getDataBits(void) {
    return databits;
}

BOOL SerialCommPort::setBaudRate(DWORD br) {
    return baudRate = br;
}

BOOL SerialCommPort::setStopBit(DWORD sb) {
    return stopBit = sb;
}

BOOL SerialCommPort::setParity(DWORD p) {
    return parity = p;
}

BOOL SerialCommPort::setDataBits(DWORD db) {
    return databits = db;
}

BOOL SerialCommPort::setDefaultDCB(void) {
    GetCommState(hcomm, &portDCB);
    portDCB.DCBlength = sizeof(DCB);
    LPCSTR dcbSettings = "baud=2400 parity=N data=8 stop=1";
    baudRate = 2400;
    parity = 'N';
    databits = 8;
    stopBit = 1;

    if (!BuildCommDCB(dcbSettings, &portDCB)) {
        displayErrorMessageBox(TEXT("Unable to configure the build the DCB"));
        return false;
    }
    return true;

}

BOOL SerialCommPort::initComProp(void) {
    if (!GetCommProperties(hcomm, &commprop)) {
        displayErrorMessageBox(TEXT("Error getting COM properties"));
        disconnectPort();
        return false;
    }
    return true;
}

BOOL SerialCommPort::initTimeout(void) {
    GetCommTimeouts(hcomm, &comTimeOut);

    comTimeOut.ReadIntervalTimeout = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier = 0;
    comTimeOut.ReadTotalTimeoutConstant = 0;
    comTimeOut.WriteTotalTimeoutMultiplier = 0;
    comTimeOut.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hcomm, &comTimeOut)) {
        displayErrorMessageBox(TEXT("Unable to to set Timeout"));
        return false;
    }
    return true;
}

BOOL SerialCommPort::initReadThread(void) {
    readThread = CreateThread(NULL,
        0,
        threadFuncCaller,
        this,
        0,
        NULL);
    if (readThread == NULL) {
        displayErrorMessageBox(TEXT("cannot create thread"));
        return false;
    }
    return true;
}

BOOL SerialCommPort::setReadWriteEventMask(void) {
    DWORD dwStoredFlags;
    dwStoredFlags = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | \
        EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY;

    if (!SetCommMask(hcomm, dwStoredFlags)) {
        displayErrorMessageBox(TEXT("Unable to set comm mask"));
        return false;
    }

    return true;
}

BOOL SerialCommPort::read(void) {
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osReader.hEvent == NULL) {
        displayErrorMessageBox(TEXT("Unable to configure the overlapped write event handle. "));
        return false;
    }
    while (!fWaitingOnRead) {
        if (ReadFile(hcomm, &inputRead, READ_WRITE_BUFFSIZE, &dwBytesRead, &osReader) == NULL) {
            if (GetLastError() != ERROR_IO_PENDING) {
                displayErrorMessageBox(TEXT("Unable to read from port."));
                return false;
            } else {
                fWaitingOnRead = TRUE;
            }
        } else {
            if (dwBytesRead > 0) {
                dwSize += dwBytesRead;
                ssin << inputRead;
                displayIncomingInput(&inputRead);
            }

        }
    }
    if (fWaitingOnRead) {
        dwRes = WaitForSingleObject(osReader.hEvent, 200);
        switch (dwRes) {
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(hcomm, &osReader, &dwBytesRead, FALSE)) {
                    MessageBox(NULL,
                        TEXT("Unable to read from port"),
                        TEXT("Error"),
                        MB_OK);
                } else {
                    if (dwBytesRead > 0) {
                        dwSize += dwBytesRead;
                        ssin << inputRead;
                        //displayIncoming(hwnd, char_x_pos, char_y_pos, width);
                        displayIncomingInput(&inputRead);
                    }

                    fWaitingOnRead = FALSE;
                }
                break;

            case WAIT_TIMEOUT:
                break;

            default:
                // Error in the WaitForSingleObject; abort.
                // This indicates a problem with the OVERLAPPED structure's
                // event handle.
                MessageBox(NULL,
                    TEXT("Unable to read from Overlapped IO port"),
                    TEXT("Error"),
                    MB_OK);
                break;
        }

    }
    CloseHandle(osReader.hEvent);
    return true;
}

void SerialCommPort::displayErrorMessageBox(LPCTSTR text) {
    MessageBox(mainHwnd, text, ERR, MB_OK);
}

void SerialCommPort::displayIncomingInput(char* c) {
    HDC hdc = GetDC(incomingHwnd);
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);


    TextOut(hdc, char_pos_readInputTextBox[0], char_pos_readInputTextBox[1], c, strlen(c)); // output character    

    *char_pos_readInputTextBox += tm.tmAveCharWidth; // increment the screen x-coordinate

    if (*char_pos_readInputTextBox + tm.tmAveCharWidth >= 400) {

        *char_pos_readInputTextBox = 0;
        char_pos_readInputTextBox[1] = char_pos_readInputTextBox[1] + tm.tmHeight + tm.tmExternalLeading;

    }
    ReleaseDC(incomingHwnd, hdc);
}
