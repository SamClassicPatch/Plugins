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

#include "Sandbox.h"
#include <CoreLib/Interfaces/PropertyFunctions.h>

// Scheduled commands to be executed after the world loads
CStaticStackArray<CTString> IServerSandbox::astrScheduled;

// Schedule one command
void IServerSandbox::ScheduleCommand(const CTString &strCommand) {
  CPutString(TRANS("Scheduled command for the server:\n"));
  CPrintF("  %s\n", strCommand);

  astrScheduled.Push() = strCommand;
};

// List all scheduled commands in order
void IServerSandbox::ListScheduledCommands(void) {
  if (astrScheduled.Count() == 0) {
    CPutString(TRANS("No commands have been scheduled\n"));
    return;
  }

  CPutString(TRANS("Scheduled commands for the next server start:\n"));

  for (INDEX iCommand = 0; iCommand < astrScheduled.Count(); iCommand++) {
    CPrintF("  %s\n", astrScheduled[iCommand]);
  }
};

// Clear scheduled commands
void IServerSandbox::ClearScheduledCommands(void) {
  CPrintF(TRANS("Cleared %d scheduled commands\n"), astrScheduled.Count());

  astrScheduled.Clear();
};

// Delete an entity from the world
void IServerSandbox::DeleteEntity(INDEX iEntityID) {
  if (iEntityID < 0) {
    CPrintF(TRANS("Invalid entity ID: %d\n"), iEntityID);
    return;
  }

  // Schedule the command before the game starts
  if (!_pNetwork->IsServer()) {
    CTString strCommand;
    strCommand.PrintF("sutl_DeleteEntity(%d);", iEntityID);

    ScheduleCommand(strCommand);
    return;
  }

  CEntity *pen = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);

  // No entity
  if (pen == NULL) {
    CPrintF(TRANS("Could not find entity under ID: %d\n"), iEntityID);
    return;
  }
  
  CPrintF(TRANS("Destroyed '%s' entity under ID: %d\n"), pen->GetName(), iEntityID);
  pen->Destroy();
};

// Initialize/reinitialize an entity
void IServerSandbox::InitEntity(INDEX iEntityID) {
  if (iEntityID < 0) {
    CPrintF(TRANS("Invalid entity ID: %d\n"), iEntityID);
    return;
  }

  // Schedule the command before the game starts
  if (!_pNetwork->IsServer()) {
    CTString strCommand;
    strCommand.PrintF("sutl_InitEntity(%d);", iEntityID);

    ScheduleCommand(strCommand);
    return;
  }

  CEntity *pen = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);

  // No entity
  if (pen == NULL) {
    CPrintF(TRANS("Could not find entity under ID: %d\n"), iEntityID);
    return;
  }

  // Reinitialize if some render type has already been set
  if (pen->GetRenderType() == CEntity::RT_NONE) {
    CPrintF(TRANS("Initialized '%s' entity under ID: %d\n"), pen->GetName(), iEntityID);
    pen->Initialize();

  } else {
    CPrintF(TRANS("Reinitialized '%s' entity under ID: %d\n"), pen->GetName(), iEntityID);
    pen->Reinitialize();
  }
};

// Set new absolute position of an entity
void IServerSandbox::SetEntityPosition(INDEX iEntityID, FLOAT fX, FLOAT fY, FLOAT fZ) {
  if (iEntityID < 0) {
    CPrintF(TRANS("Invalid entity ID: %d\n"), iEntityID);
    return;
  }

  // Schedule the command before the game starts
  if (!_pNetwork->IsServer()) {
    CTString strCommand;
    strCommand.PrintF("sutl_SetEntityPosition(%d, %f, %f, %f);", iEntityID, fX, fY, fZ);

    ScheduleCommand(strCommand);
    return;
  }

  CEntity *pen = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);

  // No entity
  if (pen == NULL) {
    CPrintF(TRANS("Could not find entity under ID: %d\n"), iEntityID);
    return;
  }

  CPlacement3D plEntity = pen->GetPlacement();
  plEntity.pl_PositionVector = FLOAT3D(fX, fY, fZ);

  pen->Teleport(plEntity, FALSE);
};

// Set new absolute rotation of an entity
void IServerSandbox::SetEntityRotation(INDEX iEntityID, FLOAT fH, FLOAT fP, FLOAT fB) {
  if (iEntityID < 0) {
    CPrintF(TRANS("Invalid entity ID: %d\n"), iEntityID);
    return;
  }

  // Schedule the command before the game starts
  if (!_pNetwork->IsServer()) {
    CTString strCommand;
    strCommand.PrintF("sutl_SetEntityRotation(%d, %f, %f, %f);", iEntityID, fH, fP, fB);

    ScheduleCommand(strCommand);
    return;
  }

  CEntity *pen = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);

  // No entity
  if (pen == NULL) {
    CPrintF(TRANS("Could not find entity under ID: %d\n"), iEntityID);
    return;
  }

  CPlacement3D plEntity = pen->GetPlacement();
  plEntity.pl_OrientationAngle = ANGLE3D(fH, fP, fB);

  pen->Teleport(plEntity, FALSE);
};

// Set new value to some property by its name of an entity
void IServerSandbox::SetEntityProperty(INDEX iEntityID, CTString &strProperty, CTString &strValue) {
  if (iEntityID < 0) {
    CPrintF(TRANS("Invalid entity ID: %d\n"), iEntityID);
    return;
  }

  // Schedule the command before the game starts
  if (!_pNetwork->IsServer()) {
    CTString strCommand;
    strCommand.PrintF("sutl_SetEntityProperty(%d, \"%s\", \"%s\");", iEntityID, strProperty, strValue);

    ScheduleCommand(strCommand);
    return;
  }
  
  CEntity *pen = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);

  // No entity
  if (pen == NULL) {
    CPrintF(TRANS("Could not find entity under ID: %d\n"), iEntityID);
    return;
  }

  CEntityProperty *pep = pen->PropertyForName(strProperty);

  // No property
  if (pep == NULL) {
    CPrintF(TRANS("Could not find entity property with the name '%s' in %s\n"), strProperty, pen->en_pecClass->ec_pdecDLLClass->dec_strName);
    return;
  }

  INDEX iPropType = IProperties::ConvertType(pep->ep_eptType);
  BOOL bPropertySet = FALSE;

  switch (iPropType)
  {
    case CEntityProperty::EPT_INDEX: {
      INDEX iIndex;
      strValue.ScanF("%d", &iIndex);

      bPropertySet = IProperties::SetPropValue(pen, pep, &iIndex);
    } break;

    case CEntityProperty::EPT_FLOAT: {
      FLOAT fFloat;
      strValue.ScanF("%g", &fFloat);

      bPropertySet = IProperties::SetPropValue(pen, pep, &fFloat);
    } break;

    case CEntityProperty::EPT_STRING: {
      bPropertySet = IProperties::SetPropValue(pen, pep, &strValue);
    } break;

    case CEntityProperty::EPT_ENTITYPTR: {
      INDEX iEntityID;
      strValue.ScanF("%d", &iEntityID);

      CEntity *penSet = NULL;

      if (iEntityID >= 0) {
        penSet = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);
      }

      bPropertySet = IProperties::SetPropValue(pen, pep, &penSet);
    } break;

    case CEntityProperty::EPT_FLOAT3D: {
      FLOAT3D vVector;
      strValue.ScanF("%g,%g,%g", &vVector(1), &vVector(2), &vVector(3));

      bPropertySet = IProperties::SetPropValue(pen, pep, &vVector);
    } break;
  }

  // Couldn't set new value
  if (!bPropertySet) {
    CPrintF(TRANS("Could not set '%s' value to '%s' property\n"), strValue, strProperty);
  }
};

// Parent an entity to another entity
void IServerSandbox::ParentEntity(INDEX iEntityID, INDEX iParentEntityID) {
  if (iEntityID < 0) {
    CPrintF(TRANS("Invalid entity ID: %d\n"), iEntityID);
    return;
  }

  // Schedule the command before the game starts
  if (!_pNetwork->IsServer()) {
    CTString strCommand;
    strCommand.PrintF("sutl_ParentEntity(%d, %d);", iEntityID, iParentEntityID);

    ScheduleCommand(strCommand);
    return;
  }

  CEntity *pen = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iEntityID);

  // No entity
  if (pen == NULL) {
    CPrintF(TRANS("Could not find entity under ID: %d\n"), iEntityID);
    return;
  }

  // Unparent
  if (iParentEntityID < 0) {
    pen->SetParent(NULL);
    return;
  }

  // Parent to some entity
  CEntity *penParent = IWorld::FindEntityByID(IWorld::GetWorld(), (ULONG)iParentEntityID);
  pen->SetParent(penParent);
};
