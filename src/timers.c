#include "timers.h"



int g_timerDivisionFactor = 0;
int g_isTimerDivisionActive = 0;
int g_timerInitialValue = 0;
int g_timerCurrentValue = 0;
int g_timerDividedValue = 0;
int globalTimerValue = 0; // Assuming this is defined and managed elsewhere

// Function implementation
int __cdecl initializeTimerDivision(int initial_value, int division_factor)
{
    if (division_factor < 2)
        return -1;

    g_timerDivisionFactor = division_factor;
    g_isTimerDivisionActive = 1;
    g_timerInitialValue = initial_value;
    g_timerCurrentValue = globalTimerValue;
    g_timerDividedValue = globalTimerValue / division_factor;

    return 0;
}