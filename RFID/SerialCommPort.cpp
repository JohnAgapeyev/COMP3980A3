#include "SerialCommPort.h"

/*--------------------------------------------------------------------------
-- FUNCTION: SerialCommPort Constructor
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void SerialCommPort (void)
-- 
-- NOTES:
-- instantiates all / most of the data membvers
-- the constructor the the instance that will represnet a "physical" layer
--------------------------------------------------------------------------*/

SerialCommPort::SerialCommPort()
{
    this->mainHwnd = nullptr;
    this->incomingHwnd = nullptr;
    this->connected = false;
    this->baudRate = 2400;
    this->stopBit = 1;
    this->parity = 'N';
    this->databits = 8;
    this->dwRes = 0;
    this->fWaitingOnRead = false;
    this->fRes = false;
    this->dwBytesWritten = 0;
    this->toWriteBuffSize = 1;
    this->dwBytesRead = 0;
    this->dwSize = 0;
}

/*--------------------------------------------------------------------------
-- FUNCTION: init
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void init (HWND, HWND)
-- HWND handle to the parent window
-- HWND handle to the window where the information from read will be output
--
-- NOTES:
-- intializes the instance of the com port activity
-- this function ensures that the UART file can be opened
-- and is ready for serial communication
--------------------------------------------------------------------------*/

SerialCommPort::~SerialCommPort()
{
    if (connected) {
        this->disconnectPort();
    }
    CloseHandle(hcomm);
    this->mainHwnd = nullptr;
    this->incomingHwnd = nullptr;
    this->connected = false;
    this->baudRate = 0;
    this->stopBit = 0;
    this->parity = '\0';
    this->databits = 0;
    this->dwRes = 0;
    this->fWaitingOnRead = false;
    this->fRes = false;
    this->dwBytesWritten = 0;
    this->toWriteBuffSize = 1;
    this->dwBytesRead = 0;
    this->dwSize = 0;
}


void SerialCommPort::init(HWND hwnd, HWND inHwnd)
{
    this->mainHwnd = hwnd;
    this->incomingHwnd = inHwnd;

    this->hcomm = CreateFile(this->lpszCommName,
                             GENERIC_READ | GENERIC_WRITE,
                             0, NULL, OPEN_EXISTING,
                             FILE_FLAG_OVERLAPPED, //to have nonoverlapped: 0
                             NULL);

    if (this->hcomm == INVALID_HANDLE_VALUE)
    {
        displayErrorMessageBox(TEXT("Error occured when opening port"));
        return;
    }
    this->initComProp();
}

/*--------------------------------------------------------------------------
-- FUNCTION: connectPort
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: BOOL connectPort (void)
--
-- RETURNS: 
-- boolean representing success / failure of connection established
--
-- NOTES:
-- this is the physical layers connection the port
-- the calll is made by the application layer and established by session layer
-- this is where all the structs are intialized 
-- a thread for reading from port is intialized here 
--------------------------------------------------------------------------*/

BOOL SerialCommPort::connectPort(void)
{
    if (IDYES == MessageBox(NULL,
                            TEXT("Would you like to use default settings?"),
                            TEXT("CONNECT"),
                            MB_YESNO)) 
    {
        this->setDefaultDCB();
    }
    if (
        this->initTimeout()
        && this->setReadWriteEventMask()
        && this->initReadThread()
        )
    {
        this->connected = true;
        return true;
    }
    return false;
}
BOOL SerialCommPort::setDCBConfig(LPCSTR settings) {
    GetCommState(hcomm, &portDCB);
    portDCB.DCBlength = sizeof(DCB);
    DWORD bcdcb = BuildCommDCB(settings, &portDCB);
    if (bcdcb == 0)
    {
        displayErrorMessageBox(TEXT("Unable to configure the build the DCB"));
        return false;
    }
    return true;
}
/*--------------------------------------------------------------------------
-- FUNCTION: diconnects from port 
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: [returntype] disconnectPort (void)
--
-- NOTES:
-- 
--------------------------------------------------------------------------*/
BOOL SerialCommPort::disconnectPort(void)
{
    if (GetCommState(hcomm, &portDCB)) {
        if (IDYES == MessageBox(NULL,
                                TEXT("Are you sure you want to disconnect?"),
                                TEXT("DISCONNECT"),
                                MB_YESNO))
        {
            CloseHandle(readThread);
            PurgeComm(hcomm, PURGE_RXCLEAR | PURGE_TXCLEAR);
            this->connected = false;
            return true;
        }
    }
    return false;
}

/*--------------------------------------------------------------------------
-- FUNCTION: threadFuncCaller
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: DOWRD threadFuncCaller (LPVOID)
-- LPVOID str --arguments for the thread call 
--
-- RETURNS: 
-- DWORD as specified by the thread function pointer
-- DWORD here does not serve much of a purpose 
--
-- NOTES:
--  this is a static function that will call a member function 
-- this is a helper function
--------------------------------------------------------------------------*/

DWORD SerialCommPort::threadFuncCaller(LPVOID hwnd)
{
    SerialCommPort* thisPortInstance = static_cast<SerialCommPort*>(hwnd);
    thisPortInstance->read();
    return 0;
}

/*--------------------------------------------------------------------------
-- FUNCTION: write
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void [write] lpOutBuff
-- lpOutBuff pointer to char buffer that indicates the characters to be sent via the port. 
--
-- RETURNS: 
--
-- NOTES:
--------------------------------------------------------------------------*/

void SerialCommPort::write(char * lpOutBuff)
{
    osWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osWriter.hEvent == NULL)
    {
        MessageBox(NULL,
                   TEXT("Unable to configure the overlapped write read handle. "),
                   TEXT("Error"),
                   MB_OK);
        //disconnect();
        return;
    }

    if (this->connected) {
        DWORD dwBytesSent = 0;
        if (!WriteFile(this->hcomm, lpOutBuff, strlen(lpOutBuff), &this->dwBytesWritten, &osWriter))
        {
            displayErrorMessageBox(TEXT("Unable to write string to port"));
            fRes = FALSE;
        }
        else {
            if (!GetOverlappedResult(hcomm, &osWriter, &dwBytesWritten, TRUE))
            {
                fRes = FALSE;
            }
            else
            {
                // Write operation completed successfully.
                fRes = TRUE;
            }
        }
    }
    else
    {
        fRes = TRUE;
    }
    CloseHandle(osWriter.hEvent);
}

/*--------------------------------------------------------------------------
-- FUNCTION: getCommHandle
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: void getCommHandle (void)
--
-- NOTES:
-- GEtter for the handle to the comm port 
-- for resuasbility and ecanpsulation
*/

HANDLE * SerialCommPort::getCommHandle(void)
{
    return &(this->hcomm);
}

std::stringstream * SerialCommPort::getReadStringStream(void)
{
    return &(this->ssin);
}
/*--------------------------------------------------------------------------
-- FUNCTION: isConnected
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- NOTES:
-- checks that i
--------------------------------------------------------------------------*/
BOOL SerialCommPort::isConnected()
{
    return this->connected;
}

DWORD SerialCommPort::getBaudRate(void)
{
    return this->baudRate;
}

DWORD SerialCommPort::getStopBit(void)
{
    return this->stopBit;
}

CHAR SerialCommPort::getParity(void)
{
    return this->parity;
}

DWORD SerialCommPort::getDataBits(void)
{
    return this->databits;
}

BOOL SerialCommPort::setBaudRate(DWORD br)
{
    return this->baudRate = br;
}

BOOL SerialCommPort::setStopBit(DWORD sb)
{
    return this->stopBit = sb;
}

BOOL SerialCommPort::setParity(DWORD p)
{
    return this->parity = p;
}

BOOL SerialCommPort::setDataBits(DWORD db)
{
    return  this->databits = db;
}

BOOL SerialCommPort::setDefaultDCB(void)
{
    GetCommState(hcomm, &portDCB);
    portDCB.DCBlength = sizeof(DCB);
    LPCSTR dcbSettings = "baud=2400 parity=N data=8 stop=1";
    baudRate = 2400;
    parity = 'N';
    databits  = 8;
    stopBit = 1;

    if (!BuildCommDCB(dcbSettings, &portDCB))
    {
        displayErrorMessageBox(TEXT("Unable to configure the build the DCB"));
        return false;
    }
    return true;

}
/*--------------------------------------------------------------------------
-- FUNCTION: initComProp
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: bool initComProp (void)
--
-- RETURNS: 
-- BOOL true is comprop was set successfully
-- 
-- NOTES:
-- the begining of a series of initializers .. brace yourelf.
*/
BOOL SerialCommPort::initComProp(void)
{
    if (!GetCommProperties(hcomm, &commprop))
    {
        displayErrorMessageBox(TEXT("Error getting COM properties"));
        disconnectPort();
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------
-- FUNCTION: initTimeout
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: bool initTimeout (void)
--
-- RETURNS: 
-- BOOL true is timeout struct was set successfully
-- 
-- NOTES:
-- an initializer
--------------------------------------------------------------------------*/

BOOL SerialCommPort::initTimeout(void)
{
    GetCommTimeouts(hcomm, &comTimeOut);

    comTimeOut.ReadIntervalTimeout = MAXDWORD;
    comTimeOut.ReadTotalTimeoutMultiplier = 0;
    comTimeOut.ReadTotalTimeoutConstant = 0;
    comTimeOut.WriteTotalTimeoutMultiplier = 0;
    comTimeOut.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hcomm, &comTimeOut))
    {
        displayErrorMessageBox(TEXT("Unable to to set Timeout"));
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------
-- FUNCTION: initReadThread
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: bool initReadThread (void)
--
-- RETURNS: 
-- BOOL true is timeout struct was set successfully
-- 
-- NOTES:
-- an initializer
--------------------------------------------------------------------------*/

BOOL SerialCommPort::initReadThread(void) {
    this->readThread = CreateThread(NULL,
                                    0,
                                    threadFuncCaller,
                                    this,
                                    0,
                                    NULL);
    if (readThread == NULL)
    {
        displayErrorMessageBox(TEXT("cannot create thread"));
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------
-- FUNCTION: initReadThread
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: bool initReadThread (void)
--
-- RETURNS: 
-- BOOL true is timeout struct was set successfully
-- 
-- NOTES:
-- an initializer
--------------------------------------------------------------------------*/

BOOL SerialCommPort::setReadWriteEventMask(void)
{
    DWORD dwStoredFlags;
    dwStoredFlags = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | \
        EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY;

    if (!SetCommMask(hcomm, dwStoredFlags))
    {
        displayErrorMessageBox(TEXT("Unable to set comm mask"));
        return false;
    }

    return true;
}
/*--------------------------------------------------------------------------
-- FUNCTION: read
--
-- DATE: OCT. 05, 2016
--
-- REVISIONS: Set Version 2.0
--
-- DESIGNER: Eva Yu
--
-- PROGRAMMER: Eva Yu
--
-- INTERFACE: BOOL read (void)
-- [functionParam0] [description]
-- [functionParam1] [description]
-- [functionParam2] [description]
--
-- RETURNS: 
-- true if write succeeds
--
--------------------------------------------------------------------------*/
BOOL SerialCommPort::read(void)
{

    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    if (osReader.hEvent == NULL) {
        displayErrorMessageBox(TEXT("Unable to configure the overlapped write event handle. "));
        return false;
    }
        while (!fWaitingOnRead) {
            if (ReadFile(this->hcomm, &this->inputRead, READ_WRITE_BUFFSIZE, &dwBytesRead, &osReader) == NULL)
            {
                if (GetLastError() != ERROR_IO_PENDING) {
                    displayErrorMessageBox(TEXT("Unable to read from port."));
                    return false;
                }
                else {
                    fWaitingOnRead = TRUE;
                }

            }
            else
            {
                if (dwBytesRead > 0)
                {
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
                if (!GetOverlappedResult(hcomm, &osReader, &dwBytesRead, FALSE))
                {
                    MessageBox(NULL,
                               TEXT("Unable to read from port"),
                               TEXT("Error"),
                               MB_OK);
                }
                else
                {
                    if (dwBytesRead > 0)
                    {
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
void SerialCommPort::displayErrorMessageBox(LPCTSTR text)
{
    MessageBox(mainHwnd, text, ERR, MB_OK);
}

/*---------------------------------------------------------------------
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
-- [SerialCommPort::displayIncomingInput description]
-- @param c [displays incoming characters -- SHould not be here!]
---------------------------------------------------------------------------*/
void SerialCommPort::displayIncomingInput(char* c)
{
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
