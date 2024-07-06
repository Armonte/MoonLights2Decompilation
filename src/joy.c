#include "joy.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
    UINT wXpos;
    UINT wYpos;
    UINT wZpos;
    UINT wButtons;
} JoyInfo;

int dword_4C07B0 = 0;
char inputBuffer1[256] = { 0 };
char inputBuffer2[256] = { 0 };

static JoyInfo joystickInfo[2];
static JoyInfo joystickInitInfo;
static DWORD joystickFlag1 = 0;
static DWORD joystickFlag2 = 0;
static DWORD activeJoystickCount = 0;
static DWORD joystickWindowHandle = 0;
static JOYCAPS joyCaps;
static WNDCLASSEX joystickWndClass;
static const char* joystickWindowClassName = "JoystickWindowClass";

// External joystick window procedure
LRESULT CALLBACK JoystickWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}
int ResetJoystickInfo()
{
    JoyInfo* joyInfoPtr = joystickInfo;
    joystickFlag1 = 0;
    joystickFlag2 = 0;
    joyInfoPtr[1].wXpos = 0;
    joystickInitInfo.wXpos = 0;

    int result = 0x8000;
    do
    {
        joyInfoPtr->wXpos = 0;
        joyInfoPtr->wYpos = 0;
        joyInfoPtr->wZpos = 0;
        joyInfoPtr->wButtons = 0;
        joyInfoPtr->wXpos = 0x8000;
        joyInfoPtr->wYpos = 0x8000;
        ++joyInfoPtr;
    } while (joyInfoPtr < &joystickInitInfo);
    return result;
}

int ProcessJoysticks(HWND hWndParent)
{
    int joystickIndex = 0;
    UINT_PTR joystickID;
    int numDevices = joyGetNumDevs();

    joystickFlag1 = 0;
    joystickFlag2 = 0;
    ResetJoystickInfo();

    if (numDevices > 0)
    {
        do
        {
            joystickID = (joystickIndex == 1) ? 1 : 0;
            memset(&joyCaps, 0, sizeof(joyCaps));

            if (joyGetDevCapsA(joystickID, &joyCaps, sizeof(joyCaps)))
                break;

            if (joyCaps.wNumAxes > 3)
            {
                JOYINFOEX joyInfoEx;
                memset(&joyInfoEx, 0, sizeof(joyInfoEx));

                if (joyGetPosEx(joystickID, &joyInfoEx))
                    break;
            }
            else
            {
                JOYINFO joyInfo;
                memset(&joyInfo, 0, sizeof(joyInfo));

                if (joyGetPos(joystickID, &joyInfo))
                    break;
            }
            ++joystickIndex;
        } while (joystickIndex < numDevices);
    }

    activeJoystickCount = joystickIndex;
    if (joystickIndex)
    {
        InitJoystickWindow(hWndParent);
        if (!joystickWindowHandle)
            return 0;
        joystickFlag1 = (DWORD)hWndParent;
    }
    return joystickIndex;
}

HWND InitJoystickWindow(HWND hWndParent)
{
    if (joystickWindowHandle)
    {
        DestroyWindow((HWND)joystickWindowHandle);
        joystickWindowHandle = 0;
    }

    memset(&joystickWndClass, 0, sizeof(joystickWndClass));
    joystickWndClass.cbSize = sizeof(WNDCLASSEX);
    joystickWndClass.style = 0;
    joystickWndClass.lpfnWndProc = JoystickWindowProc;
    joystickWndClass.cbClsExtra = 0;
    joystickWndClass.cbWndExtra = 0;
    joystickWndClass.hInstance = GetModuleHandle(NULL);
    joystickWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    joystickWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    joystickWndClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
    joystickWndClass.lpszClassName = joystickWindowClassName;
    joystickWndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&joystickWndClass);
    HWND hwnd = CreateWindowEx(
        0,
        joystickWindowClassName,
        NULL,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hWndParent,
        NULL,
        joystickWndClass.hInstance,
        NULL
    );

    joystickWindowHandle = (HWND)hwnd;
    return hwnd;
}

int clearInputBuffers() // Add this function
{
    int result; // eax

    result = 0;
    memset(&inputBuffer1, 0, 0x100u);
    memset(&inputBuffer2, 0, 0x100u);
    return result;
}

int SetGlobalParamAndResetKeyStates(int a1) // Add this function
{
    dword_4C07B0 = a1;
    clearInputBuffers();
    return 0;
}