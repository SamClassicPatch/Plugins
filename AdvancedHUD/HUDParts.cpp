/* Copyright (c) 2023 Dreamy Cecil
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

#include "HUD.h"

#include <CoreLib/Compatibility/SymbolPtr.h>
#include <CoreLib/Objects/PropertyPtr.h>

// Armor & health constants (they don't reflect tourist/easy top values by design)
#define TOP_ARMOR  100
#define TOP_HEALTH 100

void CHud::RenderVitals(void) {
  // Prepare and draw health info
  FLOAT fValue = ClampDn(_penPlayer->GetHealth(), 0.0f);
  FLOAT fNormValue = fValue / TOP_HEALTH;

  CTString strValue;
  strValue.PrintF("%d", (SLONG)ceil(fValue));

  PrepareColorTransitions(_colMax, _colTop, _colMid, C_RED, 0.5f, 0.25f, FALSE);

  FLOAT fMoverX, fMoverY;
  COLOR colDefault = AddShaker(5, fValue, _penLast->m_iLastHealth, _penLast->m_tmHealthChanged, fMoverX, fMoverY);

  FLOAT fCol = _vpixTL(1) + units.fHalf;
  FLOAT fRow = _vpixBR(2) - units.fHalf;

  DrawBorder(fCol + fMoverX, fRow + fMoverY, units.fOne, units.fOne, _colBorder);
  DrawIcon(fCol + fMoverX, fRow + fMoverY, tex.toHealth, _colIconStd, fNormValue, TRUE);

  fCol += units.fAdv + units.fChar * 1.5f - units.fHalf;

  DrawBorder(fCol, fRow, units.fChar * 3, units.fOne, _colBorder);
  DrawString(fCol, fRow, strValue, colDefault, fNormValue);

  // Prepare and draw armor info
  fValue = _penPlayer->m_fArmor;

  // No armor
  if (fValue <= 0.0f) return;

  fNormValue = fValue / TOP_ARMOR;
  strValue.PrintF("%d", (SLONG)ceil(fValue));

  PrepareColorTransitions(_colMax, _colTop, _colMid, C_lGRAY, 0.5f, 0.25f, FALSE);

  fCol = _vpixTL(1) + units.fHalf;
  fRow = _vpixBR(2) - (units.fNext + units.fHalf);

  colDefault = AddShaker(3, fValue, _penLast->m_iLastArmor, _penLast->m_tmArmorChanged, fMoverX, fMoverY);

  fCol += fMoverX;
  fRow += fMoverY;

  DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);

  if (_bTSETheme) {
    if (fValue <= 50.5f) {
      DrawIcon(fCol, fRow, tex.atoArmor[1], _colIconStd, fNormValue, FALSE);

    } else if (fValue <= 100.5f) {
      DrawIcon(fCol, fRow, tex.atoArmor[2], _colIconStd, fNormValue, FALSE);

    } else {
      DrawIcon(fCol, fRow, tex.atoArmor[3], _colIconStd, fNormValue, FALSE);
    }

  } else {
    DrawIcon(fCol, fRow, tex.atoArmor[0], _colIconStd, fNormValue, FALSE);
  }

  fCol -= fMoverX;
  fRow -= fMoverY;

  fCol += units.fAdv + units.fChar * 1.5f - units.fHalf;
  DrawBorder(fCol, fRow, units.fChar * 3, units.fOne, _colBorder);
  DrawString(fCol, fRow, strValue, NONE, fNormValue);
};

void CHud::RenderCurrentWeapon(SIconTexture **pptoWantedWeapon, SIconTexture **pptoCurrentAmmo) {
  // Prepare and draw ammo and weapon info
  SIconTexture *ptoAmmo = NULL;
  SIconTexture *ptoCurrent = NULL;
  SIconTexture *ptoWanted = NULL;
  INDEX iCurrentWeapon = _penWeapons->m_iCurrentWeapon;
  INDEX iWantedWeapon  = _penWeapons->m_iWantedWeapon;

  // Determine corresponding ammo and weapon texture component
  HudWeapon *pWeaponInfo = NULL;

  if (GetWeaponInfo(&pWeaponInfo, iCurrentWeapon)) {
    ptoCurrent = pWeaponInfo->ptoWeapon;

    if (pWeaponInfo->paiAmmo != NULL) {
      ptoAmmo = pWeaponInfo->paiAmmo->ptoAmmo;
    }
  }

  if (GetWeaponInfo(&pWeaponInfo, iWantedWeapon)) {
    ptoWanted = pWeaponInfo->ptoWeapon;
  }

  // Borrow icons
  if (pptoCurrentAmmo != NULL) {
    *pptoCurrentAmmo = ptoAmmo;
  }

  if (pptoWantedWeapon != NULL) {
    *pptoWantedWeapon = ptoWanted;
  }

  FLOAT fCol = 320;
  FLOAT fRow = _vpixBR(2) - units.fHalf;

  // Draw weapons with ammo
  if (ptoAmmo != NULL && !GetSP()->sp_bInfiniteAmmo) {
    // Get amount of ammo
    INDEX iMaxValue = _penWeapons->GetMaxAmmo();
    INDEX iValue = _penWeapons->GetAmmo();
    FLOAT fNormValue = (FLOAT)iValue / (FLOAT)iMaxValue;

    CTString strValue;
    strValue.PrintF("%d", iValue);

    PrepareColorTransitions(_colMax, _colTop, _colMid, C_RED, (_bTSEColors ? 0.30f : 0.5f), (_bTSEColors ? 0.15f : 0.25f), FALSE);
    BOOL bDrawAmmoIcon = _fCustomScaling <= 1.0f;

    // draw ammo, value and weapon
    FLOAT fMoverX, fMoverY;
    COLOR colDefault = AddShaker(4, iValue, _penLast->m_iLastAmmo, _penLast->m_tmAmmoChanged, fMoverX, fMoverY);

    fCol -= units.fAdv + units.fChar * 1.5f - units.fHalf;

    DrawBorder(fCol + fMoverX, fRow + fMoverY, units.fOne, units.fOne, _colBorder);
    DrawIcon(fCol + fMoverX, fRow + fMoverY, *ptoCurrent, _colIconStd, fNormValue, !bDrawAmmoIcon);

    fCol += units.fAdv + units.fChar * 1.5f - units.fHalf;
    DrawBorder(fCol, fRow, units.fChar * 3, units.fOne, _colBorder);
    DrawString(fCol, fRow, strValue, (_bTSEColors ? NONE : colDefault), fNormValue);

    if (bDrawAmmoIcon) {
      fCol += units.fAdv + units.fChar * 1.5f - units.fHalf;
      DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
      DrawIcon(fCol, fRow, *ptoAmmo, _colIconStd, fNormValue, TRUE);
    }

  // Draw weapons without ammo
  } else if (ptoCurrent != NULL) {
    DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
    DrawIcon(fCol, fRow, *ptoCurrent, _colIconStd, 1.0f, FALSE);
  }
};

void CHud::RenderActiveArsenal(SIconTexture *ptoAmmo) {
  PrepareColorTransitions(_colMax, _colTop, _colMid, C_RED, 0.5f, 0.25f, FALSE);

  // Prepare position and the weapon arsenal
  FLOAT fCol = _vpixBR(1) - units.fHalf;
  FLOAT fRow = _vpixBR(2) - units.fHalf;
  const FLOAT fBarPos = units.fHalf * 0.7f;

  UpdateWeaponArsenal();

#if SE1_GAME != SS_TFE
  // Display stored bombs
  #define BOMB_FIRE_TIME 1.5f

  INDEX iBombCount = _penPlayer->m_iSeriousBombCount;
  BOOL bBombFiring = FALSE;

  // Active Serious Bomb
  if (_penPlayer->m_tmSeriousBombFired + BOMB_FIRE_TIME > _pTimer->GetLerpedCurrentTick()) {
    iBombCount = ClampUp(INDEX(iBombCount + 1), (INDEX)3);
    bBombFiring = TRUE;
  }

  if (iBombCount > 0) {
    FLOAT fNormValue = (FLOAT)iBombCount / 3.0f;

    COLOR colBombBorder = _colHUD;
    COLOR colBombIcon = _colIconStd;
    COLOR colBombBar = (iBombCount == 1) ? C_RED : (_bTSEColors ? _colHUDText : _colHUD);

    if (bBombFiring) {
      FLOAT fFactor = (_pTimer->GetLerpedCurrentTick() - _penPlayer->m_tmSeriousBombFired) / BOMB_FIRE_TIME;
      colBombBorder = LerpColor(colBombBorder, C_RED, fFactor);
      colBombIcon = LerpColor(colBombIcon, C_RED, fFactor);
      colBombBar = LerpColor(colBombBar, C_RED, fFactor);
    }

    DrawBorder(fCol, fRow, units.fOne, units.fOne, colBombBorder);
    DrawIcon(fCol, fRow, tex.toASeriousBomb, colBombIcon, fNormValue, FALSE);
    DrawBar(fCol + fBarPos, fRow, units.fOne * 0.2f, units.fOne - 2, E_BD_DOWN, colBombBar, fNormValue);

    // Make space for bombs
    fCol -= units.fAdv;
  }
#endif

  // Display available ammo
  if (!GetSP()->sp_bInfiniteAmmo) {
    for (INDEX iAmmo = GetAmmo().Count() - 1; iAmmo >= 0; iAmmo--) {
      HudAmmo &ai = GetAmmo()[iAmmo];
      ASSERT(ai.iAmmo >= 0);

      // No ammo and no weapon that uses it
      if (ai.iAmmo == 0 && !ai.bHasWeapon) continue;

      // Display ammo info
      COLOR colIcon = _colIconStd;

      if (ptoAmmo == ai.ptoAmmo) {
        colIcon = C_WHITE;

      } else if (ai.iAmmo == 0) {
        colIcon = (_bTSEColors ? C_mdGRAY : C_GRAY);
      }

      FLOAT fNormValue = (FLOAT)ai.iAmmo / (FLOAT)ai.iMaxAmmo;

      FLOAT fMoverX, fMoverY;
      COLOR colBar = AddShaker(4, ai.iAmmo, ai.iLastAmmo, ai.tmAmmoChanged, fMoverX, fMoverY);

      DrawBorder(fCol, fRow + fMoverY, units.fOne, units.fOne, _colBorder);
      DrawIcon(fCol, fRow + fMoverY, *ai.ptoAmmo, colIcon, fNormValue, FALSE);
      DrawBar(fCol + fBarPos, fRow + fMoverY, units.fOne * 0.2f, units.fOne - 2, E_BD_DOWN, colBar, fNormValue);

      // Advance to the next position
      fCol -= units.fAdv;
    }
  }

#if SE1_GAME != SS_TFE
  // Display active powerups
  PrepareColorTransitions(_colMax, _colTop, _colMid, C_RED, 0.66f, 0.33f, FALSE);

  const FLOAT *ptmPowerups = &_penPlayer->m_tmInvisibility;
  const FLOAT *ptmPowerupsMax = &_penPlayer->m_tmInvisibilityMax;

  fCol = _vpixBR(1) - units.fHalf;
  fRow = _vpixBR(2) - units.fOne - units.fAdv;

  for (INDEX iPowerUp = 0; iPowerUp < MAX_POWERUPS; iPowerUp++) {
    const TIME tmDelta = ptmPowerups[iPowerUp] - _tmNow;

    // Skip inactive
    if (tmDelta <= 0) continue;

    FLOAT fNormValue = tmDelta / ptmPowerupsMax[iPowerUp];

    // Draw icon and a little bar
    DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
    DrawIcon(fCol, fRow, tex.atoPowerups[iPowerUp], _colIconStd, fNormValue, TRUE);
    DrawBar(fCol + fBarPos, fRow, units.fOne * 0.2f, units.fOne - 2, E_BD_DOWN, NONE, fNormValue);

    // Play sound if icon is flashing
    if (fNormValue <= _cttHUD.ctt_fLowMedium * 0.5f) {
      INDEX iLastTime = INDEX(_tmLast * 4);
      INDEX iCurrentTime = INDEX(_tmNow * 4);

      if (iCurrentTime & 1 && !(iLastTime & 1)) {
        ((CPlayer *)_penPlayer)->PlayPowerUpSound();
      }
    }

    // Advance to the next position
    fCol -= units.fAdv;
  }
#endif
};

void CHud::RenderBars(void) {
  // Draw oxygen info
  BOOL bOxygenOnScreen = FALSE;
  FLOAT fValue = _penPlayer->en_tmMaxHoldBreath - (_pTimer->CurrentTick() - _penPlayer->en_tmLastBreathed);

  if (_penPlayer->IsConnected() && _penPlayer->GetFlags() & ENF_ALIVE && fValue < 30.0f) { 
    FLOAT fCol = 280.0f;
    FLOAT fRow = _vpixTL(2) + units.fOne + units.fNext;
    FLOAT fAdv = units.fAdv + units.fOne * 2 - units.fHalf;

    PrepareColorTransitions( _colMax, _colTop, _colMid, C_RED, 0.5f, 0.25f, FALSE);

    FLOAT fNormValue = ClampDn(fValue / 30.0f, 0.0f);

    DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
    DrawBorder(fCol + fAdv, fRow, units.fOne * 4, units.fOne, _colBorder);
    DrawBar(fCol + fAdv, fRow, units.fOne * 4 * 0.975f, units.fOne * 0.9375f, E_BD_LEFT, NONE, fNormValue);
    DrawIcon(fCol, fRow, tex.toOxygen, _colIconStd, fNormValue, TRUE);

    bOxygenOnScreen = TRUE;
  }

  // Draw boss energy
  if (_penPlayer->m_penMainMusicHolder != NULL) {
    CMusicHolder &mh = (CMusicHolder &)*_penPlayer->m_penMainMusicHolder;
    FLOAT fNormValue = 0;

    if (mh.m_penBoss != NULL && mh.m_penBoss->GetFlags() & ENF_ALIVE) {
      CEnemyBase &eb = (CEnemyBase &)*mh.m_penBoss;
      ASSERT(eb.m_fMaxHealth > 0);

      fValue = eb.GetHealth();
      fNormValue = fValue / eb.m_fMaxHealth;
    }

    if (mh.m_penCounter != NULL) {
      CEntity *penCounter = mh.m_penCounter;

      // Find both counter properties
      static CPropertyPtr pptrCount(penCounter);
      static CPropertyPtr pptrCountFrom(penCounter);

      BOOL bProps = pptrCount.ByIdOrOffset(CEntityProperty::EPT_INDEX, (0x153 << 8) + 5, -1)
        && pptrCountFrom.ByNameOrId(CEntityProperty::EPT_INDEX, "Count start", (0x153 << 8) + 4);

      if (bProps) {
        INDEX iCount = ENTITYPROPERTY(penCounter, pptrCount.Offset(), INDEX);
        INDEX iCountFrom = ENTITYPROPERTY(penCounter, pptrCountFrom.Offset(), INDEX);

        if (iCount > 0) {
          fValue = iCount;
          fNormValue = fValue / iCountFrom;
        }
      }
    }

    if (fNormValue > 0) {
      if (_bTSETheme) {
        PrepareColorTransitions(_colMax, _colMax, _colTop, C_RED, 0.5f, 0.25f, FALSE);
      } else {
        PrepareColorTransitions(_colMax, _colTop, _colMid, C_RED, 0.5f, 0.25f, FALSE);
      }

      FLOAT fCol = 184.0f;
      FLOAT fRow = _vpixTL(2) + units.fOne + units.fNext;
      FLOAT fAdv = units.fAdv + units.fOne * 8 - units.fHalf;

      if (bOxygenOnScreen) {
        fRow += units.fNext;
      }

      DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
      DrawBorder(fCol + fAdv, fRow, units.fOne * 16, units.fOne, _colBorder);
      DrawBar(fCol + fAdv, fRow, units.fOne * 16 * 0.995f, units.fOne * 0.9375f, E_BD_LEFT, NONE, fNormValue);
      DrawIcon(fCol, fRow, tex.toHealth, _colIconStd, fNormValue, FALSE);
    }
  }
};

void CHud::RenderGameModeInfo(EGameMode eMode) {
  static CSymbolPtr pbMessages("hud_bShowMessages");
  static CSymbolPtr piPlayers("hud_iShowPlayers");
  static CSymbolPtr piSort("hud_iSortPlayers");

  const INDEX bShowMessages = pbMessages.GetIndex();
  const INDEX iShowPlayers = piPlayers.GetIndex();

#if SE1_GAME == SS_TFE
  const INDEX bShowMatchInfo = _psShowMatchInfo.GetIndex();
#else
  static CSymbolPtr pbMatchInfo("hud_bShowMatchInfo");
  const INDEX bShowMatchInfo = pbMatchInfo.GetIndex();
#endif

  COLOR colMana, colFrags, colDeaths, colHealth, colArmor;
  COLOR colScore = _colHUD;
  INDEX iScoreSum = 0;

  // Normal text scaling
  const FLOAT fTextScale = (_vScaling(2) + 1) * 0.5f;

  // Display player list if not in singleplayer
  if (eMode != E_GM_SP) {
    // Set font
    _pfdDisplayFont->SetVariableWidth();
    _pdp->SetFont(_pfdDisplayFont);
    _pdp->SetTextScaling(fTextScale);

    // Sort player list
    BOOL bMaxScore = TRUE;
    BOOL bMaxMana = TRUE;
    BOOL bMaxFrags = TRUE;
    BOOL bMaxDeaths = TRUE;

    INDEX iSortPlayers = Clamp(piSort.GetIndex(), -1L, 6L);
    ESortKeys eKey = (ESortKeys)iSortPlayers;

    if (iSortPlayers == -1) {
      switch (eMode) {
        case E_GM_COOP:  eKey = E_SK_HEALTH; break;
        case E_GM_SCORE: eKey = E_SK_SCORE; break;
        case E_GM_FRAG:  eKey = E_SK_FRAGS; break;

        default: {
          ASSERT(FALSE);
          eKey = E_SK_NAME;
        }
      }
    }

    if (eMode == E_GM_COOP) {
      eKey = (ESortKeys)Clamp((INDEX)eKey, 0L, 3L);

    // Don't sort by health in deathmatch
    } else if (eKey == E_SK_HEALTH) {
      eKey = E_SK_NAME;
    }

    SetAllPlayersStats(eKey);

    // Go through all players
    INDEX iPlayer = 0;

    FOREACHINDYNAMICCONTAINER(_cenPlayers, CPlayer, iten) {
      CPlayer *penPlayer = iten;

      // Get player stats as strings
      const CTString strName = penPlayer->GetPlayerName();
      const INDEX iScore = penPlayer->m_psGameStats.ps_iScore;
      const INDEX iMana = penPlayer->m_iMana;
      const INDEX iFrags = penPlayer->m_psGameStats.ps_iKills;
      const INDEX iDeaths = penPlayer->m_psGameStats.ps_iDeaths;
      const INDEX iHealth = ClampDn((INDEX)ceil(penPlayer->GetHealth()), 0L);
      const INDEX iArmor = ClampDn((INDEX)ceil(penPlayer->m_fArmor), 0L);

      CTString strScore, strMana, strFrags, strDeaths, strHealth, strArmor;
      strScore.PrintF("%d", iScore);
      strMana.PrintF("%d", iMana);
      strFrags.PrintF("%d", iFrags);
      strDeaths.PrintF("%d", iDeaths);
      strHealth.PrintF("%d", iHealth);
      strArmor.PrintF("%d", iArmor);

      // Detemine corresponding colors
      colHealth = C_mlRED;
      colMana = colScore = colFrags = colDeaths = colArmor = C_lGRAY;

      if (iMana > _penPlayer->m_iMana) {
        bMaxMana = FALSE;
        colMana = C_WHITE;
      }

      if (iScore > _penPlayer->m_psGameStats.ps_iScore) {
        bMaxScore = FALSE;
        colScore = C_WHITE;
      }

      if (iFrags > _penPlayer->m_psGameStats.ps_iKills) {
        bMaxFrags = FALSE;
        colFrags = C_WHITE;
      }

      if (iDeaths > _penPlayer->m_psGameStats.ps_iDeaths) {
        bMaxDeaths = FALSE;
        colDeaths = C_WHITE;
      }

      // Current player
      if (penPlayer == _penPlayer) {
        colScore = colMana = colFrags = colDeaths = _colHUD;
      }

      // Enough health and armor
      if (iHealth > 25) colHealth = _colHUD;
      if (iArmor > 25) colArmor  = _colHUD;

      // Put player in the list
      if ((iShowPlayers == 1 || iShowPlayers == -1) && eMode != E_GM_SP) {
        const PIX pixCharW = (_pfdDisplayFont->GetWidth() - 2) * fTextScale;
        const PIX pixCharH = (_pfdDisplayFont->GetHeight() - 2) * fTextScale;

        #define PLAYER_INFO_X(Offset) (_vpixBR(1) * _vScaling(1) - Offset * pixCharW)
        const PIX pixInfoY = (_vpixTL(2) + units.fNext) * _vScaling(2) + pixCharH * iPlayer;

        if (eMode == E_GM_COOP) {
          _pdp->PutTextR(strName,   PLAYER_INFO_X(8), pixInfoY, colScore  | _ulAlphaHUD);
          _pdp->PutTextC(strHealth, PLAYER_INFO_X(6), pixInfoY, colHealth | _ulAlphaHUD);
          _pdp->PutText("/",        PLAYER_INFO_X(4), pixInfoY, _colHUD   | _ulAlphaHUD);
          _pdp->PutTextC(strArmor,  PLAYER_INFO_X(2), pixInfoY, colArmor  | _ulAlphaHUD);

        } else if (eMode == E_GM_SCORE) {
          _pdp->PutTextR(strName,  PLAYER_INFO_X(12), pixInfoY, _colHUD  | _ulAlphaHUD);
          _pdp->PutTextC(strScore, PLAYER_INFO_X(8),  pixInfoY, colScore | _ulAlphaHUD);
          _pdp->PutText("/",       PLAYER_INFO_X(5),  pixInfoY, _colHUD  | _ulAlphaHUD);
          _pdp->PutTextC(strMana,  PLAYER_INFO_X(2),  pixInfoY, colMana  | _ulAlphaHUD);

        } else {
          _pdp->PutTextR(strName,   PLAYER_INFO_X(8), pixInfoY, _colHUD   | _ulAlphaHUD);
          _pdp->PutTextC(strFrags,  PLAYER_INFO_X(6), pixInfoY, colFrags  | _ulAlphaHUD);
          _pdp->PutText("/",        PLAYER_INFO_X(4), pixInfoY, _colHUD   | _ulAlphaHUD);
          _pdp->PutTextC(strDeaths, PLAYER_INFO_X(2), pixInfoY, colDeaths | _ulAlphaHUD);
        }
      }

      // Summarize score for coop
      iScoreSum += iScore;

      // Next player in the list
      iPlayer++;
    }

    if ((eMode == E_GM_SCORE || eMode == E_GM_FRAG) && bShowMatchInfo) {
      CTString strLimitsInfo = "";

      // Draw remaining time
      if (GetSP()->sp_iTimeLimit > 0) {
        FLOAT fTimeLeft = ClampDn(GetSP()->sp_iTimeLimit * 60.0f - _pNetwork->GetGameTime(), 0.0f);
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %s\n", strLimitsInfo, TRANS("TIME LEFT"), TimeToString(fTimeLeft));
      }

      // Sort player list by frags or score
      SetAllPlayersStats(eMode == E_GM_FRAG ? E_SK_FRAGS : E_SK_SCORE);

      // Find maximum frags and score from players
      INDEX iMaxFrags = LowerLimit(INDEX(0));
      INDEX iMaxScore = LowerLimit(INDEX(0));

      {FOREACHINDYNAMICCONTAINER(_cenPlayers, CPlayer, itenStats) {
        iMaxFrags = Max(iMaxFrags, itenStats->m_psLevelStats.ps_iKills);
        iMaxScore = Max(iMaxScore, itenStats->m_psLevelStats.ps_iScore);
      }}

      if (GetSP()->sp_iFragLimit > 0) {
        INDEX iFragsLeft = ClampDn(GetSP()->sp_iFragLimit-iMaxFrags, INDEX(0));
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("FRAGS LEFT"), iFragsLeft);
      }

      if (GetSP()->sp_iScoreLimit > 0) {
        INDEX iScoreLeft = ClampDn(GetSP()->sp_iScoreLimit-iMaxScore, INDEX(0));
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("SCORE LEFT"), iScoreLeft);
      }

      _pfdDisplayFont->SetFixedWidth();
      _pdp->SetFont(_pfdDisplayFont);
      _pdp->SetTextScaling(fTextScale * 0.8f);
      _pdp->SetTextCharSpacing(-2.0f * fTextScale);
      _pdp->PutText(strLimitsInfo, 5 * _vScaling(1), 48 * _vScaling(1), C_WHITE | CT_OPAQUE);
    }

    // Prepare colors for local player printouts
    colScore  = (bMaxScore  ? C_WHITE : C_lGRAY);
    colMana   = (bMaxMana   ? C_WHITE : C_lGRAY);
    colFrags  = (bMaxFrags  ? C_WHITE : C_lGRAY);
    colDeaths = (bMaxDeaths ? C_WHITE : C_lGRAY);
  }

  // Restore font defaults
  _pfdDisplayFont->SetVariableWidth();
  _pdp->SetFont(&_fdNumbersFont);
  _pdp->SetTextCharSpacing(1);

  // Prepare outputs depending on gamemode
  FLOAT fWidthAdj = 8;
  INDEX iScore = _penPlayer->m_psGameStats.ps_iScore;
  INDEX iMana = _penPlayer->m_iMana;

  if (eMode == E_GM_FRAG) {
    if (!bShowMatchInfo) {
      fWidthAdj = 4;
    }

    iScore = _penPlayer->m_psGameStats.ps_iKills;
    iMana = _penPlayer->m_psGameStats.ps_iDeaths;

  // Show score in coop
  } else if (eMode == E_GM_COOP) {
    iScore = iScoreSum;
  }

  // Draw score or frags
  CTString strValue;
  strValue.PrintF("%d", iScore);

  FLOAT fCol = _vpixTL(1) + units.fHalf;
  FLOAT fRow = _vpixTL(2) + units.fHalf;
  FLOAT fAdv = units.fAdv + units.fChar * fWidthAdj * 0.5f - units.fHalf;

  DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
  DrawBorder(fCol + fAdv, fRow, units.fChar * fWidthAdj, units.fOne, _colBorder);
  DrawString(fCol + fAdv, fRow, strValue, colScore, 1.0f);
  DrawIcon(fCol, fRow, tex.toFrags, (_bTSETheme ? C_WHITE : colScore), 1.0f, FALSE);

  // Deathmatch
  if (eMode == E_GM_SCORE || eMode == E_GM_FRAG) {
    strValue.PrintF("%d", iMana);
    fCol = _vpixTL(1) + units.fHalf;
    fRow = _vpixTL(2) + units.fNext + units.fHalf;
    fAdv = units.fAdv + units.fChar * fWidthAdj * 0.5f - units.fHalf;

    DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
    DrawBorder(fCol + fAdv, fRow, units.fChar * fWidthAdj, units.fOne, _colBorder);
    DrawString(fCol + fAdv, fRow, strValue, colMana, 1.0f);
    DrawIcon(fCol, fRow, tex.toDeaths, (_bTSETheme ? C_WHITE : colMana), 1.0f, FALSE);

  // Singleplayer or cooperative
  } else if (eMode == E_GM_SP || eMode == E_GM_COOP) {
    // Draw high score
    strValue.PrintF("%d", Max(_penPlayer->m_iHighScore, _penPlayer->m_psGameStats.ps_iScore));
    BOOL bBeating = _penPlayer->m_psGameStats.ps_iScore>_penPlayer->m_iHighScore;

    fCol = 320.0f - units.fOne - units.fChar * 4;
    fRow = _vpixTL(2) + units.fHalf;
    fAdv = units.fAdv + units.fChar * 4 - units.fHalf;

    DrawBorder(fCol, fRow, units.fOne, units.fOne, _colBorder);
    DrawBorder(fCol + fAdv, fRow, units.fChar * 8, units.fOne, _colBorder);
    DrawString(fCol + fAdv, fRow, strValue, NONE, bBeating ? 0.0f : 1.0f);
    DrawIcon(fCol, fRow, tex.toHiScore, _colIconStd, 1.0f, FALSE);

    // Draw unread messages
    if (bShowMessages && _penPlayer->m_ctUnreadMessages > 0) {
      strValue.PrintF("%d", _penPlayer->m_ctUnreadMessages);

      fCol = _vpixBR(1) - units.fHalf - units.fAdv - units.fChar * 4;
      fRow = _vpixTL(2) + units.fHalf;
      fAdv = units.fAdv + units.fChar * 2 - units.fHalf;

      const FLOAT tmIn = 0.5f;
      const FLOAT tmOut = 0.5f;
      const FLOAT tmStay = 2.0f;
      FLOAT tmDelta = _pTimer->GetLerpedCurrentTick()-_penPlayer->m_tmAnimateInbox;
      COLOR col = _colHUD;

      if (tmDelta > 0 && tmDelta < (tmIn + tmStay + tmOut) && eMode == E_GM_SP) {
        FLOAT fRatio = 1.0f;

        if (tmDelta < tmIn) {
          fRatio = tmDelta / tmIn;

        } else if (tmDelta > tmIn + tmStay) {
          fRatio = (tmIn + tmStay + tmOut - tmDelta) / tmOut;
        }

        fCol -= units.fAdv * 15 * fRatio;
        fRow += units.fAdv * 5 * fRatio;
        col = LerpColor(_colHUD, C_WHITE | CT_OPAQUE, fRatio);
      }

      DrawBorder(fCol, fRow, units.fOne, units.fOne, col);
      DrawBorder(fCol + fAdv, fRow, units.fChar*4, units.fOne, col);
      DrawString(fCol + fAdv, fRow, strValue, col, 1.0f);
      DrawIcon(fCol, fRow, tex.toMessage, (_bTSETheme ? C_WHITE : col), 0.0f, TRUE);
    }
  }
};

void CHud::RenderCheats(void) {
  // Render active cheats while in singleplayer
  if (GetSP()->sp_ctMaxPlayers != 1) return;

  ULONG ulAlpha = sin(_tmNow * 16) * 96 + 128;
  PIX pixFontHeight = _pfdConsoleFont->fd_pixCharHeight;
  const COLOR colCheat = (_bTSEColors ? _colHUDText : _colHUD) | ulAlpha;

  _pdp->SetFont(_pfdConsoleFont);
  _pdp->SetTextScaling( 1.0f);

  static CSymbolPtr pbGod("cht_bGod");
  static CSymbolPtr pbFly("cht_bFly");
  static CSymbolPtr pbGhost("cht_bGhost");
  static CSymbolPtr pbInvisible("cht_bInvisible");
  static CSymbolPtr pfTrans("cht_fTranslationMultiplier");

  INDEX iLine = 1;

  if (pfTrans.GetFloat() > 1.0f) {
    _pdp->PutTextR("turbo", _vScreen(1) - 1, _vScreen(2) - pixFontHeight * (iLine++), colCheat);
  }

  if (pbInvisible.GetIndex()) {
    _pdp->PutTextR("invisible", _vScreen(1) - 1, _vScreen(2) - pixFontHeight * (iLine++), colCheat);
  }

  if (pbGhost.GetIndex()) {
    _pdp->PutTextR("ghost", _vScreen(1) - 1, _vScreen(2) - pixFontHeight * (iLine++), colCheat);
  }

  if (pbFly.GetIndex()) {
    _pdp->PutTextR("fly", _vScreen(1) - 1, _vScreen(2) - pixFontHeight * (iLine++), colCheat);
  }

  if (pbGod.GetIndex()) {
    _pdp->PutTextR("god", _vScreen(1) - 1, _vScreen(2) - pixFontHeight * (iLine++), colCheat);
  }
};