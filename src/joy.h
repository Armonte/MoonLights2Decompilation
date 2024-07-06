#ifndef JOY_H
#define JOY_H

#include <windows.h>
#include <mmsystem.h>

extern int dword_4C07B0;
extern char inputBuffer1[256];
extern char inputBuffer2[256];

// Function prototypes
int ResetJoystickInfo();
int ProcessJoysticks(HWND hWndParent);
HWND InitJoystickWindow(HWND hWndParent);
LRESULT CALLBACK JoystickWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
int clearInputBuffers();
int SetGlobalParamAndResetKeyStates(int a1); 

#endif // JOY_H
