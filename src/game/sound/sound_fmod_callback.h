#pragma once

#include "fmod_common.h"

FMOD_RESULT F_CALLBACK FmodCallbackFileOpen(const char* file, unsigned int* pSize, void **pHandle, void *pUserData);
FMOD_RESULT F_CALLBACK FmodCallbackFileClose(void *handle, void *userData);
FMOD_RESULT F_CALLBACK FmodCallbackAsyncRead(FMOD_ASYNCREADINFO *info, void *userData);
FMOD_RESULT F_CALLBACK FmodCallbackAsyncReadCancel(FMOD_ASYNCREADINFO *handle, void *userData);
