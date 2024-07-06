#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

extern uint8_t* p2_dataLimit;
extern char FullPath[260];
extern int g_globalTimerValue;
extern char VersionString[260];
extern uint8_t byte_6B2E65;
extern uint8_t byte_6B2F74;
extern uint8_t* title_bmp_data;
extern uint8_t* psel_bmp_data;
extern uint8_t* tokyo_bmp_data_pointer;
extern uint8_t* grad_bmp_data;
extern int dword_6B3004;
extern uint8_t byte_6B2BB8;
extern uint8_t byte_6B2BB9;
extern int HITSTOP_SLOWDOWN[10]; // Assuming the size based on common usage
extern int dword_6B2E08;
extern int dword_688028;
extern int dword_6B2FD4;
extern int dword_6B2BB4;
extern int gfx_spritetrail;
extern int gamespeed;
extern int option_difficulty_copy;
extern int option_difficulty;
extern int option_timer;
extern int roundsToWin;
extern int p2_char;
extern int gametype_enabled;
extern int character[10]; // Assuming the size based on common usage
extern int p1_setCPU[10]; // Assuming the size based on common usage
extern int p2_setCPU;
extern int hitcount[10]; // Assuming the size based on common usage
extern uint8_t byte_6B2DFC;
extern int* p1_health;
extern int dword_6B2E10;
extern int* dword_6B2E60;
extern int spriteDataLimitArray;
extern int dword_6B2E14;
extern int* p1_meter_Stocks;


void initAndRunGame(void);
int initGame();


