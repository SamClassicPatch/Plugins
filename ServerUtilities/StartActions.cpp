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

// Verify item type and reset it to default value if it's invalid
static INDEX VerifyItemType(CEntity *pen, const CPropertyPtr &pptrType, INDEX iType) {
  CTString strType = pptrType._pep->ep_pepetEnumType->NameForValue(iType);

  // Set default valid type if invalid
  if (strType == "") {
    CPrintF(TRANS("'%s' (%u) : Item type %d is invalid!\n"), pen->GetName(), pen->en_ulID, iType);
    return 1;
  }

  return iType;
};

// Affect weapon item at the beginning of the game
void AffectWeaponItem(CEntity *pen) {
  // Retrieve CWeaponItem::m_EwitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByVariable("CWeaponItem", "m_EwitType")) {
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
    iWeaponType = VerifyItemType(pen, pptr, iSetType);
    pen->Reinitialize();

  // Replace with another weapon
  } else if (_psReplaceWeapons.GetIndex() >= 0) {
    iWeaponType = VerifyItemType(pen, pptr, _psReplaceWeapons.GetIndex());
    pen->Reinitialize();
  }
};

// Affect ammo item at the beginning of the game
void AffectAmmoItem(CEntity *pen) {
  // Retrieve CAmmoItem::m_EaitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByVariable("CAmmoItem", "m_EaitType")) {
    ReportPropError(pen, "CAmmoItem::m_EaitType");
    return;
  }

  INDEX &iAmmoType = ENTITYPROPERTY(pen, pptr.Offset(), INDEX);
  INDEX iSetType = _apsAmmoItems[iAmmoType].GetIndex();

  // Remove ammo item
  if (iSetType == -1) {
    pen->Destroy();

  // Set specific type
  } else if (iSetType >= 0) {
    iAmmoType = VerifyItemType(pen, pptr, iSetType);
    pen->Reinitialize();

  // Replace with another weapon
  } else if (_psReplaceAmmo.GetIndex() >= 0) {
    iAmmoType = VerifyItemType(pen, pptr, _psReplaceAmmo.GetIndex());
    pen->Reinitialize();
  }
};

// Affect health item at the beginning of the game
void AffectHealthItem(CEntity *pen) {
  // Retrieve CHealthItem::m_EhitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByVariable("CHealthItem", "m_EhitType")) {
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
    iHealthType = VerifyItemType(pen, pptr, iSetType);
    pen->Reinitialize();

  // Replace with another health item
  } else if (_psReplaceHealth.GetIndex() >= 0) {
    iHealthType = VerifyItemType(pen, pptr, _psReplaceHealth.GetIndex());
    pen->Reinitialize();
  }
};

// Affect armor item at the beginning of the game
void AffectArmorItem(CEntity *pen) {
  // Retrieve CArmorItem::m_EaitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByVariable("CArmorItem", "m_EaitType")) {
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
    iArmorType = VerifyItemType(pen, pptr, iSetType);
    pen->Reinitialize();

  // Replace with another armor item
  } else if (_psReplaceArmor.GetIndex() >= 0) {
    iArmorType = VerifyItemType(pen, pptr, _psReplaceArmor.GetIndex());
    pen->Reinitialize();
  }
};

// Affect power up item at the beginning of the game
void AffectPowerUpItem(CEntity *pen) {
  // Retrieve CPowerUpItem::m_puitType
  static CPropertyPtr pptr(pen);

  if (!pptr.ByVariable("CPowerUpItem", "m_puitType")) {
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
    iPowerUp = VerifyItemType(pen, pptr, iSetType);
    pen->Reinitialize();
  }
};

// Affect player start marker at the beginning of the game
void AffectPlayerMarker(CEntity *pen) {
  // Retrieve CPlayerMarker::m_iGiveWeapons
  static CPropertyPtr pptrGive(pen);

  if (!pptrGive.ByVariable("CPlayerMarker", "m_iGiveWeapons")) {
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

  if (!pptrMaxAmmo.ByVariable("CPlayerMarker", "m_fMaxAmmoRatio")) {
    ReportPropError(pen, "CPlayerMarker::m_fMaxAmmoRatio");

  // Set maximum ammo
  } else if (_psMaxAmmo.GetIndex() != 0) {
    ENTITYPROPERTY(pen, pptrMaxAmmo.Offset(), FLOAT) = 1.0f;
  }

  // Retrieve CPlayerMarker::m_iTakeAmmo
  static CPropertyPtr pptrTakeAmmo(pen);

  if (!pptrTakeAmmo.ByVariable("CPlayerMarker", "m_iTakeAmmo")) {
    ReportPropError(pen, "CPlayerMarker::m_iTakeAmmo");

  // Allow maximum ammo for all available weapons
  } else if (_psMaxAmmo.GetIndex() != 0) {
    ENTITYPROPERTY(pen, pptrTakeAmmo.Offset(), INDEX) = 0;
  }

  // Retrieve CPlayerMarker::m_fHealth
  static CPropertyPtr pptrHealth(pen);

  if (!pptrHealth.ByVariable("CPlayerMarker", "m_fHealth")) {
    ReportPropError(pen, "CPlayerMarker::m_fHealth");

  // Set custom health
  } else if (_psStartHP.GetFloat() != 100.0f) {
    ENTITYPROPERTY(pen, pptrHealth.Offset(), FLOAT) = _psStartHP.GetFloat();
  }

  // Retrieve CPlayerMarker::m_fShield
  static CPropertyPtr pptrArmor(pen);

  if (!pptrArmor.ByVariable("CPlayerMarker", "m_fShield")) {
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

  if (!pptrTotal.ByVariable("CEnemySpawner", "m_ctTotal")) {
    ReportPropError(pen, "CEnemySpawner::m_ctTotal");

  // Multiply total amount
  } else if (fEnemyMul > 0.0f) {
    INDEX &iCount = ENTITYPROPERTY(pen, pptrTotal.Offset(), INDEX);
    iCount = ClampDn(INDEX(iCount * fEnemyMul), (INDEX)1);
  }

  // Retrieve CEnemySpawner::m_ctGroupSize
  static CPropertyPtr pptrGroup(pen);

  if (!pptrGroup.ByVariable("CEnemySpawner", "m_ctGroupSize")) {
    ReportPropError(pen, "CEnemySpawner::m_ctGroupSize");

  // Multiply group size
  } else if (fEnemyMul > 0.0f) {
    INDEX &iGroup = ENTITYPROPERTY(pen, pptrGroup.Offset(), INDEX);
    iGroup = ClampDn(INDEX(iGroup * fEnemyMul), (INDEX)1);
  }

  // Retrieve CEnemySpawner::m_tmSingleWait
  static CPropertyPtr pptrDelaySingle(pen);

  if (!pptrDelaySingle.ByVariable("CEnemySpawner", "m_tmSingleWait")) {
    ReportPropError(pen, "CEnemySpawner::m_tmSingleWait");

  // Decrease delay between single enemies
  } else if (fEnemyMul > 0.0f) {
    FLOAT &fTime = ENTITYPROPERTY(pen, pptrDelaySingle.Offset(), FLOAT);
    fTime = ClampDn(fTime / fEnemyMul, 0.05f);
  }

  // Retrieve CEnemySpawner::m_tmGroupWait
  static CPropertyPtr pptrDelayGroup(pen);

  if (!pptrDelayGroup.ByVariable("CEnemySpawner", "m_tmGroupWait")) {
    ReportPropError(pen, "CEnemySpawner::m_tmGroupWait");

  // Decrease delay between groups of enemies
  } else if (fEnemyMul > 0.0f) {
    FLOAT &fTime = ENTITYPROPERTY(pen, pptrDelayGroup.Offset(), FLOAT);
    fTime = ClampDn(fTime / fEnemyMul, 0.05f);
  }
};
