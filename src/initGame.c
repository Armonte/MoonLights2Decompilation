#include "initGame.h"
#include <stdio.h>
#include "ResourceUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "joy.h"
#include "bitmap.h"
#include "main.h"
#include "reg.h"
#include "FileUtils.h"
#include "KeyboardManager.h"
#include "audio.h"
#include "timers.h"
#include <direct.h>
uint8_t* p2_dataLimit = NULL;
char FullPath[260] = { 0 };
int g_globalTimerValue = 0;
char VersionString[260] = "Version 1.0";
uint8_t byte_6B2E65 = 0;
uint8_t byte_6B2F74 = 0;
uint8_t* title_bmp_data = NULL;
uint8_t* psel_bmp_data = NULL;
uint8_t* tokyo_bmp_data_pointer = NULL;
uint8_t* grad_bmp_data = NULL;
int dword_6B3004 = 0;
uint8_t byte_6B2BB8 = 0;
uint8_t byte_6B2BB9 = 0;
int HITSTOP_SLOWDOWN[10] = { 0 }; // Assuming the size based on common usage
int dword_6B2E08 = 0;
int dword_688028 = 0;
int dword_6B2FD4 = 0;
int dword_6B2BB4 = 0;
int gfx_spritetrail = 0;
int gamespeed = 0;
int option_difficulty_copy = 0;
int option_difficulty = 0;
int option_timer = 0;
int roundsToWin = 0;
int p2_char = 0;
int gametype_enabled = 0;
int character[10] = { 0 }; // Assuming the size based on common usage
int p1_setCPU[10] = { 0 }; // Assuming the size based on common usage
int p2_setCPU = 0;
int hitcount[10] = { 0 }; // Assuming the size based on common usage
uint8_t byte_6B2DFC = 0;
int* p1_health = NULL;
int dword_6B2E10 = 0;
int* dword_6B2E60 = NULL;
int spriteDataLimitArray = 0;
int dword_6B2E14 = 0;
int* p1_meter_Stocks = NULL;


#ifdef _MSC_VER
__declspec(dllimport)
#endif
void split_path(const char* path, char* drive, char* dir, char* fname, char* ext);

#ifdef _MSC_VER
__declspec(dllimport)
#endif
void _makepath(char* path, const char* drive, const char* dir, const char* fname, const char* ext);


int css_general(void) {
    return 0; 
}
int MainBattle(void) {
    return 0; 
}
int mmBgScroll(void) {
    return 0;
}
int mmCssTeamBattle(void) {
    return 0;
}
int sub_41F290(void) {
    return 0;
}

int HandleP1Inputs(void) {
    return 0;
}
int HandleP2Inputs(void) {
    return 0;
}

int GameLoop(void) {
    return 0;
}

extern int mainGameLoop();

extern int updateRenderingData(uint32_t imageId, int renderFlag, int xOffset, int yOffset,
    int xPos, int yPos, int width, int height, int renderType);

int CustomDirectoryProcessing(void);
//void split_path(const char* path, char* drive, char* dir, char* fname, char* ext);
//void _makepath(char* path, const char* drive, const char* dir, const char* fname, const char* ext);
//void CustomDataCopy(const char* path, void* array);
int CustomAudioControl(void);
//void ProcessJoysticks(void* hWndParent);
//void SetGlobalParamAndResetKeyStates(void* hWndParent);
//int InitAnimationControl(void* hWndParent, int a, int b, int c);
int ReallocateGlobalResourceArray(int newSize);
int ReallocateRenderBuffer(int size);
int ReallocateMemoryBuffer(int size);
HPALETTE handleGlobalPalette(HPALETTE palette);
//void initAndRunGame(void);
void* ProcessAndFindMatchingEntry(const char* fileName, unsigned int fileOffset, size_t Size, size_t* processedSize);

BYTE* ProcessBitmapData(void* bitmapHandle, HBITMAP* outBitmap, DWORD* outColorCount, int* outWidth, int* outHeight);

void LoadSfxFileBundle(void);
void HandleGameOptionsRegistry(void);
void InitializeGameResources(void);
int nullsub_2(void);
int GameLoop(void);
int css_general(void);
void nullsub_1(int a, int b);
void MainEventLoop(void);
//void MainBattle(void); stubbed
void mmManualPages(void);
void audioReset(void);
int mmBgScroll(void);
//void sub_41F290(void); stubbed
int mmCssTeamBattle(void);
DWORD setupGameEnvironment();


void* tmpPtr;

typedef struct {
    char name[20];
    bool compressed;
    uint32_t offset;
    uint32_t size;
} NcdFile;

NcdFile ncd_array[] = {
    {"LOGO.BMP",     true,  0x00000063, 0x000946},
    {"PSEL.BMP",     true,  0x000009A9, 0x0079E2},
    {"TITLE.BMP",    true,  0x0000838B, 0x0026B6},
    {"TOKYO.BMP",    true,  0x0000AA41, 0x00CEF0},
    {"SE.NCD",       true,  0x00017931, 0x023752},
    {"MANUALBG.BMP", true,  0x0003B083, 0x00B336},
    {"SIGN.BMP",     true,  0x000463B9, 0x004A7A},
    {"GRAD.BMP",     true,  0x0004AE33, 0x003034},
    {"AE.FRM",       true,  0x0004DE6A, 0x002234},
    {"AE.SP",        true,  0x0005009E, 0x012479},
    {"AE.PAL",       true,  0x00062517, 0x00011B},
    {"AE.VSE",       true,  0x00062632, 0x0032C4},
    {"AE.NCD",       true,  0x000658F6, 0x00D4C5},
    {"AS.FRM",       true,  0x00072DBE, 0x003F99},
    {"AS.SP",        true,  0x00076D57, 0x012BE2},
    {"AS.PAL",       true,  0x00089939, 0x000128},
    {"AS.VSE",       true,  0x00089A61, 0x0033E6},
    {"AS.NCD",       true,  0x0008CE47, 0x0175D8},
    {"CH.FRM",       true,  0x000A4422, 0x003DA1},
    {"CH.SP",        true,  0x000A81C3, 0x0138FD},
    {"CH.PAL",       true,  0x000BBAC0, 0x000129},
    {"CH.VSE",       true,  0x000BBBE9, 0x0036B2},
    {"CH.NCD",       true,  0x000BF29B, 0x017C70},
    {"FU.FRM",       true,  0x000D6F0E, 0x003CF0},
    {"FU.SP",        true,  0x000DABFE, 0x00FD54},
    {"FU.PAL",       true,  0x000EA952, 0x000103},
    {"FU.VSE",       true,  0x000EAA55, 0x0035DA},
    {"FU.NCD",       true,  0x000EE02F, 0x013172},
    {"HA.FRM",       true,  0x001011A4, 0x002B45},
    {"HA.SP",        true,  0x00103CE9, 0x0122A2},
    {"HA.PAL",       true,  0x00115F8B, 0x0000FE},
    {"HA.VSE",       true,  0x00116089, 0x002F58},
    {"HA.NCD",       true,  0x00118FE1, 0x0188C3},
    {"HI.FRM",       true,  0x001318A7, 0x004677},
    {"HI.SP",        true,  0x00135F1E, 0x014895},
    {"HI.PAL",       true,  0x0014A7B3, 0x0000EB},
    {"HI.VSE",       true,  0x0014A89E, 0x0033AC},
    {"HI.NCD",       true,  0x0014DC4A, 0x00AAB7},
    {"HO.FRM",       true,  0x00158704, 0x004321},
    {"HO.SP",        true,  0x0015CA25, 0x0126FB},
    {"HO.PAL",       true,  0x0016F120, 0x0000E8},
    {"HO.VSE",       true,  0x0016F208, 0x003895},
    {"HO.NCD",       true,  0x00172A9D, 0x011CF4},
    {"KA.FRM",       true,  0x00184794, 0x001E7D},
    {"KA.SP",        true,  0x00186611, 0x00AF8D},
    {"KA.PAL",       true,  0x0019159E, 0x000118},
    {"KA.VSE",       true,  0x001916B6, 0x002A70},
    {"KA.NCD",       true,  0x00194126, 0x013D79},
    {"RE.FRM",       true,  0x001A7EA2, 0x00432C},
    {"RE.SP",        true,  0x001AC1CE, 0x0117DC},
    {"RE.PAL",       true,  0x001BD9AA, 0x000144},
    {"RE.VSE",       true,  0x001BDAEE, 0x0039A9},
    {"RE.NCD",       true,  0x001C1497, 0x00E806},
    {"RR.FRM",       true,  0x001CFCA0, 0x003FF4},
    {"RR.SP",        true,  0x001D3C94, 0x012848},
    {"RR.PAL",       true,  0x001E64DC, 0x00011C},
    {"RR.VSE",       true,  0x001E65F8, 0x0030A6},
    {"RR.NCD",       true,  0x001E969E, 0x0182EC},
    {"RY.FRM",       true,  0x0020198D, 0x002F88},
    {"RY.SP",        true,  0x00204915, 0x012A82},
    {"RY.PAL",       true,  0x00217397, 0x0000AF},
    {"RY.VSE",       true,  0x00217446, 0x002E6D},
    {"RY.NCD",       true,  0x0021A2B3, 0x019A27},
    {"UM.FRM",       true,  0x00233CDD, 0x003470},
    {"UM.SP",        true,  0x0023714D, 0x010061},
    {"UM.PAL",       true,  0x002471AE, 0x0000F1},
    {"UM.VSE",       true,  0x0024729F, 0x002DB8},
    {"UM.NCD",       true,  0x0024A057, 0x00AE7F},
    {"US.FRM",       true,  0x00254ED9, 0x00386F},
    {"US.SP",        true,  0x00258748, 0x012573},
    {"US.PAL",       true,  0x0026ACBB, 0x0000F8},
    {"US.VSE",       true,  0x0026ADB3, 0x0031A8},
    {"US.NCD",       true,  0x0026DF5B, 0x014906},
    {"MA.FRM",       true,  0x00282864, 0x005027},
    {"MA.SP",        true,  0x0028788B, 0x01ACD2},
    {"MA.PAL",       true,  0x002A255D, 0x0000E7},
    {"MA.VSE",       true,  0x002A2644, 0x00412F},
    {"MA.NCD",       false, 0x002A6773, 0x017CC8}
};

int initGame() {
    //   uint8_t* toktoBitmapDataPtr;
//   uint8_t* gradBitmapDataPtr;
//   int pixelIndex;
    char customDirResult;
    int processedPixelCount;
    int gameLoopResult;
    int* resourceArrayPointer;
    int16_t* cpuSetPointer;
    int savedOptionTimer;
    int savedRoundsToWin;
    int gameModeResult;
    int battleResult;
    int16_t* cpuCharPointer;
    int loopTimer;
    int resetTimer;
    int savedRounds;
    int16_t** charMeterPointer;
    int savedRoundsTemp;
    int outWidth;
    int outHeight;
    char tempOptionDifficultyCopy;
    int optionDifficultyCopyTemp;
    char directoryPath[260];
    char drivePath[260];
    char fullPathBuffer[260];
    char fileName[260];
    char extension[260];
    HBITMAP outBitmap;
    DWORD outColorCount;


    printf("%s", VersionString);
    customDirResult = CustomDirectoryProcessing();
    byte_6B2E65 = 1;
    byte_6B2F74 = customDirResult;
    _splitpath_s(FullPath, drivePath, sizeof(drivePath), directoryPath, sizeof(directoryPath), fileName, sizeof(fileName), extension, sizeof(extension));
    _makepath_s(fullPathBuffer, sizeof(fullPathBuffer), drivePath, directoryPath, "lights2", "ncd");
    CustomDataCopy(fullPathBuffer, ncd_array);
    dword_6B3004 = 0;
    byte_6B2BB8 = 0;
    byte_6B2BB9 = 0;
    g_globalTimerValue = CustomAudioControl();
    // ProcessJoysticks(hWndParent);
    // SetGlobalParamAndResetKeyStates(hWndParent);
    g_screenPosHorizontal = InitAnimationControl(gameWND, 256, 256, 8);
    ReallocateGlobalResourceArray(17);
    ReallocateRenderBuffer(600);
    ReallocateMemoryBuffer(32);
    if (!handleGlobalPalette(0))
        return 0;
    processedPixelCount = 0;
    initAndRunGame();
    title_bmp_data = 0;
    psel_bmp_data = 0;
    tokyo_bmp_data_pointer = 0;
    void* title_bmp_data = ProcessBitmapData("title.bmp", &outBitmap, &outColorCount, &outHeight, &outWidth);
    if (title_bmp_data) {
        ProcessBitmapData(title_bmp_data, &outBitmap, &outColorCount, &outHeight, &outWidth);
        uint8_t* psel_bmp_data = (uint8_t*)ProcessBitmapData("psel.bmp", &outBitmap, &outColorCount, &outHeight, &outWidth);
        if (psel_bmp_data) {
            ProcessBitmapData(psel_bmp_data, &outBitmap, &outColorCount, &outHeight, &outWidth);
            uint8_t* tokyo_bmp_data_pointer = ProcessBitmapData("tokyo.bmp", &outBitmap, &outColorCount, &outHeight, &outWidth);
            if (tokyo_bmp_data_pointer) {
                uint8_t* toktoBitmapDataPtr = (uint8_t*)ProcessBitmapData(tokyo_bmp_data_pointer, &outBitmap, &outColorCount, &outHeight, &outWidth);
                if (outHeight * outWidth > 0) {
                    int processedPixelCount = 0;
                    do {
                        *toktoBitmapDataPtr++ -= 76;
                        ++processedPixelCount;
                    } while (outHeight * outWidth > processedPixelCount);
                }
                uint8_t* grad_bmp_data = (uint8_t*)ProcessBitmapData("grad.bmp", &outBitmap, &outColorCount, &outHeight, &outWidth);
                if (grad_bmp_data) {
                    uint8_t* gradBitmapDataPtr = (uint8_t*)ProcessBitmapData(grad_bmp_data, &outBitmap, &outColorCount, &outHeight, &outWidth);
                    for (int pixelIndex = 0; outHeight * outWidth > pixelIndex; ++pixelIndex)
                        *gradBitmapDataPtr++ -= 76;
                    // LoadSfxFileBundle();
                    // showInputDisplay = 0;
                    gametype_enabled = 0;
                    HITSTOP_SLOWDOWN[0] = 0;
                    dword_6B2E08 = 0;
                    dword_688028 = 0;
                    dword_6B2FD4 = 0;
                    dword_6B2BB4 = 80;
                    gfx_spritetrail = 1;
                    gamespeed = 0;
                    option_difficulty_copy = 1;
                    option_difficulty = 1;
                    option_timer = 3600;
                    roundsToWin = 2;
                    // HandleGameOptionsRegistry();
                    // InitializeGameResources();
                    p2_char = 0;
                    gametype_enabled = 2;
                    character[0] = 0;
                    p1_setCPU[0] = 1;
                    p2_setCPU = 1;
                    while (1) {
                        switch (gametype_enabled) {
                        case 0:
                            gametype_enabled = 2;
                            continue;
                        case 1:
                            // nullsub_2();
                            continue;
                        case 2:
                            gameLoopResult = GameLoop();
                            // gameLoopResult = NULL;
                            if (gameLoopResult) {
                                if (gameLoopResult == 1) {
                                    gametype_enabled = 10;
                                }
                                else if (gameLoopResult == 2) {
                                    gametype_enabled = 13;
                                }
                            }
                            else {
                                gametype_enabled = 7;
                            }
                            continue;
                        case 3:
                            if (css_general() == -1) {
                                byte_6B2BB9 = 0;
                                gametype_enabled = 7;
                            }
                            else {
                                // nullsub_1(0, 0);
                                gametype_enabled = 5;
                            }
                            continue;
                        case 4:
                            // nullsub_3();
                            gametype_enabled = 0;
                            // MainEventLoop();
                            continue;
                        case 5:
                            resourceArrayPointer = hitcount;
                            do {
                                *resourceArrayPointer = 0;
                                resourceArrayPointer += 71;
                            } while (resourceArrayPointer < (int*)byte_6B2DFC);
                            // MainBattle();
                            gametype_enabled = 0;
                            if (!p1_setCPU[0] && !p2_setCPU || byte_6B2BB9 == 1) {
                                gametype_enabled = 5;
                                if (css_general() == -1) {
                                    byte_6B2BB9 = 0;
                                    gametype_enabled = 7;
                                }
                                else {
                                    // nullsub_1(0, 0);
                                }
                            }
                            continue;
                        case 6:
                            // mmManualPages();
                            gametype_enabled = 7;
                            continue;
                        case 7:
                            // audioReset();
                            switch (mmBgScroll()) {
                            case 0:
                                gametype_enabled = 3;
                                break;
                            case 1:
                                gametype_enabled = 9;
                                byte_6B2BB9 = 0;
                                break;
                            case 2:
                                gametype_enabled = 14;
                                byte_6B2BB9 = 0;
                                break;
                            case 4:
                                gametype_enabled = 12;
                                byte_6B2BB9 = 0;
                                break;
                            case 5:
                                p1_setCPU[0] = 0;
                                byte_6B2BB9 = 0;
                                gametype_enabled = 3;
                                p2_setCPU = 0;
                                break;
                            case 6:
                                gametype_enabled = 11;
                                byte_6B2BB9 = 0;
                                break;
                            case 7:
                                gametype_enabled = 8;
                                byte_6B2BB9 = 0;
                                break;
                            case 8:
                                gametype_enabled = 6;
                                break;
                            case 10:
                                gametype_enabled = 10;
                                break;
                            case 11:
                                gametype_enabled = 13;
                                byte_6B2BB9 = 0;
                                break;
                            default:
                                byte_6B2BB9 = 0;
                                gametype_enabled = 7;
                                break;
                            }
                            continue;
                        case 8:
                            p1_setCPU[0] = 0;
                            p2_setCPU = 0;
                            /*    if (css_general() == -1)
                            {
                                gametype_enabled = 7;
                            }
                            else {
                                // showInputDisplay = 1;
                                dword_6B2E08 = 1;
                                dword_688028 = 1;
                                dword_6B2FD4 = 1;
                            //    MainBattle();
                                gametype_enabled = 7;
                                // showInputDisplay = 0;
                                dword_6B2E08 = 0;
                                dword_688028 = 0;
                                dword_6B2FD4 = 0;
                            }*/
                            continue;
                        case 9:

                            if (css_general() == -1) {
                                gametype_enabled = 7;
                            }
                            else {
                                cpuSetPointer = (int16_t*)p1_health;
                                do {
                                    *cpuSetPointer = 96;
                                    cpuSetPointer += 142;
                                    *(cpuSetPointer - 141) = 32;
                                    *(cpuSetPointer - 140) = 96;
                                    *(cpuSetPointer - 148) = 0;
                                    *(cpuSetPointer - 149) = 0;
                                } while (cpuSetPointer < (int16_t*)(&dword_6B2E10 + 1));
                                savedOptionTimer = option_timer;
                                savedRoundsToWin = roundsToWin;
                                tempOptionDifficultyCopy = option_difficulty_copy;
                                option_difficulty_copy = 1;
                                option_difficulty = 1;
                                option_timer = 5940;
                                roundsToWin = 1;
                                *dword_6B2E60 = 0;
                                MainBattle();
                                option_timer = savedOptionTimer;
                                option_difficulty_copy = tempOptionDifficultyCopy;
                                roundsToWin = savedRoundsToWin;
                                option_difficulty = tempOptionDifficultyCopy;
                                gametype_enabled = 7;
                            }
                            continue;
                        case 10:
                            goto LABEL_76;
                        case 11:
                            if (css_general() == -1) {
                                gametype_enabled = 7;
                            }
                            else if (p1_setCPU[0] == 1 && p2_setCPU == 1) {
                                gametype_enabled = 7;
                            }
                            else {
                                gameModeResult = 0;
                                battleResult = 0;
                                cpuCharPointer = (int16_t*)p1_setCPU;
                                do {
                                    if (!*cpuCharPointer)
                                        gameModeResult = battleResult;
                                    cpuCharPointer += 142;
                                    ++battleResult;
                                } while ((uintptr_t)cpuCharPointer < (uintptr_t)spriteDataLimitArray + sizeof(int16_t));
                                    loopTimer = option_timer;
                                tempOptionDifficultyCopy = option_difficulty_copy;
                                dword_6B2E08 = 1;
                                option_difficulty_copy = 1;
                                optionDifficultyCopyTemp = roundsToWin;
                                dword_6B2E14 = 0;
                                option_difficulty = 1;
                                option_timer = 5940;
                                roundsToWin = 1;
                                MainBattle();
                                option_difficulty_copy = tempOptionDifficultyCopy;
                                option_timer = loopTimer;
                                roundsToWin = optionDifficultyCopyTemp;
                                dword_6B2E08 = 0;
                                option_difficulty = tempOptionDifficultyCopy;
                                gametype_enabled = 11;
                                p1_setCPU[142 * gameModeResult] = 0;
                            }
                            continue;
                        case 12:
                            p1_setCPU[0] = 0;
                            p2_setCPU = 0;
                            if (css_general() == -1) {
                                gametype_enabled = 7;
                            }
                            else {
                                p1_setCPU[0] = 1;
                                p2_setCPU = 1;
                                MainBattle();
                                gametype_enabled = 12;
                            }
                            continue;
                        case 13:
                            resetTimer = option_timer;
                            savedRounds = roundsToWin;
                            tempOptionDifficultyCopy = option_difficulty_copy;
                            option_difficulty_copy = 2;
                            option_difficulty = 2;
                            option_timer = 3600;
                            roundsToWin = 2;
                            p1_setCPU[0] = 1;
                            p2_setCPU = 1;
                            MainBattle();
                            option_timer = resetTimer;
                            option_difficulty_copy = tempOptionDifficultyCopy;
                            roundsToWin = savedRounds;
                            option_difficulty = tempOptionDifficultyCopy;
                            gametype_enabled = 7;
                            continue;
                        case 14:
                            gametype_enabled = 14;
                            if (mmCssTeamBattle() == -1) {
                                byte_6B2BB9 = 0;
                                gametype_enabled = 7;
                            }
                            else {
                                charMeterPointer = (int16_t**)p1_meter_Stocks;
                                do {
                                    *charMeterPointer = 0;
                                    charMeterPointer += 142;
                                    *(charMeterPointer - 143) = 0;
                                    *(charMeterPointer - 148) = 0;
                                } while ((uintptr_t)charMeterPointer < (uintptr_t)(&p2_dataLimit + 1));
                                    savedRoundsTemp = roundsToWin;
                                roundsToWin = 3;
                                sub_41F290();
                                roundsToWin = savedRoundsTemp;
                                if (p1_setCPU[0] == 1 || p2_setCPU == 1)
                                    gametype_enabled = 7;
                            }
                            continue;
                        default:
                            continue;
                        }
                    }
                }
            }
        }
    }
LABEL_76:
    if (tokyo_bmp_data_pointer)
        free(tokyo_bmp_data_pointer);
    if (title_bmp_data)
        free(title_bmp_data);
    if (psel_bmp_data)
        free(psel_bmp_data);
    return 0;
}


void initAndRunGame()
{
    BYTE* bitmapDataOffset;
    char* BitmapColorTableSize;
    unsigned int horizontalPadding;
    unsigned int verticalPadding;
    int frameCounter;
    void* logoBitmapData;
    int originalHeight;
    int outWidth;

    logoBitmapData = ProcessAndFindMatchingEntry("logo.bmp", 0, 0xFFFFF, 0);
    if (logoBitmapData)
    {
        bitmapDataOffset = (BYTE*)ProcessBitmapData(logoBitmapData, 0, 0, &outWidth, &originalHeight);
        int colorTableSize = GetBitmapColorTableSize(logoBitmapData, 0); // Proper casting
        BitmapColorTableSize = (char*)(uintptr_t)colorTableSize; // Proper casting to char*
        UpdatePaletteEntries(0, 99, BitmapColorTableSize, 1u);
        horizontalPadding = (256 - outWidth) >> 1;
        verticalPadding = (256 - originalHeight) >> 1;
        InitAnimations();
        frameCounter = 0;
        setupGameEnvironment();
        do
        {
            if (frameCounter > 10 && (HandleP1Inputs() || HandleP2Inputs()))
                break;
            ++frameCounter;
            updateRenderingData(
                0,
                (int)(uintptr_t)bitmapDataOffset, // Proper casting to int
                -1,
                0,
                horizontalPadding,
                verticalPadding,
                outWidth,
                originalHeight,
                1);
            mainGameLoop();
            clearGlobalAnimationControl();
        } while (frameCounter < 240);
        // g_UpdateObject();
        free(logoBitmapData);
    }
}