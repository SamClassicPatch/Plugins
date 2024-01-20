/* Copyright (c) 2024 Dreamy Cecil
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

// Define packet events for the plugin

void IPacketEvents::OnCharacterConnect(INDEX iClient, CPlayerCharacter &pc)
{
  CPlayerSettings *pps = (CPlayerSettings *)pc.pc_aubAppearance;

  // Disallow third person view
  if (_psThirdPerson.GetIndex() != -1) {
    pps->ps_ulFlags &= ~PSF_PREFER3RDPERSON;
  }
};

BOOL IPacketEvents::OnCharacterChange(INDEX iClient, INDEX iPlayer, CPlayerCharacter &pc)
{
  CPlayerSettings *pps = (CPlayerSettings *)pc.pc_aubAppearance;

  // Disallow third person view
  if (_psThirdPerson.GetIndex() != -1) {
    pps->ps_ulFlags &= ~PSF_PREFER3RDPERSON;
  }
  return TRUE;
};

void IPacketEvents::OnPlayerAction(INDEX iClient, INDEX iPlayer, CPlayerAction &pa, INDEX iResent)
{
  // Lock disabled buttons
  pa.pa_ulButtons &= _psActionButtons.GetIndex();

  // Disallow toggling third person view using a specified button
  INDEX i3rdPersonButton = _psThirdPerson.GetIndex();

  if (i3rdPersonButton != -1) {
    pa.pa_ulButtons &= ~(1 << i3rdPersonButton);
  }
};

BOOL IPacketEvents::OnChatMessage(INDEX iClient, ULONG ulFrom, ULONG ulTo, CTString &strMessage)
{
  return TRUE;
};
