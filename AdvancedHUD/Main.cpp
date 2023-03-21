/* Copyright (c) 2023 Dreamy Cecil
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"

#include "HUD.h"

bool CPatch::_bDebugOutput = false;

// Define own pointer to the API
CCoreAPI *_pCoreAPI = NULL;

// Retrieve module information
MODULE_API void Module_GetInfo(CPluginAPI::PluginInfo *pInfo) {
  // Refuse to load for anything if running under a mod with custom logic
  if (_fnmMod != "") {
    CTFileName fnmFull;
    ExpandFilePath(EFP_READ, CTString("Bin\\Entities") + _strModExt + ".dll", fnmFull);

    if (fnmFull.HasPrefix(_fnmApplicationPath + _fnmMod)) {
      pInfo->SetUtility(0);
      return;
    }
  }

  // Utility flags
  pInfo->SetUtility(CPluginAPI::PF_GAME | CPluginAPI::PF_EDITOR);

  // Metadata
  pInfo->strAuthor = "Dreamy Cecil";
  pInfo->strName = "Advanced HUD";
  pInfo->strDescription = "Patches for the heads-up display with various improvements and expanded customization.";
  pInfo->ulVersion = CCoreAPI::MakeVersion(1, 0, 0);
};

CPluginSymbol _psEnable(SSF_PERSISTENT | SSF_USER, INDEX(1));

// Select default theme
#if SE1_GAME == SS_TFE
  CPluginSymbol _psTheme(SSF_PERSISTENT | SSF_USER, INDEX(0));
#else
  CPluginSymbol _psTheme(SSF_PERSISTENT | SSF_USER, INDEX(2));
#endif

CPluginSymbol _psScreenEdgeX(SSF_PERSISTENT | SSF_USER, INDEX(5));
CPluginSymbol _psScreenEdgeY(SSF_PERSISTENT | SSF_USER, INDEX(5));

#if SE1_GAME == SS_TFE
  // TFE specific
  CPluginSymbol _psShowMatchInfo(SSF_PERSISTENT | SSF_USER, INDEX(1));

#else
  // TSE specific
  CPluginSymbol _psScopeAlpha(SSF_PERSISTENT | SSF_USER, FLOAT(1.0f));
#endif

// Module entry point
MODULE_API void Module_Startup(void) {
  // Hook pointer to the API
  HookSymbolAPI();

  // Custom symbols
  _psEnable.Register("ahud_bEnable");
  _psTheme.Register("ahud_iTheme");

  _psScreenEdgeX.Register("ahud_iScreenEdgeX");
  _psScreenEdgeY.Register("ahud_iScreenEdgeY");

  #if SE1_GAME == SS_TFE
    // TFE specific
    _psShowMatchInfo.Register("hud_bShowMatchInfo");

  #else
    // TSE specific
    _psScopeAlpha.Register("ahud_fScopeAlpha");
  #endif

  // Initialize the HUD itself
  _HUD.Initialize();
};

// Module cleanup
MODULE_API void Module_Shutdown(void)
{
};
