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
#include <CoreLib/Interfaces/ConfigFunctions.h>

bool CPatch::_bDebugOutput = false;

// Define own pointer to the API
CCoreAPI *_pCoreAPI = NULL;

// Retrieve module information
MODULE_API void Module_GetInfo(CPluginAPI::PluginInfo *pInfo) {
  // Check if default entities are modified in a mod
  if (_fnmMod != "") {
    CTFileName fnmFull;
    ExpandFilePath(EFP_READ, CTString("Bin\\Entities") + _strModExt + ".dll", fnmFull);

    if (fnmFull.HasPrefix(_fnmApplicationPath + _fnmMod)) {
      // Load configuration file for the mod
      IConfig::CProperties aProps;
      BOOL bRefuse = TRUE;

      if (IConfig::ReadConfig(aProps, "Bin\\Plugins\\AdvancedHUD.ini")) {
        for (INDEX i = 0; i < aProps.Count(); i++) {
          const CTString &strKey = aProps[i].strKey;

          // Determine whether or not to use the same function hook
          if (strKey == "SameHook") {
            bRefuse = (aProps[i].strVal != "1");
            break;
          }
        }
      }

      // Refuse to load
      if (bRefuse) {
        pInfo->SetUtility(0);
        return;
      }
    }
  }

  // Utility flags
  pInfo->SetUtility(CPluginAPI::PF_GAME | CPluginAPI::PF_EDITOR);

  // Metadata
  pInfo->strAuthor = "Dreamy Cecil";
  pInfo->strName = "Advanced HUD";
  pInfo->strDescription = "Patches for the heads-up display with various improvements and expanded customization.";
  pInfo->ulVersion = CCoreAPI::MakeVersion(1, 1, 0);
};

CPluginSymbol _psEnable(SSF_PERSISTENT | SSF_USER, INDEX(1));

// Select default theme
#if SE1_GAME == SS_TFE
  CPluginSymbol _psTheme(SSF_PERSISTENT | SSF_USER, (INDEX)E_HUD_TFE);
#else
  CPluginSymbol _psTheme(SSF_PERSISTENT | SSF_USER, (INDEX)E_HUD_TSE);
#endif

CPluginSymbol _psScreenEdgeX(SSF_PERSISTENT | SSF_USER, INDEX(5));
CPluginSymbol _psScreenEdgeY(SSF_PERSISTENT | SSF_USER, INDEX(5));
CPluginSymbol _psIconShake(SSF_PERSISTENT | SSF_USER, INDEX(1));

#if SE1_GAME == SS_TFE
  // TFE specific
  CPluginSymbol _psShowClock(SSF_PERSISTENT | SSF_USER, INDEX(0));
  CPluginSymbol _psShowMatchInfo(SSF_PERSISTENT | SSF_USER, INDEX(1));

#else
  // TSE specific
  CPluginSymbol _psScopeAlpha(SSF_PERSISTENT | SSF_USER, FLOAT(1.0f));
  CPluginSymbol _psScopeColoring(SSF_PERSISTENT | SSF_USER, INDEX(0));
#endif

CPluginSymbol _psShowPlayerPing(SSF_PERSISTENT | SSF_USER, INDEX(0));
CPluginSymbol _psShowAmmoRow(SSF_PERSISTENT | SSF_USER, INDEX(1));
CPluginSymbol _psShowDepletedAmmo(SSF_PERSISTENT | SSF_USER, INDEX(1));
CPluginSymbol _psShowHighScore(SSF_PERSISTENT | SSF_USER, INDEX(1));

// HUD colorization (no alpha channel)
CPluginSymbol _psColorize(SSF_PERSISTENT | SSF_USER, INDEX(0));
static CPluginSymbol _psColorPreset(SSF_PERSISTENT | SSF_USER, INDEX(0));

CPluginSymbol _psColorBase(SSF_PERSISTENT | SSF_USER, INDEX(0x00FF00));
CPluginSymbol _psColorIcon(SSF_PERSISTENT | SSF_USER, INDEX(0x00FF00));
CPluginSymbol _psColorWeapon(SSF_PERSISTENT | SSF_USER, INDEX(0x00FF00));
CPluginSymbol _psColorSelect(SSF_PERSISTENT | SSF_USER, INDEX(0xFFFFFF));
CPluginSymbol _psColorMax(SSF_PERSISTENT | SSF_USER, INDEX(0x00FF00));
CPluginSymbol _psColorTop(SSF_PERSISTENT | SSF_USER, INDEX(0x00FF00));
CPluginSymbol _psColorMid(SSF_PERSISTENT | SSF_USER, INDEX(0x00FF00));
CPluginSymbol _psColorLow(SSF_PERSISTENT | SSF_USER, INDEX(0xFF0000));

// Module entry point
MODULE_API void Module_Startup(void) {
  // Hook pointer to the API
  HookSymbolAPI();

  // Custom symbols
  _psEnable.Register("ahud_bEnable");
  _psTheme.Register("ahud_iTheme");

  _psScreenEdgeX.Register("ahud_iScreenEdgeX");
  _psScreenEdgeY.Register("ahud_iScreenEdgeY");
  _psIconShake.Register("ahud_bIconShake");

  #if SE1_GAME == SS_TFE
    // TFE specific
    _psShowClock.Register("hud_bShowClock");
    _psShowMatchInfo.Register("hud_bShowMatchInfo");

  #else
    // TSE specific
    _psScopeAlpha.Register("ahud_fScopeAlpha");
    _psScopeColoring.Register("ahud_bScopeColoring");
  #endif

  _psShowPlayerPing.Register("ahud_iShowPlayerPing");
  _psShowAmmoRow.Register("ahud_bShowAmmoRow");
  _psShowDepletedAmmo.Register("ahud_bShowDepletedAmmo");
  _psShowHighScore.Register("ahud_bShowHighScore");

  _psColorize.Register("ahud_bColorize");
  _psColorPreset.Register("ahud_iColorPreset");

  _psColorBase.Register("ahud_iColorBase");
  _psColorIcon.Register("ahud_iColorIcon");
  _psColorWeapon.Register("ahud_iColorWeapon");
  _psColorSelect.Register("ahud_iColorSelect");
  _psColorMax.Register("ahud_iColorMax");
  _psColorTop.Register("ahud_iColorTop");
  _psColorMid.Register("ahud_iColorMid");
  _psColorLow.Register("ahud_iColorLow");

  // Initialize the HUD itself
  _HUD.Initialize();
};

// Module cleanup
MODULE_API void Module_Shutdown(void)
{
};
