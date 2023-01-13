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

void IProcessingEvents::OnStep(void)
{
  _cenPlayers.Clear();
  IWorld::GetLocalPlayers(_cenPlayers);

  // No players
  if (_cenPlayers.Count() == 0) return;
  
  // Take first local player as the killer
  CEntity *penKiller = _cenPlayers.Pointer(0);

  // Kill enemies each tick
  INDEX iAutoKill = _psAutoKill.GetIndex();

  if (iAutoKill > 0) {
    FOREACHINDYNAMICCONTAINER(IWorld::GetWorld()->wo_cenEntities, CEntity, iten) {
      CEntity *pen = iten;

      // Not an enemy
      if (!IsDerivedFromClass(pen, "Enemy Base")) {
        continue;
      }

      // Too far from the killer
      if (_psAutoKillRange.GetFloat() > 0.0f) {
        FLOAT3D vDiff = (pen->GetPlacement().pl_PositionVector - penKiller->GetPlacement().pl_PositionVector);

        if (vDiff.Length() > _psAutoKillRange.GetFloat()) {
          continue;
        }
      }

      // CEnemyBase::m_bTemplate
      CEntityProperty *pep = IWorld::PropertyForNameOrId(pen, CEntityProperty::EPT_BOOL, "Template", (0x136 << 8) + 86);

      // Skip template enemies
      if (pep != NULL) {
        if (ENTITYPROPERTY(pen, pep->ep_slOffset, BOOL)) {
          continue;
        }
      }

      BOOL bKillEnemy = TRUE;

      // Kill enemies that target someone
      if (iAutoKill > 1) {
        // CEnemyBase::m_penEnemy
        pep = IWorld::PropertyForIdOrOffset(pen, CEntityProperty::EPT_ENTITYPTR, (0x136 << 8) + 3, 0x30C);

        // No property
        if (pep == NULL) {
          continue;
        }

        CEntity *penEnemy = ENTITYPROPERTY(pen, pep->ep_slOffset, CEntityPointer);

        // No active enemy
        if (penEnemy == NULL) {
          bKillEnemy = FALSE;
        }
      }

      // Kill the enemy if permitted
      if (bKillEnemy) {
        CLiveEntity *penAlive = (CLiveEntity *)pen;

        // Inflict at least 5 points of damage each tick
        penKiller->InflictDirectDamage(pen, penKiller, (DamageType)1000, ClampDn(penAlive->GetHealth(), 5.0f),
                                       pen->GetPlacement().pl_PositionVector, FLOAT3D(0.0f, 0.0f, 0.0f));
      }
    }
  }
};

void IProcessingEvents::OnFrame(CDrawPort *pdp)
{
};
