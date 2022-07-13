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

#include <CoreLib/World/WorldFunctions.h>

// Define own pointer to the API
CCoreAPI *_pCoreAPI = NULL;

// Retrieve module information
MODULE_API void Module_GetInfo(CPluginAPI::PluginInfo *pInfo) {
  // Used API and utility
  pInfo->apiVer = CORE_API_VERSION;
  pInfo->ulFlags = CPluginAPI::PF_GAME | CPluginAPI::PF_EDITOR;

  // Metadata
  pInfo->strAuthor = "Dreamy Cecil";
  pInfo->strName = "Local Cheats";
  pInfo->strDescription = "Local client cheats such as noclip and weapon creation that can be used regardless of gamemode or mod. Not multiplayer-synchronized!";
  pInfo->ulVersion = CCoreAPI::MakeVersion(1, 0, 0);
};

// Container of local player entities
static CDynamicContainer<CPlayerEntity> _cenPlayers;

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

    CPrintF("%s^r - wall walking: %s\n", pen->GetName(), (bDisable ? "^cff0000OFF" : "^c00ff00ON"));
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

    CPrintF("%s^r - noclip: %s\n", pen->GetName(), (bDisable ? "^cff0000OFF" : "^c00ff00ON"));
  }
};

// Set health for all local players
static void SetHealth(INDEX iHealth) {
  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;

    pen->SetHealth(iHealth);
    CPrintF("Set %s^r health to %d\n", pen->GetName(), iHealth);
  }
};

// Create weapon entity
static void CreateWeapon(INDEX iWeapon) {
  FOREACHPLAYER(iten) {
    CPlayerEntity *pen = iten;

    // Create weapon item
    CEntity *penWeapon = IWorld::GetWorld()->CreateEntity_t(pen->GetPlacement(), CTFILENAME("Classes\\WeaponItem.ecl"));

    // CWeaponItem::m_EwitType
    CEntityProperty *pep = IWorld::FindProperty(penWeapon, (0x322 << 8) + 1, 0x3AC, CEntityProperty::EPT_ENUM);
    CTString strWeapon = "";

    // Property found
    if (pep != NULL) {
      strWeapon = pep->ep_pepetEnumType->NameForValue(iWeapon);
    }
    
    // Unknown weapon
    if (strWeapon == "") {
      strWeapon = "<unknown>";
      iWeapon = 1;
    }

    // Initialize weapon item
    ENTITYPROPERTY(penWeapon, pep->ep_slOffset, INDEX) = iWeapon;
    penWeapon->Initialize();

    CPrintF("%s^r created '%s' weapon item (%u)\n", pen->GetName(), strWeapon, penWeapon->en_ulID);
  }
};

// Module entry point
MODULE_API void Module_Startup(void) {
  // Hook pointer to the API
  HookSymbolAPI();

  // Custom symbols
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_WallWalking",  "void",  &WallWalking);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_Noclip",       "void",  &Noclip);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_SetHealth",    "INDEX", &SetHealth);
  GetPluginAPI()->RegisterMethod(TRUE, "void", "cht_CreateWeapon", "INDEX", &CreateWeapon);
};
