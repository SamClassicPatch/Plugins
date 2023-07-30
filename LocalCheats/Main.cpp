/* Copyright (c) 2022-2023 Dreamy Cecil
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

#include <CoreLib/Compatibility/VanillaEvents.h>

// Define own pointer to the API
CCoreAPI *_pCoreAPI = NULL;

static IProcessingEvents _evProcessing;

// Retrieve module information
MODULE_API void Module_GetInfo(CPluginInfo &info) {
  // Utility flags
  info.SetUtility(PLF_GAMEPLAY_LOGIC);

  // Metadata
  info.strAuthor = "Dreamy Cecil";
  info.strName = "Local Cheats";
  info.strDescription = "Local client cheats such as noclip and weapon creation that can be used regardless of gamemode or mod. Not multiplayer synchronized!";
  info.ulVersion = CORE_PATCH_VERSION;
};

CPluginSymbol _psAutoKill(SSF_USER, INDEX(0));
CPluginSymbol _psAutoKillRange(SSF_PERSISTENT | SSF_USER, 256.0f);

// Container of local player entities
CDynamicContainer<CPlayerEntity> _cenPlayers;

// Iterate through each local player
#define FOREACHPLAYER(_PlayerIter) \
  _cenPlayers.Clear(); \
  IWorld::GetLocalPlayers(_cenPlayers); \
  FOREACHINDYNAMICCONTAINER(_cenPlayers, CPlayerEntity, _PlayerIter)

// Walk on any solid polygon
static void WallWalking(void) {
  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;

    BOOL bDisable = pen->GetPhysicsFlags() & EPF_STICKYFEET;

    if (bDisable) {
      pen->SetPhysicsFlags(pen->GetPhysicsFlags() & ~EPF_STICKYFEET);
    } else {
      pen->SetPhysicsFlags(pen->GetPhysicsFlags() | EPF_STICKYFEET);
    }

    CPrintF(TRANS("%s^r - wall walking: %s\n"), pen->GetName(), (bDisable ? "^cff0000OFF" : "^c00ff00ON"));
  }
};

// Fly around the map
static void Noclip(void) {
  const ULONG ulWorldCollision = (((1UL << 0) | (1UL << 1)) << ECB_TEST);
  const ULONG ulPhysFlags = (EPF_TRANSLATEDBYGRAVITY | EPF_ORIENTEDBYGRAVITY);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;

    BOOL bDisable = !(pen->GetCollisionFlags() & ulWorldCollision);

    if (bDisable) {
      pen->SetCollisionFlags(pen->GetCollisionFlags() | ulWorldCollision);
      pen->SetPhysicsFlags(pen->GetPhysicsFlags() | ulPhysFlags);

    } else {
      pen->SetCollisionFlags(pen->GetCollisionFlags() & ~ulWorldCollision);
      pen->SetPhysicsFlags(pen->GetPhysicsFlags() & ~ulPhysFlags);

      pen->en_plViewpoint.pl_OrientationAngle = ANGLE3D(0.0f, 0.0f, 0.0f);
    }

    CPrintF(TRANS("%s^r - noclip: %s\n"), pen->GetName(), (bDisable ? "^cff0000OFF" : "^c00ff00ON"));
  }
};

// Set health for all local players
static void SetHealth(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  INDEX iHealth = NEXT_ARG(INDEX);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;

    pen->SetHealth(iHealth);
    CPrintF(TRANS("Set %s^r health to %d\n"), pen->GetName(), iHealth);
  }
};

// Trigger an entity at the crosshair position
static void Trigger(void) {
  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;
    CPlacement3D plView = IWorld::GetViewpoint(pen, FALSE);

    CCastRay crRay(pen, plView);
    crRay.cr_bHitTranslucentPortals = FALSE;
    crRay.cr_bPhysical = FALSE;
    crRay.cr_ttHitModels = CCastRay::TT_COLLISIONBOX;

    IWorld::GetWorld()->CastRay(crRay);

    if (crRay.cr_penHit != NULL) {
      VNL_ETrigger eTrigger;
      eTrigger.penCaused = pen;

      crRay.cr_penHit->SendEvent(eTrigger);
    }
  }
};

// Create specific item of a specific type
static void CreateItem(CEntity *penPlayer, const CTString &strClass,
                       const CTString &strPropName, INDEX iPropID, INDEX iType) {
  // Create weapon item
  CEntity *penWeapon = IWorld::GetWorld()->CreateEntity_t(penPlayer->GetPlacement(), strClass);

  // Retrieve type property
  CEntityProperty *pep = IWorld::PropertyForNameOrId(penWeapon, CEntityProperty::EPT_ENUM, strPropName, iPropID);
  CTString strItem = "";

  // Property found
  if (pep != NULL) {
    strItem = pep->ep_pepetEnumType->NameForValue(iType);
  }
    
  // Unknown item type
  if (strItem == "") {
    strItem = TRANS("<unknown>");
    iType = 1; // Default valid type
  }

  // Initialize the item
  ENTITYPROPERTY(penWeapon, pep->ep_slOffset, INDEX) = iType;
  penWeapon->Initialize();

  CPrintF(TRANS("%s^r created '%s' item (%u)\n"), penPlayer->GetName(), strItem, penWeapon->en_ulID);
};

// Create weapon entity
static void CreateWeapon(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  INDEX iType = NEXT_ARG(INDEX);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;
    CreateItem(pen, "Classes\\WeaponItem.ecl", "Type", (0x322 << 8) + 1, iType);
  }
};

// Create ammo entity
static void CreateAmmo(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  INDEX iType = NEXT_ARG(INDEX);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;
    CreateItem(pen, "Classes\\AmmoItem.ecl", "Type", (0x323 << 8) + 1, iType);
  }
};

// Create health entity
static void CreateHealth(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  INDEX iType = NEXT_ARG(INDEX);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;
    CreateItem(pen, "Classes\\HealthItem.ecl", "Type", (0x321 << 8) + 1, iType);
  }
};

// Create armor entity
static void CreateArmor(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  INDEX iType = NEXT_ARG(INDEX);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;
    CreateItem(pen, "Classes\\ArmorItem.ecl", "Type", (0x324 << 8) + 1, iType);
  }
};

// Create powerup entity
static void CreatePowerUp(SHELL_FUNC_ARGS) {
  BEGIN_SHELL_FUNC;
  INDEX iType = NEXT_ARG(INDEX);

  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;
    CreateItem(pen, "Classes\\PowerUpItem.ecl", "Type", (0x328 << 8) + 1, iType);
  }
};

// Module entry point
MODULE_API void Module_Startup(void) {
  // Hook pointer to the API
  HookSymbolAPI();

  _evProcessing.Register();

  // Custom symbols
  _psAutoKill.Register("cht_iAutoKill");
  _psAutoKillRange.Register("cht_fAutoKillRange");

  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_WallWalking",  "void",  &WallWalking);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_Noclip",       "void",  &Noclip);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_SetHealth",    "INDEX", &SetHealth);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_Trigger",      "void",  &Trigger);

  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_CreateWeapon",  "INDEX", &CreateWeapon);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_CreateAmmo",    "INDEX", &CreateAmmo);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_CreateHealth",  "INDEX", &CreateHealth);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_CreateArmor",   "INDEX", &CreateArmor);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_CreatePowerUp", "INDEX", &CreatePowerUp);
};

// Module cleanup
MODULE_API void Module_Shutdown(void) {
  _evProcessing.Unregister();
};
