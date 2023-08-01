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

#include "StartActions.h"
#include "Sandbox.h"

#include <EntitiesV/StdH/StdH.h>
#include <EntitiesV/AmmoItem.h>
#include <EntitiesV/ArmorItem.h>
#include <EntitiesV/HealthItem.h>
#include <EntitiesV/PowerUpItem.h>
#include <EntitiesV/WeaponItem.h>
#include <EntitiesV/EnemySpawner.h>
#include <EntitiesV/PlayerMarker.h>

void IGameEvents::OnGameStart(void)
{
  // Affect entities at the beginning of the game
  FOREACHINDYNAMICCONTAINER(IWorld::GetWorld()->wo_cenEntities, CEntity, iten) {
    CEntity *pen = iten;

    if (IsOfClassID(pen, CWeaponItem_ClassID)) {
      AffectWeaponItem(pen);

    } else if (IsOfClassID(pen, CAmmoItem_ClassID)) {
      AffectAmmoItem(pen);

    } else if (IsOfClassID(pen, CHealthItem_ClassID)) {
      AffectHealthItem(pen);

    } else if (IsOfClassID(pen, CArmorItem_ClassID)) {
      AffectArmorItem(pen);

    } else if (IsOfClassID(pen, CPowerUpItem_ClassID)) {
      AffectPowerUpItem(pen);

    } else if (IsOfClassID(pen, CPlayerMarker_ClassID)) {
      AffectPlayerMarker(pen);

    } else if (IsOfClassID(pen, CEnemySpawner_ClassID)) {
      AffectEnemySpawner(pen);
    }
  }

  // Execute all scheduled commands
  CStringStack &astrCommands = IServerSandbox::astrScheduled;

  if (astrCommands.Count() > 0) {
    CPrintF(TRANS("Executing scheduled sandbox commands...\n"));

    CTString strExecute = "";

    for (INDEX iCommand = 0; iCommand < astrCommands.Count(); iCommand++) {
      strExecute += astrCommands[iCommand];
    }

    _pShell->Execute(strExecute);
  }
};

void IGameEvents::OnChangeLevel(void)
{
  // Affect entities on a new level
  if (_pVarData->gex.bGameplayExt)
  {
    // [Cecil] TODO: It should be implemented as extension packets, since it's a server plugin
    //OnGameStart();
  }
};

void IGameEvents::OnGameStop(void)
{
};

void IGameEvents::OnGameSave(const CTFileName &fnmSave)
{
};

void IGameEvents::OnGameLoad(const CTFileName &fnmSave)
{
};
