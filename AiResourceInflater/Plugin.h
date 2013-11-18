#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "MPQDraftPlugin.h"

class Plugin: public IMPQDraftPlugin {
  public:
    BOOL WINAPI Identify(LPDWORD lpdwPluginID);
    BOOL WINAPI GetPluginName(LPSTR lpszPluginName, DWORD nNameBufferLength);
    BOOL WINAPI CanPatchExecutable(LPCSTR lpszEXEFileName);
    BOOL WINAPI Configure(HWND hParentWnd);
    BOOL WINAPI ReadyForPatch();
    BOOL WINAPI GetModules(MPQDRAFTPLUGINMODULE *lpPluginModules, LPDWORD lpnNumModules);
    BOOL WINAPI InitializePlugin(IMPQDraftServer *lpMPQDraftServer);
    BOOL WINAPI TerminatePlugin();

    unsigned int AI_mineralMined;
    unsigned int AI_mineralGain;
    unsigned int AI_gasMined;
    unsigned int AI_gasGain;
    unsigned int AI_depletedGasGain;
};

extern Plugin plugin;