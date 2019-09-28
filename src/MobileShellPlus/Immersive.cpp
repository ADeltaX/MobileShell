#include "pch.h"
#include "Immersive.h"

//TODO, IMPORTANT FOR ACCESSING MORE WINDOW BANDS

#pragma section(".imrsiv", read, write, uninitalized)
#pragma data_seg(".imrsiv")
int MsftWatermark = 0;