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

void IGameEvents::OnGameStart(void)
{
  // Affect entities at the beginning of the game
  FOREACHINDYNAMICCONTAINER(IWorld::GetWorld()->wo_cenEntities, CEntity, iten) {
    CEntity *pen = iten;

    if (IsOfClass(pen, "Weapon Item")) {
      AffectWeaponItem(pen);

    } else if (IsOfClass(pen, "Health Item")) {
      AffectHealthItem(pen);

    } else if (IsOfClass(pen, "Armor Item")) {
      AffectArmorItem(pen);

    } else if (IsOfClass(pen, "PowerUp Item")) {
      AffectPowerUpItem(pen);

    } else if (IsOfClass(pen, "Player Marker")) {
      AffectPlayerMarker(pen);

    } else if (IsOfClass(pen, "Enemy Spawner")) {
      AffectEnemySpawner(pen);
    }
  }

  // Execute all scheduled commands
  CStaticStackArray<CTString> &astrCommands = IServerSandbox::astrScheduled;

  if (astrCommands.Count() > 0) {
    CPrintF(TRANS("Executing scheduled sandbox commands...\n"));

    CTString strExecute = "";

    for (INDEX iCommand = 0; iCommand < astrCommands.Count(); iCommand++) {
      strExecute += astrCommands[iCommand];
    }

    _pShell->Execute(strExecute);
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
