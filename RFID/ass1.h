
#pragma once
#define IDM_Port          102
#define IDM_Speed         103
#define IDM_Word          104
#define IDM_ZETA          105
#define IDM_Save          106
#define IDM_THETA         107
#define IDM_HELP          108
#define IDM_COM1          109
#define IDM_COM2          110
#define IDM_COM3          111
#define IDM_COM4          112
#define IDM_110           113
#define IDM_300           114
#define IDM_600           115
#define IDM_1200          116
#define IDM_2400          117
#define IDM_4800          118
#define IDM_9600          119
#define IDM_14400         120
#define IDM_19200         121
#define IDM_38400         122
#define IDM_57600         123
#define IDM_115200        124
#define IDM_128000        125
#define IDM_256000        126
#define IDM_DATA4         127
#define IDM_DATA5         128
#define IDM_DATA6         129
#define IDM_DATA7         130
#define IDM_DATA8         131
#define IDM_STOP0         132
#define IDM_STOP1         133
#define IDM_STOP2         134
#define IDM_PARITY0       135
#define IDM_PARITY1       136
#define IDM_PARITY2       137
#define IDM_PARITY3       138
#define IDM_PARITY4       139
#define IDM_COMMAND       140
#define IDM_CONNECT       141
#define IDM_EXIT          142

bool saveSettings(HWND hwnd);

void changeCOMPort(HWND hwnd, WPARAM comSetting);

void setBaudRate(HWND hwnd, WPARAM chosenRate);

void setByteSize(HWND hwnd, WPARAM chosenSize);

void setStopBits(HWND hwnd, WPARAM stopChoice);

void setParityMode(HWND hwnd, WPARAM parityChoice);

void startReading(HWND hwnd);

void startWriting();

void stopWriting();

void showHelp();

