#include <stdbool.h>
#include <windows.h>
#include "game.h"
#include "anim.h"
#include "ResourceUtils.h"
#include "KeyboardManager.h"
#include "reg.h"

#define RENDER_DATA_SIZE 44 // Size of each render data entry in bytes
extern int gamestateVariable_Unk;
extern int dword_4C0774; // Maximum number of render objects

extern void cleanupResources(void);
extern void setVolume(int volume);
extern HPALETTE createDefaultPalette(void);

// Global variables
static DWORD g_frameTickCount = 0;
static BOOL g_isGamePaused = FALSE;
static DWORD g_gameMaxTickCount = 0;
static DWORD g_frameMaxTickCount = 0;
static DWORD g_unknownVar1 = 0;
static DWORD g_unknownVar2 = 0;
static BOOL g_superflashGradBmpFlag = FALSE;
static HDC g_deviceContext = NULL;
static HWND g_parentWindow = NULL;
static DWORD g_lastActionTickTime = 0;
static int g_globalTimerValue = 0;
BYTE g_keyboardStateData[256];

int gamestateVariable_Unk = 0;
int g_gameStateVariable = 0;
int g_currentResourceSize2 = 0;

extern HWND mainWindowHandle; // This replaces dword_4C0794
extern int g_maxScreenWidth;
extern int g_maxScreenHeight;
extern int nBitCount;
extern int g_bitDepth;
static int g_currentRenderObjectCount = 0;
static int g_maxRenderObjects = 0;
static int32_t* g_renderDataArray = NULL;


extern int g_timerCurrentValue;
extern int globalTimerValue;

int g_isFullscreenReg = 0;
HDC g_hdc = NULL;
int g_isMouseActive = 0;
int g_isAudioEnabled = 0;
DWORD g_tickIncrementTable[] = { 16, 32, 48, 64 }; // Example values, adjust as necessary
int g_currentTickIncrementIndex = 0;
HWND mainWindowHandle = NULL;
int nBitCount = 32; // Example value, adjust as necessary



extern int isGraphicsSystemInitialized();

extern void* GlobalResourceArray;

int resetGlobalVariables() {
    g_isTimerDivisionActive = 0;
    g_timerInitialValue = 0;
    g_timerDivisionFactor = 0;
    g_timerDividedValue = 0;
    g_timerCurrentValue = globalTimerValue;
    return 0;
}

int DecrementAndResetVariables(void)
{
    int remainingDivisionFactor;

    if (!g_isTimerDivisionActive)
        return -1;

    remainingDivisionFactor = g_timerDivisionFactor - 1;
    g_timerCurrentValue -= g_timerDividedValue;

    if (g_timerCurrentValue < 0)
        g_timerCurrentValue = 0;

    --g_timerDivisionFactor;

    if (remainingDivisionFactor == 0)
        resetGlobalVariables();

    return 0;
}


int updateGameState1() {
    int index = 0;
    int successCount = 0;
    unsigned int i;
    DWORD* currentObject;
    int result;
    int success;

    for (i = 0; i < resourceArraySize; ++i) {
        for (currentObject = *((DWORD**)GlobalResourceArray + index); currentObject; currentObject = (DWORD*)*currentObject) {
            result = currentObject[1];
            if (result) {
                if (result == 1) {
                    success = fillRectColorWrapper(currentObject);
                }
                else {
                    success = -1;
                }
            }
            else {
                success = renderPixelAreaWrapper(currentObject);
            }
            if (!success) {
                ++successCount;
            }
        }
        index += 4;
    }
    cleanupResources();
    return successCount;
}

DWORD PerformDelayAction(int delayAmount) {
    DWORD currentTickCount = GetTickCount();
    DWORD endTickCount = currentTickCount + delayAmount;

    while (currentTickCount < endTickCount) {
        currentTickCount = GetTickCount();
    }

    return currentTickCount;
}

int InitializeWindow(HDC deviceContext) {
    HDC targetDC;
    HDC compatibleDC;
    HBITMAP bitmap;
    RECT clientRect;

    if (isGraphicsSystemInitialized())
        return -1;

    GetClientRect(mainWindowHandle, &clientRect);
    targetDC = deviceContext;
    if (!deviceContext)
        targetDC = GetDC(mainWindowHandle);

    compatibleDC = CreateCompatibleDC(targetDC);
    bitmap = CreateBitmap(g_maxScreenWidth, g_maxScreenHeight, 1, nBitCount, g_bitDepth);
    SelectObject(compatibleDC, bitmap);

    if (clientRect.right == g_maxScreenWidth && clientRect.bottom == g_maxScreenHeight)
        BitBlt(targetDC, 0, 0, g_maxScreenWidth, g_maxScreenHeight, compatibleDC, 0, 0, SRCCOPY);
    else
        StretchBlt(targetDC, 0, 0, clientRect.right, clientRect.bottom, compatibleDC, 0, 0, g_maxScreenWidth, g_maxScreenHeight, SRCCOPY);

    if (!deviceContext)
        ReleaseDC(mainWindowHandle, targetDC);

    DeleteDC(compatibleDC);
    DeleteObject(bitmap);
    return 0;
}

void resetGameState2(void)
{
    g_gameStateVariable = 0;
}

void resetResourceCounter2(void)
{
    g_currentResourceSize2 = 0;
}

int decrementAndResetVariables(void)
{
    if (!g_isTimerDivisionActive)
    {
        return -1;  // Timer division is not active
    }

    g_timerCurrentValue -= g_timerDividedValue;

    if (g_timerCurrentValue < 0)
    {
        g_timerCurrentValue = 0;
    }

    g_timerDivisionFactor--;

    if (g_timerDivisionFactor == 0)
    {
        resetGlobalVariables();
    }

    return 0;  // Success
}

int mainGameLoop(void)
{
    updateKeyboardState(g_keyboardStateData);

    if (g_isGamePaused)
    {
        updateAnimations();
        DecrementAndResetVariables();

        g_isGamePaused = (g_isGamePaused > g_gameSpeed) ? g_gameSpeed : g_isGamePaused;
        g_isGamePaused--;

        if (g_isGamePaused == 0)
        {
            cleanupResources();
            g_lastActionTickTime = GetTickCount();
        }
        return -1;
    }

    handleWindowMessages();
    DWORD currentTickCount = GetTickCount();
    updateGameState1();

    if (g_isGamePaused == 1)
    {
        g_isGamePaused = 0;
        processGraphicsAndBitmap();
    }

    updateAnimations();
    DecrementAndResetVariables();
    g_frameTickCount = GetTickCount();

    if (g_isFullscreenReg)
    {
        processScreenUpdatesAndResources();
    }
    else
    {
        InitializeWindow(g_hdc);
    }

    DWORD elapsedTime = GetTickCount() - g_lastActionTickTime;
    DWORD tickIncrement = g_tickIncrementTable[g_currentTickIncrementIndex];
    unsigned int ticksToDecrement = elapsedTime / tickIncrement;
    g_lastActionTickTime += tickIncrement * ticksToDecrement;

    if (ticksToDecrement > 0)
    {
        ticksToDecrement--;
    }

    g_isGamePaused = ticksToDecrement;

    if (tickIncrement - 3 > elapsedTime)
    {
        g_frameTickCount = elapsedTime;
        unsigned int delayTime = tickIncrement - elapsedTime - 3;
        if (delayTime > 2)
        {
            PerformDelayAction(delayTime);
        }
    }
    else
    {
        g_frameTickCount = elapsedTime;
        g_lastActionTickTime = GetTickCount();
    }

    if (g_gameMaxTickCount < ticksToDecrement)
    {
        g_gameMaxTickCount = ticksToDecrement;
    }

    if (g_frameMaxTickCount < elapsedTime)
    {
        g_frameMaxTickCount = elapsedTime;
    }

    g_isMouseActive = 0;
    return 0;
}

HFONT CreateCustomFont(HDC deviceContext, const char* fontName, int height, int width)
{
    if (!deviceContext || !fontName || fontName[0] <= ' ')
    {
        return NULL;
    }

    LOGFONTA logFont;
    memset(&logFont, 0, sizeof(logFont));

    logFont.lfHeight = height;
    logFont.lfWidth = width;
    logFont.lfEscapement = 0;
    logFont.lfOrientation = 0;
    logFont.lfWeight = FW_THIN;  // 100 is FW_THIN
    logFont.lfItalic = FALSE;
    logFont.lfUnderline = FALSE;
    logFont.lfStrikeOut = FALSE;
    logFont.lfCharSet = (unsigned char)fontName[0] >= 0x80 ? SHIFTJIS_CHARSET : ANSI_CHARSET;
    logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont.lfQuality = PROOF_QUALITY;  // 2 is PROOF_QUALITY
    logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    errno_t err = strncpy_s(logFont.lfFaceName, LF_FACESIZE, fontName, _TRUNCATE);
    if (err != 0 && err != STRUNCATE)
    {
        return NULL; 
    }

    HFONT createdFont = CreateFontIndirectA(&logFont);
    if (!createdFont)
    {
        return NULL;
    }

    SetMapperFlags(deviceContext, 1);  // 1 is ASPECT_FILTERING

    return createdFont;
}

DWORD setupGameEnvironment(void)
{
    g_frameTickCount = 0;
    g_isGamePaused = FALSE;
    g_gameMaxTickCount = 0;
    g_frameMaxTickCount = 0;
    g_unknownVar1 = 0;
    g_unknownVar2 = 0;
    g_superflashGradBmpFlag = FALSE;

    clearGlobalAnimationControl();
    cleanupResources();
    ResetJoystickInfo();
    updateKeyboardState(g_keyboardStateData);
    setVolume(g_globalTimerValue);

    if (!g_deviceContext)
    {
        g_deviceContext = GetDC(g_parentWindow);
        void* customFont = CreateCustomFont(g_deviceContext, "Arial", 12, 6);  // Assuming "Arial" as font name
        SelectObject(g_deviceContext, customFont);
        HPALETTE defaultPalette = createDefaultPalette();
        SelectPalette(g_deviceContext, defaultPalette, FALSE);
        RealizePalette(g_deviceContext);
    }

    updateColorInformation();

    g_lastActionTickTime = GetTickCount();
    return g_lastActionTickTime;
}

int updateRenderingData(uint32_t imageId, int renderFlag, int xOffset, int yOffset,
    int xPos, int yPos, int width, int height, int renderType)
{
    if (g_currentRenderObjectCount >= g_maxRenderObjects || !renderFlag)
    {
        return -1;
    }

    if (isRectangleWithinScreen(xPos, yPos, width, height))
    {
        return -1;
    }

    int32_t* currentRenderData = g_renderDataArray + (RENDER_DATA_SIZE / sizeof(int32_t)) * g_currentRenderObjectCount;

    currentRenderData[0] = 0;  // Initialize the first element

    if (updatePointerArray(imageId, currentRenderData) == -1)
    {
        return -1;
    }

    currentRenderData[1] = 0;
    currentRenderData[2] = renderFlag;
    currentRenderData[3] = xPos;
    currentRenderData[4] = yPos;
    currentRenderData[5] = width;
    currentRenderData[6] = height;
    currentRenderData[7] = xOffset;
    currentRenderData[8] = yOffset;
    currentRenderData[9] = renderType;

    g_currentRenderObjectCount++;

    return 0;
}

int isRectangleWithinScreen(int posX, int posY, int width, int height)
{
    // Check if the rectangle has valid dimensions
    if (width <= 0 || height <= 0)
    {
        return -1;
    }

    // Check if the rectangle is within or partially within the screen boundaries
    if (posX > g_maxScreenWidth || posY > g_maxScreenHeight)
    {
        return -1;
    }

    // Check if the rectangle extends beyond the left or top edge of the screen
    if (posX + width < 0 || posY + height < 0)
    {
        return -1;
    }

    // If all checks pass, the rectangle is within or partially within the screen
    return 0;
}