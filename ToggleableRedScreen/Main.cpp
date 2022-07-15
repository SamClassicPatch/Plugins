/* Copyright (c) 2022 Dreamy Cecil
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

#include <CoreLib/Interfaces/WorldFunctions.h>

// Patcher
#include <CoreLib/FuncPatching.h>

bool CPatch::_bDebugOutput = false;

// Define own pointer to the API
CCoreAPI *_pCoreAPI = NULL;

// Retrieve module information
MODULE_API void Module_GetInfo(CPluginAPI::PluginInfo *pInfo) {
  // Used API and utility
  pInfo->apiVer = CORE_API_VERSION;
  pInfo->ulFlags = CPluginAPI::PF_GAME | CPluginAPI::PF_EDITOR;

  // Metadata
  pInfo->strAuthor = "Dreamy Cecil";
  pInfo->strName = "Toggleable Red Screen";
  pInfo->strDescription = "A new console command that allows toggling of red screen on damage.";
  pInfo->ulVersion = CCoreAPI::MakeVersion(1, 0, 0);
};

// Toggle red screen on damage
static CPluginSymbol _psRedScreen(SSF_PERSISTENT | SSF_USER, INDEX(1));

// Original function pointer
static void (CDrawPort::*pBlendScreen)(void) = NULL;

// Patched function
class CDrawPortPatch : public CDrawPort {
  public:
    void P_BlendScreen(void)
    {
      if (!_psRedScreen.GetIndex()) {
        BOOL bReset = FALSE;

        // Reset blending altogether if no world glaring available
        if (IWorld::GetWSC(IWorld::GetWorld()) == NULL) {
          bReset = TRUE;

        // Reset red screen blending
        } else if (dp_ulBlendingRA >  0 && dp_ulBlendingA  >  0
                && dp_ulBlendingGA == 0 && dp_ulBlendingBA == 0) {
          bReset = TRUE;
        }

        if (bReset) {
          dp_ulBlendingRA = 0;
          dp_ulBlendingGA = 0;
          dp_ulBlendingBA = 0;
          dp_ulBlendingA  = 0;
        }
      }

      // Proceed to the original function
      (this->*pBlendScreen)();
    };
};

// Module entry point
MODULE_API void Module_Startup(void) {
  // Hook pointer to the API
  HookSymbolAPI();

  // Patch screen blending function
  pBlendScreen = &CDrawPort::BlendScreen;
  NewPatch(pBlendScreen, &CDrawPortPatch::P_BlendScreen, "CDrawPort::BlendScreen()");

  // Custom symbols
  _psRedScreen.Register("sam_bRedScreenOnDamage", "", "");
};
