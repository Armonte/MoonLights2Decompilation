#ifndef UNICODE
#define UNICODE
#endif
#include "main.h"
#include "reg.h"
#include "FileUtils.h"
#include "ResourceUtils.h"
#include "initGame.h"
#include <windows.h>
#include <strsafe.h>
#include <stringapiset.h>
#include <commctrl.h>
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"gdi32.lib")

Settings settings;
GameSettings g_settings;
#define ML2 L"Moon Lights 2"
#define ML2VER L"Moon Lights 2 Ver.1.07"
#define IDC_FOLDER_PATH 0x40a
#define IDC_BROWSE      0x40b
extern char* NO_USE;
extern char* JOYSTICK1;
extern char* JOYSTICK2;
char InitialFolderPath[260];
int g_gameSpeed = 0;

void* g_graphicsInterface = NULL;




LPCWSTR BITDEPTH_ERROR_JA = L"このゲ−ムは２５６色モ−ド専用です。\nスタ−トから→設定→コントロ−ルパネル→画面のプロパティ→ディスプレイの詳細のカラ−パレットの項を２５６色にして、\nマシンを再起動して下さい";
LPCWSTR BITDEPTH_ERROR_EN = L"This game is for 256-color mode only.\nFrom Start -> Settings -> Control Panel -> Display Properties -> Display Details -> Set the color palette section to 256 colors.\nPlease reboot your machine";

HWND gameWND;
HINSTANCE hInstance_004c15f4;
char DriveName_00687860[MAX_PATH * 2];
typedef enum {
    WINDOW_TYPE_0 = 0,
    WINDOW_TYPE_1 = 1,
    WINDOW_TYPE_2 = 2,
    WINDOW_TYPE_3 = 3
} window_t;



int UseActiveWindow(LPCWSTR window_name) {
    HWND hWnd;
    WINDOWPLACEMENT window_placement;

    hWnd = FindWindowEx((HWND)0, (HWND)0, window_name, (LPCWSTR)0);
    if (hWnd == (HWND)0) {
        return 0;
    }
    window_placement.length = 0x2c;
    if (GetWindowPlacement(hWnd, &window_placement) == 0) {
        return -1;
    }
    if (window_placement.showCmd == 2) {
        window_placement.showCmd = 1;
        if (SetWindowPlacement(hWnd, &window_placement) == 0) {
            return -1;
        }
    }
    SetActiveWindow(hWnd);
    SetForegroundWindow(hWnd);
    return -1;
}

void lollmao() {
    /*
      int msgboxID = MessageBox(
      NULL,
      (LPCWSTR)BITDEPTH_ERROR_EN,
      (LPCWSTR)ML2VER,
      MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
      );
    */
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    LPTSTR lpszFunction = TEXT("AA");
    DWORD dw = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s lol. lmao. failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
}

// yes this should be normal winmain but shift-JIS is a curse upon this land
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
 /*   int iVar1;
    int* IVar2;
    int* puVar3;*/

    InitCommonControls();
    if (UseActiveWindow(ML2) == -1) {
        return -1;
    }

    memset(DriveName_00687860, 0, 0x110);
    int drive = FindCDDrive();
    if (drive != -1) {
        DriveName_00687860[0] = (char)drive + 'A';
        DriveName_00687860[1] = ':';
        DriveName_00687860[2] = '\\';
        DriveName_00687860[3] = 0;
    }
    hInstance_004c15f4 = hInstance;

    Settings settings = { 0 };
    char fullPath[MAX_PATH] = { 0 };
    /* Registry Functions
    FUN_0042a3f0();regmankeyconfig
    FUN_00429fe0();regmancheckloadsettings
       */
    if (regManCheckLoadSettings(&settings, fullPath) == 0) {
        printf("Settings and path loaded successfully.\n");
    } else {
        printf("Failed to load settings and path.\n");
        return -1;
    } 
    if (DialogBoxParam(hInstance_004c15f4, L"IDD_BOOT",
        (HWND)0x0, MainDialogProc, 0) == -1) {
        lollmao();
        printf("dialogbox");
        return -1;
    }
    /*
    FUN_0042a270(); //regManSaveSettingsAndPath
    if (settings.v1 == 1) {
    DAT_00439860 = DAT_00439860 | 2;
    }
    iVar1 = findActiveWindow@FUN_00406a90((LPCSTR)ML2);
    if (iVar1 == -1) {
    return 0xffffffff;
    }
    _DAT_0068a3a8 = 1;
    */
    gameWND = CreateMainWindow(settings.window_type, hInstance, nCmdShow);
    if (gameWND == NULL) {
        return 0;
    }
    /* Sound Stuff
    if (DAT_00439868 == 0) {
        _DAT_006b2fd0 = 0xffffffff;
    }
    else {
        _DAT_006b2fd0 = soundrelated?@FUN_0042ac40(HWND@DAT_00688518);
    }
    soundorstringrelated@FUN_00410c50(s_13.ZMD_004398cc);
    */
    /* Registry Scores
    registryManage@FUN_0042a790();regmanscore
    registryStuff2@FUN_0042a920();regmansurv
    regsitryTimeAttack@FUN_0042aab0();
    */
    //_DAT_006b2e68 = 0;
    //init?@FUN_00413eb0();initgame
    initGame();
    //DestroyWindow(HWND@DAT_00688518);
    lollmao();
    return 0;
}

HWND CreateMainWindow(window_t type, HINSTANCE hInstance, int nCmdShow) {
    ATOM atom;
    HDC info_context;
    int iVar1;
    int iVar2;
    HMENU hmenu;
    HWND hwnd = NULL;
    LPVOID lpParam;
    WNDCLASS wnd_class_ex = {
        .hInstance = hInstance,
        .lpszClassName = ML2,
        .lpfnWndProc = WindowProc,
    };
    WNDCLASS wnd_class = {
        .hInstance = hInstance,
        .lpszClassName = ML2,
        .lpfnWndProc = WindowProc,
    };
    switch (type) {
    case 0:
        wnd_class_ex.style = 0;
        // EX Field
        // wnd_class_ex.cbSize = 0x30;
        // EX Field
        /* wnd_class_ex.hIconSm = LoadIcon((HINSTANCE)0x0,(LPCSTR)0x7f05); */
        // TODO:Figure out icon things later
        //wnd_class_ex.hIcon = LoadIcon(hInstance, "ICON");
        wnd_class_ex.hCursor = LoadCursor((HINSTANCE)0x0, (LPCWSTR)0x7f00);
        wnd_class_ex.lpszMenuName = 0x0;
        wnd_class_ex.cbClsExtra = 0;
        wnd_class_ex.cbWndExtra = 0;
        wnd_class_ex.hbrBackground = (HBRUSH)GetStockObject(0);
        atom = RegisterClass(&wnd_class_ex);
        if (atom == 0) {
            return (HWND)0x0;
        }
        hwnd = CreateWindowEx(0, ML2, ML2VER, 0xcf0000,
            (int)0x80000000, (int)0x80000000, 0x180, 0x140,
            (HWND)0x0, (HMENU)0x0, hInstance, (LPVOID)0x0);
        if (hwnd != (HWND)0x0) {
            info_context = CreateICA("Display", (LPCSTR)0x0, (LPCSTR)0x0, (DEVMODEA*)0x0);
            iVar1 = GetDeviceCaps(info_context, 0xc);
            DeleteDC(info_context);
            // This bit checks for 8bpp. As a modern machine has truecolor this has been changed from != to <
            // A value of 1 indicates greater than 8 color depth. Seems to be 32 for truecolor.
            if (iVar1 < 8 && iVar1 != 1) {
                MessageBox(hwnd, (LPCWSTR)BITDEPTH_ERROR_EN, ML2VER, 0);
                return (HWND)0x0;
            }
            ShowWindow(hwnd, nCmdShow);
            UpdateWindow(hwnd);
            return hwnd;
        }
        break;
    case 1:
        //iVar1 = FUN_0042b890(&DAT_004c1560);init device capabilities
        //if (iVar1 == -1) {
        //    return (HWND)0x0;
        // }
        //FUN_0042b900(8,0x280,0x1e0);
        wnd_class.cbClsExtra = 0;
        wnd_class.cbWndExtra = 0;
        wnd_class.style = 3;
        // TODO:Figure out icon things later
        //wnd_class.hIcon = LoadIcon(hInstance,(LPCSTR)0x7f00);
        wnd_class.hCursor = LoadCursor((HINSTANCE)0x0, (LPCWSTR)0x7f00);
        wnd_class.hbrBackground = (HBRUSH)0x0;
        wnd_class.lpszMenuName = ML2VER;
        RegisterClass(&wnd_class);
        lpParam = (LPVOID)0x0;
        hmenu = (HMENU)0x0;
        hwnd = (HWND)0x0;
        // Fullscreen
        iVar1 = GetSystemMetrics(1);
        iVar2 = GetSystemMetrics(0);
        hwnd = CreateWindowEx(8, ML2, ML2VER, 0x80000000,
            0, 0, iVar2, iVar1,
            hwnd, hmenu, hInstance, lpParam);
        if (hwnd != (HWND)0x0) {
            info_context = CreateICA("Display", (LPCSTR)0x0, (LPCSTR)0x0, (DEVMODEA*)0x0);
            iVar1 = GetDeviceCaps(info_context, 0xc);
            DeleteDC(info_context);
            if (iVar1 < 8 && iVar1 != 1) {
                MessageBox(hwnd, (LPCWSTR)BITDEPTH_ERROR_EN, ML2VER, 0);
                return (HWND)0x0;
            }
            ShowWindow(hwnd, nCmdShow);
            UpdateWindow(hwnd);
            SetFocus(hwnd);
            ShowCursor(0);
            return hwnd;
        }
        break;
    case 2:
        wnd_class.cbClsExtra = 0;
        wnd_class.cbWndExtra = 0;
        wnd_class.style = 3;
        // TODO:Figure out icon things later
        //wnd_class.hIcon = LoadIcon(hInstance,(LPCWSTR)0x7f00);
        wnd_class.hCursor = LoadCursor((HINSTANCE)0x0, (LPCWSTR)0x7f00);
        wnd_class.hbrBackground = (HBRUSH)0x0;
        wnd_class.lpszMenuName = ML2VER;
        RegisterClass(&wnd_class);
        hwnd = CreateWindowEx(0, ML2, ML2VER, 0xca0000,
            (int)0x80000000, (int)0x80000000, 0x106, 0x118, (HWND)0x0, (HMENU)0x0, hInstance,
            (LPVOID)0x0);
        if (hwnd != (HWND)0x0) {
            info_context = CreateICA("Display", (LPCSTR)0x0, (LPCSTR)0x0, (DEVMODEA*)0x0);
            iVar1 = GetDeviceCaps(info_context, 0xc);
            DeleteDC(info_context);
            if (iVar1 < 8 && iVar1 != 1) {
                MessageBox(hwnd, (LPCWSTR)BITDEPTH_ERROR_EN, ML2VER, 0);
                return (HWND)0x0;
            }
            ShowWindow(hwnd, nCmdShow);
            UpdateWindow(hwnd);
            //iVar1 = FUN_00406580(0,hwnd);//init ddraw?
            if (iVar1 == 0) {
                //MessageBox(hwnd,&DAT_00439938,(LPCSTR)ML2VER,0);
                DestroyWindow(hwnd);
                return (HWND)0x0;
            }
        }
        break;
    case 3:
        wnd_class.cbClsExtra = 0;
        wnd_class.cbWndExtra = 0;
        wnd_class.style = 3;
        // Figure out icon things later
        //wnd_class.hIcon = LoadIcon(hInstance,(LPCWSTR)0x7f00);
        wnd_class.hCursor = LoadCursor((HINSTANCE)0x0, (LPCWSTR)0x7f00);
        wnd_class.hbrBackground = (HBRUSH)0x0;
        wnd_class.lpszMenuName = ML2VER;
        RegisterClass(&wnd_class);
        lpParam = (LPVOID)0x0;
        hmenu = (HMENU)0x0;
        hwnd = (HWND)0x0;
        // Fullscreen
        iVar1 = GetSystemMetrics(1);
        iVar2 = GetSystemMetrics(0);
        hwnd = CreateWindowEx(8, ML2, ML2VER, 0x80000000, 0
            , 0, iVar2, iVar1, hwnd, hmenu, hInstance, lpParam);
        if (hwnd != (HWND)0x0) {
            ShowWindow(hwnd, nCmdShow);
            UpdateWindow(hwnd);
            SetFocus(hwnd);
            ShowCursor(0);
            //iVar1 = FUN_00406580(1,hwnd);
            if (iVar1 == 0) {
                //MessageBox(hwnd,&DAT_004398e0,(LPCSTR)ML2VER,0);
                DestroyWindow(hwnd);
                return (HWND)0x0;
            }
        }
    }
    return hwnd;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
 //   INT_PTR dialogResult;
    int browseResult;
    HWND controlHandle;
    size_t pathLength;
    WCHAR folderPath[MAX_PATH];
    WCHAR wideInitialPath[MAX_PATH];
#define IDC_FOLDER_PATH 0x40a
#define IDC_BROWSE 0x40b
    extern int g_isAudioEnabled;

    if (uMsg == WM_INITDIALOG) {
        regManCheckLoadSettings(&g_settings, DriveName_00687860);
        // Convert Shift-JIS to UTF-16
        MultiByteToWideChar(932, 0, DriveName_00687860, -1, wideInitialPath, MAX_PATH);
        SetDlgItemTextW(hwnd, IDC_FOLDER_PATH, wideInitialPath);
        SendDlgItemMessageW(hwnd, IDC_FOLDER_PATH, EM_SETSEL, 0, MAKELPARAM(0, -1));
        SetFocus(GetDlgItem(hwnd, IDC_FOLDER_PATH));
        if (g_isFullscreenReg == 0) {
            CheckRadioButton(hwnd, 0x65, 0x66, 0x66);
        }
        else {
            CheckRadioButton(hwnd, 0x65, 0x66, 0x65);
        }
        if (g_isAudioEnabled != 0) {
            CheckRadioButton(hwnd, 0x67, 0x67, 0x67);
        }
    }
    else if (uMsg == WM_COMMAND) {
        if (wParam < 0x66) {
            if (wParam == 0x65) {
                settings.window_type = settings.window_type | 1;
            }
            else if (wParam == IDOK) {
                EndDialog(hwnd, 0);
            }
            else if (wParam == IDCANCEL) {
                EndDialog(hwnd, -1);
            }
        }
        else {
            if (wParam < 0x403) {
                if (wParam != 0x402) {
                    if (wParam == 0x66) {
                        settings.window_type = 0;
                        return 0;
                    }
                    if (wParam != 0x67) {
                        return 0;
                    }
                    settings.v1 = settings.v1 + 1 & 1;
                    return 0;
                }
                if (DialogBoxParamW(hInstance_004c15f4, L"IDD_BOOT_OPTION", NULL, OptionsDialogProc, 0) == -1) {
                    return 0;
                }
            }
            else {
                if (wParam != IDC_BROWSE) {
                    return 0;
                }
                browseResult = BrowseAndSetFolderPath(hwnd, folderPath);
                if (browseResult == -1) {
                    return 0;
                }
                pathLength = wcslen(folderPath);
                if ((pathLength != 0) && (folderPath[pathLength - 1] != L'\\')) {
                    wcscat_s(folderPath, MAX_PATH, L"\\");
                }
                WideCharToMultiByte(932, 0, folderPath, -1, DriveName_00687860, sizeof(DriveName_00687860), NULL, NULL);
                SetDlgItemTextW(hwnd, IDC_FOLDER_PATH, folderPath);
                SendDlgItemMessageW(hwnd, IDC_FOLDER_PATH, EM_SETSEL, 0, MAKELPARAM(0, -1));
                controlHandle = GetDlgItem(hwnd, IDC_FOLDER_PATH);
                SetFocus(controlHandle);
            }
            regManSaveSettingsAndPath(&g_settings, DriveName_00687860);
        }
    }
    return 0;
}

LRESULT OptionsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == 0x111) {
        if (wParam < IDC_FOLDER_PATH) {
            if (wParam != 0x409) {
                if (wParam != 1) {
                    return 0;
                }
                EndDialog(hwnd, 0);
                return 0;
            }
            //regSaveLoadkeyconfig@FUN_0042a3f0();
            if (DialogBoxParam(hInstance_004c15f4, L"IDD_KEYCONFIG", (HWND)0x0,
                KeyConfigDialogProc, 0) != -1) {
                // reg keyconfig stuff 2
                //FUN_0042a550();
                return 0;
            }
        }
        else {
            switch (wParam) {
            case 0x40c:
                DialogBoxParam(hInstance_004c15f4, L"IDD_BOOT_DIRECT_SOUND", (HWND)0x0,
                    DirectSoundDialogProc, 0);
                return 0;
            case 0x40d:
                DialogBoxParam(hInstance_004c15f4, L"IDD_BOOT_DIRECTX_DISPLAY", (HWND)0x0,
                    DXDisplayDialogProc, 0);
                return 0;
            case 0x40e:
                DialogBoxParam(hInstance_004c15f4, L"IDD_BOOT_GAME_SPEED", (HWND)0x0,
                    GameSpeedDialogProc, 0);
                return 0;
            case 0x40f:
                DialogBoxParam(hInstance_004c15f4, L"IDD_BOOT_SOUND", (HWND)0x0,
                    SoundDialogProc, 0);
                return 0;
            case 0x414:
                DialogBoxParam(hInstance_004c15f4, L"IDD_BOOT_SPEED", (HWND)0x0,
                    SpeedDialogProc, 0);
            }
        }
    }
    return 0;
}

LRESULT KeyConfigDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return 0;
}

LRESULT DirectSoundDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        if (settings.directsound_enable != 0) {
            CheckRadioButton(hwnd, 0x408, 0x408, 0x408);
            return 0;
        }
    }
    else {
        if (uMsg != WM_COMMAND) {
            return 0;
        }
        if (wParam == 1) {
            EndDialog(hwnd, 0);
            return 0;
        }
        if (wParam != 0x408) {
            return 0;
        }
        settings.directsound_enable = settings.directsound_enable + 1U & 1;
    }
    return 0;
}

LRESULT GameSpeedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return 0;
}

LRESULT DXDisplayDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == 0x110) {
        if (settings.imagesize != 0) {
            CheckRadioButton(hwnd, 0x417, 0x417, 0x417);
        }
        if (settings.v18 != 0) {
            CheckRadioButton(hwnd, 0x419, 0x419, 0x419);
        }
        if (settings.resolution != 0) {
            CheckRadioButton(hwnd, 0x415, 0x416, 0x415);
            return 0;
        }
        CheckRadioButton(hwnd, 0x415, 0x416, 0x416);
        return 0;
    }
    if (uMsg != 0x111) {
        return 0;
    }
    if (0x415 < wParam) {
        if (wParam == 0x416) {
            settings.resolution = 0;
            CheckRadioButton(hwnd, 0x415, 0x416, 0x416);
            return 0;
        }
        if (wParam == 0x417) {
            settings.imagesize = settings.imagesize + 1U & 1;
            return 0;
        }
        if (wParam == 0x419) {
            settings.v18 = settings.v18 + 1U & 1;
            return 0;
        }
        return 0;
    }
    if (wParam == 0x415) {
        settings.resolution = 1;
        CheckRadioButton(hwnd, 0x415, 0x416, 0x415);
        return 0;
    }
    if (wParam == 1) {
        EndDialog(hwnd, 0);
        return 0;
    }
    return 0;
}

LRESULT SpeedDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        switch (g_gameSpeed) {
        case 0:
        case 1:
            CheckRadioButton(hwnd, 0x41b, 0x41f, 0x41b);
            return 0;
        case 2:
        case 3:
            CheckRadioButton(hwnd, 0x41b, 0x41f, 0x41c);
            return 0;
        case 4:
            CheckRadioButton(hwnd, 0x41b, 0x41f, 0x41d);
            return 0;
        case 5:
        case 6:
            CheckRadioButton(hwnd, 0x41b, 0x41f, 0x41e);
            return 0;
        default:
            CheckRadioButton(hwnd, 0x41b, 0x41f, 0x41f);
            return 0;
        }
    }
    if (uMsg != WM_COMMAND) {
        return 0;
    }
    if (0x41b < wParam) {
        switch (wParam) {
        case 0x41c:
            g_gameSpeed = 2;
            return 0;
        case 0x41d:
            g_gameSpeed = 4;
            return 0;
        case 0x41e:
            g_gameSpeed = 6;
            return 0;
        case 0x41f:
            g_gameSpeed = 8;
        }
        return 0;
    }
    if (wParam == 0x41b) {
        g_gameSpeed = 0;
        return 0;
    }
    if (wParam == 1) {
        EndDialog(hwnd, 0);
        return 0;
    }
    return 0;
}

LRESULT SoundDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        if (settings.bgm != 0) {
            CheckRadioButton(hwnd, 0x420, 0x420, 0x420);
        }
        if (settings.sound_effects != 0) {
            CheckRadioButton(hwnd, 0x421, 0x421, 0x421);
            return 0;
        }
    }
    else {
        if (uMsg != WM_COMMAND) {
            return 0;
        }
        if (wParam == 1) {
            EndDialog(hwnd, 0);
            return 0;
        }
        if (wParam == 0x420) {
            settings.bgm = settings.bgm + 1U & 1;
            return 0;
        }
        if (wParam != 0x421) {
            return 0;
        }
        settings.sound_effects = settings.sound_effects + 1U & 1;
    }
    return 0;
}

int handleWindowMessages(void)
{
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            exit((int)message.wParam);
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}