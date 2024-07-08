#pragma once
#include <windows.h>
#include "reg.h"

extern char FullPath[260];  // Assuming MAX_PATH length

typedef enum {
    DEFAULT,
    FULLSCREEN,
    DDRAW,
    DDRAWFULLSCREEN,
} window_t;

typedef struct {
    window_t window_type;
    UINT v1;
    UINT directsound_enable;
    UINT vc;
    UINT frameskip;
    UINT imagesize;
    // Greyed out? Desc is HW image optimization
    UINT v18;
    // 0 for 320, 1 for 640
    UINT resolution;
    // How fast the combat is
    UINT speed;
    UINT bgm;
    UINT sound_effects;
  //  UINT g_isFullscreenReg;
    UINT g_isAudioEnabled;
    // Unused?
    UINT v2c;
} Settings;



int handleWindowMessages(void);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
HWND CreateMainWindow(window_t type, HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int InitDirectDraw(int mode, HWND hwnd);
int DirectDrawSurfaceFunc();

INT_PTR CALLBACK  MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int UseActiveWindow(LPCWSTR window_name);

// Originally below keyconfig in the original but moved here for clarity
void DDrawSomething();

INT_PTR CALLBACK  KeyConfigDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  OptionsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  DirectSoundDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  GameSpeedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  DXDisplayDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  SpeedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  SoundDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
