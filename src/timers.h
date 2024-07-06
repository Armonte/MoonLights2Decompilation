

#ifndef TIMERS_H
#define TIMERS_H

#include <stdbool.h>

// Function declarations
int initializeTimerDivision(int initialValue, int divisionFactor);

// External variable declarations
extern int g_timerDivisionFactor;
extern int g_isTimerDivisionActive;
extern int g_timerInitialValue;
extern int g_timerCurrentValue;
extern int g_timerDividedValue;
extern int g_globalTimerValue;
#endif // TIMERS_H