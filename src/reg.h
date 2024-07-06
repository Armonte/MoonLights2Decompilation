// reg.h
#ifndef REG_H
#define REG_H

#include <windows.h>

// Define the settings structure (as before)
typedef struct {
    BYTE g_resourceHandlerState;
    int g_isFullscreenReg;
    int g_isAudioEnabled;
    int dword_43986C;
    int MAX_VOLUME;
    int g_screenResolutionFlag;
    int dword_439878;
    int isFullScreen;
    int dword_439880;
    int dword_439884;
    int dword_439888;
    int dword_43988C;
} GameSettings;

// Function prototypes
int regManCheckLoadSettings(GameSettings* settings, char* FullPath);
int regManSaveSettingsAndPath(GameSettings* settings, const char* FullPath);

#endif // REG_H