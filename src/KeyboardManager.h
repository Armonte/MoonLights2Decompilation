#pragma once
#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <windows.h>
#include <stdint.h>

extern uint8_t BYTE_ARRAY_0067a0a8[256];
extern BYTE g_keyState[256];
extern BYTE g_inputBuffer1[256];
extern BYTE g_inputBuffer2[256];
extern BYTE g_previousKeyState[256];

typedef struct
{
    int dummy; //dummy struct
} KeyboardManager;

// Function declarations
int updateKeyboardState(BYTE* g_KeyboardStateData);
void SetupKeyboard(HWND hWnd);
void ClearKeyboard(void);
int ReadKeyboard(int* mystery);
uint32_t ConvertReadInputsToBitArray(uint8_t button1,
    uint8_t button2,
    uint8_t button3,
    uint8_t button4,
    uint8_t button5,
    uint8_t button6,
    uint8_t button7,
    uint8_t button8);

#endif // KEYBOARD_MANAGER_H