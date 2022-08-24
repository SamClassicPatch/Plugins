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

#include "Sandbox.h"

// Define own pointer to the API
CCoreAPI *_pCoreAPI = NULL;

// Plugin event handlers
static INetworkEvents _evNetwork;

// Retrieve module information
MODULE_API void Module_GetInfo(CPluginAPI::PluginInfo *pInfo) {
  // Used API and utility
  pInfo->apiVer = CORE_API_VERSION;
  pInfo->ulFlags = CPluginAPI::PF_GAME | CPluginAPI::PF_SERVER;

  // Metadata
  pInfo->strAuthor = "Dreamy Cecil";
  pInfo->strName = "Server Utilities";
  pInfo->strDescription = "A collection of commands for enhanced world manipulation for hosting custom game servers.";
  pInfo->ulVersion = CCoreAPI::MakeVersion(1, 0, 0);
};

// Mode switch symbol (-100 = ignore)
#define MSS CPluginSymbol(0, INDEX(-100))

// Difficulty settings
CPluginSymbol _psStartHP(SSF_PERSISTENT | SSF_USER, 100.0f);
CPluginSymbol _psStartAR(SSF_PERSISTENT | SSF_USER, 0.0f);
CPluginSymbol _psEnemyMul(SSF_PERSISTENT | SSF_USER, 1.0f);

// Weapon settings
CPluginSymbol _psMaxAmmo(SSF_PERSISTENT | SSF_USER, INDEX(FALSE));

// 0 = take away; 1 = give
CPluginSymbol _apsGiveWeapons[CT_WEAPONS] = {
  MSS, MSS, MSS, MSS, MSS, MSS, MSS, // Knife, Colt, DColt, SShotgun, DShotgun, Tommygun, Minigun
  MSS, MSS, MSS, MSS, MSS, MSS, MSS, // RLauncher, GLauncher, Chainsaw, Flamer, Laser, Sniper, Cannon
  MSS, MSS, MSS, MSS, MSS, MSS, MSS,
  MSS, MSS, MSS, MSS, MSS, MSS, MSS,
  MSS, MSS, MSS,
};

// 0+ = specific type
CPluginSymbol _apsWeaponItems[CT_WEAPONS] = {
  MSS, MSS, MSS, MSS, MSS, MSS, MSS, // <none>, Colt, SShotgun, DShotgun, Tommygun, Minigun, RLauncher
  MSS, MSS, MSS, MSS, MSS, MSS, MSS, // GLauncher, Sniper, Flamer, Laser, Chainsaw, Cannon, Ghostbuster
  MSS, MSS, MSS, MSS, MSS, MSS, MSS,
  MSS, MSS, MSS, MSS, MSS, MSS, MSS,
  MSS, MSS, MSS,
};

// 0+ = specific type
CPluginSymbol _apsPowerUpItems[CT_POWERUPS] = {
  MSS, MSS, MSS, MSS, MSS, // Invisibility, Invulnerability, Serious Damage, Serious Speed, Serious Bomb
  MSS, MSS, MSS, MSS, MSS,
};

// -1 = leave as is; 0+ = item index
CPluginSymbol _psReplaceWeapons(SSF_PERSISTENT | SSF_USER, INDEX(-1));
CPluginSymbol _psReplaceHealth (SSF_PERSISTENT | SSF_USER, INDEX(-1));
CPluginSymbol _psReplaceArmor  (SSF_PERSISTENT | SSF_USER, INDEX(-1));

// Module entry point
MODULE_API void Module_Startup(void) {
  // Hook pointer to the API
  HookSymbolAPI();

  // Register plugin events
  _evNetwork.Register();

  // Custom symbols
  {
    // Difficulty settings
    _psStartHP.Register("sutl_fStartHealth");
    _psStartAR.Register("sutl_fStartArmor");
    _psEnemyMul.Register("sutl_fEnemyMultiplier");

    // Weapon settings
    _psMaxAmmo.Register("sutl_bMaxAmmo");

    CTString strCommand;

    for (INDEX iWeapon = 0; iWeapon < CT_WEAPONS; iWeapon++)
    {
      strCommand.PrintF("sutl_iGiveWeapon%d", iWeapon);
      _apsGiveWeapons[iWeapon].Register(strCommand);

      strCommand.PrintF("sutl_iWeaponType%d", iWeapon);
      _apsWeaponItems[iWeapon].Register(strCommand);
    }

    // Item settings
    for (INDEX iPowerup = 0; iPowerup < CT_POWERUPS; iPowerup++)
    {
      strCommand.PrintF("sutl_iPowerUpType%d", iPowerup);
      _apsPowerUpItems[iPowerup].Register(strCommand);
    }

    _psReplaceWeapons.Register("sutl_iReplaceWeapons");
    _psReplaceHealth.Register("sutl_iReplaceHealth");
    _psReplaceArmor.Register("sutl_iReplaceArmor");

    // Server sandbox commands
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_ListScheduledCommands",  "void", &IServerSandbox::ListScheduledCommands);
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_ClearScheduledCommands", "void", &IServerSandbox::ClearScheduledCommands);

    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_DeleteEntity",      "INDEX",                      &IServerSandbox::DeleteEntity);
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_InitEntity",        "INDEX",                      &IServerSandbox::InitEntity);
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_SetEntityPosition", "INDEX, FLOAT, FLOAT, FLOAT", &IServerSandbox::SetEntityPosition);
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_SetEntityRotation", "INDEX, FLOAT, FLOAT, FLOAT", &IServerSandbox::SetEntityRotation);
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_SetEntityProperty", "INDEX, CTString, CTString",  &IServerSandbox::SetEntityProperty);
    GetPluginAPI()->RegisterMethod(TRUE, "void", "sutl_ParentEntity",      "INDEX, INDEX",               &IServerSandbox::ParentEntity);
  }
};
