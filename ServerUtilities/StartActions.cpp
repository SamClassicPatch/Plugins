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

#include <CoreLib/Objects/PropertyPtr.h>

// Notify about not being able to retrieve some entity property
static void ReportPropError(CEntity *pen, const char *strPropertyName) {
  CPrintF(TRANS("'%s' (%u) : Cannot retrieve '%s' property!\n"), pen->GetName(), pen->en_ulID, strPropertyName);
};

// Affect weapon item at the beginning of the game
void AffectWeaponItem(CEntity *pen) {
  // Retrieve CWeaponItem::m_EwitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByNameOrId(CEntityProperty::EPT_ENUM, "Type", (0x322 << 8) + 1)) {
    ReportPropError(pen, "CWeaponItem::m_EwitType");
    return;
  }

  INDEX &iWeaponType = ENTITYPROPERTY(pen, pptr.Offset(), INDEX);
  INDEX iSetType = _apsWeaponItems[iWeaponType].GetIndex();

  // Remove weapon item
  if (iSetType == -1) {
    pen->Destroy();

  // Set specific type
  } else if (iSetType >= 0) {
    iWeaponType = iSetType;
    pen->Reinitialize();

  // Replace with another weapon
  } else if (_psReplaceWeapons.GetIndex() >= 0) {
    iWeaponType = _psReplaceWeapons.GetIndex();
    pen->Reinitialize();
  }
};

// Affect health item at the beginning of the game
void AffectHealthItem(CEntity *pen) {
  // Retrieve CHealthItem::m_EhitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByNameOrId(CEntityProperty::EPT_ENUM, "Type", (0x321 << 8) + 1)) {
    ReportPropError(pen, "CHealthItem::m_EhitType");
    return;
  }

  INDEX &iHealthType = ENTITYPROPERTY(pen, pptr.Offset(), INDEX);
  INDEX iSetType = _apsHealthItems[iHealthType].GetIndex();

  // Remove health item
  if (iSetType == -1) {
    pen->Destroy();

  // Set specific type
  } else if (iSetType >= 0) {
    iHealthType = iSetType;
    pen->Reinitialize();

  // Replace with another health item
  } else if (_psReplaceHealth.GetIndex() >= 0) {
    iHealthType = _psReplaceHealth.GetIndex();
    pen->Reinitialize();
  }
};

// Affect armor item at the beginning of the game
void AffectArmorItem(CEntity *pen) {
  // Retrieve CArmorItem::m_EaitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByNameOrId(CEntityProperty::EPT_ENUM, "Type", (0x324 << 8) + 1)) {
    ReportPropError(pen, "CArmorItem::m_EaitType");
    return;
  }

  INDEX &iArmorType = ENTITYPROPERTY(pen, pptr.Offset(), INDEX);
  INDEX iSetType = _apsArmorItems[iArmorType].GetIndex();

  // Remove armor item
  if (iSetType == -1) {
    pen->Destroy();

  // Set specific type
  } else if (iSetType >= 0) {
    iArmorType = iSetType;
    pen->Reinitialize();

  // Replace with another armor item
  } else if (_psReplaceArmor.GetIndex() >= 0) {
    iArmorType = _psReplaceArmor.GetIndex();
    pen->Reinitialize();
  }
};

// Affect power up item at the beginning of the game
void AffectPowerUpItem(CEntity *pen) {
  // Retrieve CPowerUpItem::m_puitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByNameOrId(CEntityProperty::EPT_ENUM, "Type", (0x328 << 8) + 1)) {
    ReportPropError(pen, "CPowerUpItem::m_puitType");
    return;
  }

  INDEX &iPowerUp = ENTITYPROPERTY(pen, pptr.Offset(), INDEX);
  INDEX iSetType = _apsPowerUpItems[iPowerUp].GetIndex();

  // Remove power up
  if (iSetType == -1) {
    pen->Destroy();

  // Set specific type
  } else if (iSetType >= 0) {
    iPowerUp = iSetType;
    pen->Reinitialize();
  }
};

// Affect player start marker at the beginning of the game
void AffectPlayerMarker(CEntity *pen) {
  // Retrieve CPlayerMarker::m_iGiveWeapons
  static CPropertyPtr pptrGive(pen);

  if (!pptrGive.ByNameOrId(CEntityProperty::EPT_INDEX, "Give Weapons", (0x194 << 8) + 3)) {
    ReportPropError(pen, "CPlayerMarker::m_iGiveWeapons");

  } else {
    // Give out specific weapons
    for (INDEX iWeapon = 0; iWeapon < CT_WEAPONS; iWeapon++)
    {
      INDEX iGive = _apsGiveWeapons[iWeapon].GetIndex();

      // Give weapon
      if (iGive == 1) {
        ENTITYPROPERTY(pen, pptrGive.Offset(), INDEX) |= (1 << iWeapon);

      // Take away the weapon
      } else if (iGive == 0) {
        ENTITYPROPERTY(pen, pptrGive.Offset(), INDEX) &= ~(1 << iWeapon);
      }
    }
  }

  // Retrieve CPlayerMarker::m_fMaxAmmoRatio
  static CPropertyPtr pptrMaxAmmo(pen);

  if (!pptrMaxAmmo.ByNameOrId(CEntityProperty::EPT_FLOAT, "Max ammo ratio", (0x194 << 8) + 9)) {
    ReportPropError(pen, "CPlayerMarker::m_fMaxAmmoRatio");

  // Set maximum ammo
  } else if (_psMaxAmmo.GetIndex() != 0) {
    ENTITYPROPERTY(pen, pptrMaxAmmo.Offset(), FLOAT) = 1.0f;
  }

  // Retrieve CPlayerMarker::m_fHealth
  static CPropertyPtr pptrHealth(pen);

  if (!pptrHealth.ByNameOrId(CEntityProperty::EPT_FLOAT, "Health", (0x194 << 8) + 1)) {
    ReportPropError(pen, "CPlayerMarker::m_fHealth");

  // Set custom health
  } else if (_psStartHP.GetFloat() != 100.0f) {
    ENTITYPROPERTY(pen, pptrHealth.Offset(), FLOAT) = _psStartHP.GetFloat();
  }

  // Retrieve CPlayerMarker::m_fShield
  static CPropertyPtr pptrArmor(pen);

  if (!pptrArmor.ByNameOrId(CEntityProperty::EPT_FLOAT, "Shield", (0x194 << 8) + 2)) {
    ReportPropError(pen, "CPlayerMarker::m_fShield");

  // Set custom armor
  } else if (_psStartAR.GetFloat() != 0.0f) {
    ENTITYPROPERTY(pen, pptrArmor.Offset(), FLOAT) = _psStartAR.GetFloat();
  }
};

// Affect enemy spawner at the beginning of the game
void AffectEnemySpawner(CEntity *pen) {
  const FLOAT fEnemyMul = _psEnemyMul.GetFloat();

  // Retrieve CEnemySpawner::m_ctTotal
  static CPropertyPtr pptrTotal(pen);

  if (!pptrTotal.ByNameOrId(CEntityProperty::EPT_INDEX, "Count total", (0x130 << 8) + 8)) {
    ReportPropError(pen, "CEnemySpawner::m_ctTotal");

  // Multiply total amount
  } else if (fEnemyMul > 0.0f) {
    INDEX &iCount = ENTITYPROPERTY(pen, pptrTotal.Offset(), INDEX);
    iCount = ClampDn(INDEX(iCount * fEnemyMul), (INDEX)1);
  }

  // Retrieve CEnemySpawner::m_ctGroupSize
  static CPropertyPtr pptrGroup(pen);

  if (!pptrGroup.ByNameOrId(CEntityProperty::EPT_INDEX, "Count group", (0x130 << 8) + 17)) {
    ReportPropError(pen, "CEnemySpawner::m_ctGroupSize");

  // Multiply group size
  } else if (fEnemyMul > 0.0f) {
    INDEX &iGroup = ENTITYPROPERTY(pen, pptrGroup.Offset(), INDEX);
    iGroup = ClampDn(INDEX(iGroup * fEnemyMul), (INDEX)1);
  }

  // Retrieve CEnemySpawner::m_tmSingleWait
  static CPropertyPtr pptrDelaySingle(pen);

  if (!pptrDelaySingle.ByNameOrId(CEntityProperty::EPT_FLOAT, "Delay single", (0x130 << 8) + 16)) {
    ReportPropError(pen, "CEnemySpawner::m_tmSingleWait");

  // Decrease delay between single enemies
  } else if (fEnemyMul > 0.0f) {
    FLOAT &fTime = ENTITYPROPERTY(pen, pptrDelaySingle.Offset(), FLOAT);
    fTime = ClampDn(fTime / fEnemyMul, 0.05f);
  }

  // Retrieve CEnemySpawner::m_tmGroupWait
  static CPropertyPtr pptrDelayGroup(pen);

  if (!pptrDelayGroup.ByNameOrId(CEntityProperty::EPT_FLOAT, "Delay group", (0x130 << 8) + 5)) {
    ReportPropError(pen, "CEnemySpawner::m_tmGroupWait");

  // Decrease delay between groups of enemies
  } else if (fEnemyMul > 0.0f) {
    FLOAT &fTime = ENTITYPROPERTY(pen, pptrDelayGroup.Offset(), FLOAT);
    fTime = ClampDn(fTime / fEnemyMul, 0.05f);
  }
};
