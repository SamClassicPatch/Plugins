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

// Affect weapon item at the beginning of the game
void AffectWeaponItem(CEntity *pen) {
  // Retrieve CWeaponItem::m_EwitType
  CEntityProperty *pep = IWorld::FindProperty(pen, (0x322 << 8) + 1, 0x3AC, CEntityProperty::EPT_ENUM);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CWeaponItem::m_EwitType property!\n", pen->GetName(), pen->en_ulID);
    return;
  }

  INDEX &iWeaponType = ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX);

  // Remove weapon item
  if (_apsWeaponItems[iWeaponType].GetIndex() == 0) {
    pen->Destroy();

  // Replace with another weapon
  } else if (_psReplaceWeapons.GetIndex() >= 0) {
    iWeaponType = _psReplaceWeapons.GetIndex();
    pen->Reinitialize();
  }
};

// Affect health item at the beginning of the game
void AffectHealthItem(CEntity *pen) {
  // Retrieve CHealthItem::m_EhitType
  CEntityProperty *pep = IWorld::FindProperty(pen, (0x321 << 8) + 1, 0x3AC, CEntityProperty::EPT_ENUM);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CHealthItem::m_EhitType property!\n", pen->GetName(), pen->en_ulID);
    return;
  }

  INDEX &iHealthType = ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX);

  // Replace with another health item
  if (_psReplaceHealth.GetIndex() >= 0) {
    iHealthType = _psReplaceHealth.GetIndex();
    pen->Reinitialize();
  }
};

// Affect armor item at the beginning of the game
void AffectArmorItem(CEntity *pen) {
  // Retrieve CArmorItem::m_EaitType
  CEntityProperty *pep = IWorld::FindProperty(pen, (0x324 << 8) + 1, 0x3AC, CEntityProperty::EPT_ENUM);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CArmorItem::m_EaitType property!\n", pen->GetName(), pen->en_ulID);
    return;
  }

  INDEX &iArmorType = ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX);

  // Replace with another armor item
  if (_psReplaceArmor.GetIndex() >= 0) {
    iArmorType = _psReplaceArmor.GetIndex();
    pen->Reinitialize();
  }
};

// Affect power up item at the beginning of the game
void AffectPowerUpItem(CEntity *pen) {
  // Retrieve CPowerUpItem::m_puitType
  CEntityProperty *pep = IWorld::FindProperty(pen, (0x328 << 8) + 1, 0x3A8, CEntityProperty::EPT_ENUM);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CPowerUpItem::m_puitType property!\n", pen->GetName(), pen->en_ulID);
    return;
  }

  INDEX &iPowerUp = ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX);

  // Remove power up
  if (_apsPowerUpItems[iPowerUp].GetIndex() == 0) {
    pen->Destroy();
  }
};

// Affect player start marker at the beginning of the game
void AffectPlayerMarker(CEntity *pen) {
  // Retrieve CPlayerMarker::m_iGiveWeapons
  CEntityProperty *pep = IWorld::FindProperty(pen, (0x194 << 8) + 3, 0xE0, CEntityProperty::EPT_INDEX);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CPlayerMarker::m_iGiveWeapons property!\n", pen->GetName(), pen->en_ulID);

  } else {
    // Give out specific weapons
    for (INDEX iWeapon = 0; iWeapon < CT_WEAPONS; iWeapon++)
    {
      if (_apsGiveWeapons[iWeapon].GetIndex() != 0) {
        ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX) |= (1 << iWeapon);
      }
    }
  }
  
  // Retrieve CPlayerMarker::m_fMaxAmmoRatio
  pep = IWorld::FindProperty(pen, (0x194 << 8) + 9, 0xF8, CEntityProperty::EPT_FLOAT);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CPlayerMarker::m_fMaxAmmoRatio property!\n", pen->GetName(), pen->en_ulID);

  // Set maximum ammo
  } else if (_psMaxAmmo.GetIndex() != 0) {
    ENTITYPROPERTY(pen, pep->ep_slOffset, FLOAT) = 1.0f;
  }
  
  // Retrieve CPlayerMarker::m_fHealth
  pep = IWorld::FindProperty(pen, (0x194 << 8) + 1, 0xD8, CEntityProperty::EPT_FLOAT);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CPlayerMarker::m_fHealth property!\n", pen->GetName(), pen->en_ulID);

  // Set custom health
  } else if (_psStartHP.GetFloat() != 100.0f) {
    ENTITYPROPERTY(pen, pep->ep_slOffset, FLOAT) = _psStartHP.GetFloat();
  }
  
  // Retrieve CPlayerMarker::m_fShield
  pep = IWorld::FindProperty(pen, (0x194 << 8) + 2, 0xDC, CEntityProperty::EPT_FLOAT);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CPlayerMarker::m_fShield property!\n", pen->GetName(), pen->en_ulID);

  // Set custom armor
  } else if (_psStartAR.GetFloat() != 0.0f) {
    ENTITYPROPERTY(pen, pep->ep_slOffset, FLOAT) = _psStartAR.GetFloat();
  }
};

// Affect enemy spawner at the beginning of the game
void AffectEnemySpawner(CEntity *pen) {
  const FLOAT fEnemyMul = _psEnemyMul.GetFloat();

  // Retrieve CEnemySpawner::m_ctTotal
  CEntityProperty *pep = IWorld::FindProperty(pen, (0x130 << 8) + 8, 0x110, CEntityProperty::EPT_INDEX);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CEnemySpawner::m_ctTotal property!\n", pen->GetName(), pen->en_ulID);

  // Multiply total amount
  } else if (fEnemyMul > 0.0f) {
    INDEX &iCount = ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX);
    iCount = ClampDn(INDEX(iCount * fEnemyMul), (INDEX)1);
  }

  // Retrieve CEnemySpawner::m_ctGroupSize
  pep = IWorld::FindProperty(pen, (0x130 << 8) + 17, 0x10C, CEntityProperty::EPT_INDEX);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CEnemySpawner::m_ctGroupSize property!\n", pen->GetName(), pen->en_ulID);

  // Multiply group size
  } else if (fEnemyMul > 0.0f) {
    INDEX &iGroup = ENTITYPROPERTY(pen, pep->ep_slOffset, INDEX);
    iGroup = ClampDn(INDEX(iGroup * fEnemyMul), (INDEX)1);
  }

  // Retrieve CEnemySpawner::m_tmSingleWait
  pep = IWorld::FindProperty(pen, (0x130 << 8) + 16, 0x104, CEntityProperty::EPT_FLOAT);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CEnemySpawner::m_tmSingleWait property!\n", pen->GetName(), pen->en_ulID);

  // Decrease delay between single enemies
  } else if (fEnemyMul > 0.0f) {
    FLOAT &fTime = ENTITYPROPERTY(pen, pep->ep_slOffset, FLOAT);
    fTime = ClampDn(fTime / fEnemyMul, 0.05f);
  }

  // Decrease delay between groups
  pep = IWorld::FindProperty(pen, (0x130 << 8) + 5, 0x108, CEntityProperty::EPT_FLOAT);

  if (pep == NULL) {
    CPrintF("'%s' (%u) : Cannot retieve CEnemySpawner::m_tmGroupWait property!\n", pen->GetName(), pen->en_ulID);

  // Decrease delay between groups of enemies
  } else if (fEnemyMul > 0.0f) {
    FLOAT &fTime = ENTITYPROPERTY(pen, pep->ep_slOffset, FLOAT);
    fTime = ClampDn(fTime / fEnemyMul, 0.05f);
  }
};
