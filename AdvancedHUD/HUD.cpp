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

// Main HUD structure
CHud _HUD;

// Update weapon and ammo tables with current info
void CHud::UpdateWeaponArsenal(void) {
  // Ammo quantities
  GetAmmo()[0].iAmmo    = _penWeapons->m_iShells;
  GetAmmo()[0].iMaxAmmo = _penWeapons->m_iMaxShells;
  GetAmmo()[1].iAmmo    = _penWeapons->m_iBullets;
  GetAmmo()[1].iMaxAmmo = _penWeapons->m_iMaxBullets;
  GetAmmo()[2].iAmmo    = _penWeapons->m_iRockets;
  GetAmmo()[2].iMaxAmmo = _penWeapons->m_iMaxRockets;
  GetAmmo()[3].iAmmo    = _penWeapons->m_iGrenades;
  GetAmmo()[3].iMaxAmmo = _penWeapons->m_iMaxGrenades;

#if SE1_GAME != SS_TFE
  GetAmmo()[4].iAmmo    = _penWeapons->m_iNapalm;
  GetAmmo()[4].iMaxAmmo = _penWeapons->m_iMaxNapalm;
  GetAmmo()[5].iAmmo    = _penWeapons->m_iSniperBullets;
  GetAmmo()[5].iMaxAmmo = _penWeapons->m_iMaxSniperBullets;
#endif

  GetAmmo()[6].iAmmo    = _penWeapons->m_iElectricity;
  GetAmmo()[6].iMaxAmmo = _penWeapons->m_iMaxElectricity;
  GetAmmo()[7].iAmmo    = _penWeapons->m_iIronBalls;
  GetAmmo()[7].iMaxAmmo = _penWeapons->m_iMaxIronBalls;

  // Prepare ammo table for weapon possesion
  for (INDEX i = 0; i < GetAmmo().Count(); i++) {
    GetAmmo()[i].bHasWeapon = FALSE;
  }

  INDEX iAvailableWeapons = _penWeapons->m_iAvailableWeapons;

  // Weapon possesion
  for (INDEX iCheck = 1; iCheck < GetWeapons().Count(); iCheck++) {
    HudWeapon *pInfo = &GetWeapons()[iCheck];

    // Regular weapons
    pInfo->bHasWeapon = iAvailableWeapons & (1 << (pInfo->iWeapon - 1));

    if (pInfo->paiAmmo != NULL) {
      pInfo->paiAmmo->bHasWeapon |= pInfo->bHasWeapon;
    }
  }
};

// Render entire interface
void CHud::DrawHUD(const CPlayer *penCurPl, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penOwner)
{
  static CSymbolPtr pbLatency("hud_bShowLatency");
  static CSymbolPtr pfOpacity("hud_fOpacity");
  static CSymbolPtr pfScaling("hud_fScaling");
  static CSymbolPtr pfWeapons("hud_tmWeaponsOnScreen");

  const FLOAT tmWeaponsOnScreen = pfWeapons.GetFloat();
  const INDEX bShowLatency = pbLatency.GetIndex();

  // No player or no owner for snooping
  if (penCurPl == NULL || penCurPl->GetFlags() & ENF_DELETED) return;
  if (bSnooping && penOwner == NULL) return;

  // Find last values in case of a predictor
  _penLast = (CPlayer *)penCurPl;

  if (penCurPl->IsPredictor()) {
    _penLast = (CPlayer *)_penLast->GetPredicted();
  }

  // Make sure there's an entity
  ASSERT(_penLast != NULL);
  if (_penLast == NULL) return;

  // Player entities
  _penPlayer = (CPlayer *)penCurPl;
  _penWeapons = (CPlayerWeapons *)&*_penPlayer->m_penWeapons;

  // Get drawport with its dimensions
  _pdp = pdpCurrent;
  _vpixScreen = PIX2D(_pdp->GetWidth(), _pdp->GetHeight());

  _tmNow = _pTimer->CurrentTick();

  // Limit scaling
  FLOAT fHudScaling = Clamp(pfScaling.GetFloat(), 0.05f, 2.0f);

  // Set wide adjustment dynamically and apply it to scaling
  _fWideAdjustment = ((FLOAT)_vpixScreen(2) / (FLOAT)_vpixScreen(1)) * (4.0f / 3.0f);
  fHudScaling *= _fWideAdjustment;

  _vScaling(1) = (FLOAT)_vpixScreen(1) / 640.0f;
  _vScaling(2) = (FLOAT)_vpixScreen(2) / (480.0f * _fWideAdjustment);

  // Determine screen edges
  INDEX iScreenEdgeX = ClampDn(_psScreenEdgeX.GetIndex(), (INDEX)0);
  INDEX iScreenEdgeY = ClampDn(_psScreenEdgeY.GetIndex(), (INDEX)0);

  _vpixTL = PIX2D(iScreenEdgeX + 1, iScreenEdgeY + 1);
  _vpixBR = PIX2D(640 - _vpixTL(1), (480 * _fWideAdjustment) - _vpixTL(2));

  _ulAlphaHUD = NormFloatToByte(Clamp(pfOpacity.GetFloat(), 0.0f, 1.0f));

  // Setup HUD theme
  const INDEX iCurrentTheme = Clamp(_psTheme.GetIndex(), (INDEX)0, INDEX(E_HUD_MAX - 1));
  _bTSEColors = (iCurrentTheme > E_HUD_TFE);
  _bTSETheme = (iCurrentTheme >= E_HUD_TSE);

  static const HudColorSet *aColorSets[E_HUD_MAX] = {
    &_hcolTFE, &_hcolWarped, &_hcolTSE, &_hcolSSR,
  };

  pColorSet = aColorSets[iCurrentTheme];

  // Set colors
  _colHUD = COL_Base();
  _colBorder = _colHUD;
  _colIconStd = COL_Icon();

  _colMax = COL_ValueOverTop();
  _colTop = COL_ValueTop();
  _colMid = COL_ValueMid();
  _colLow = COL_ValueLow();

  // Adjust border color during snooping
  if (bSnooping) {
    _colBorder = COL_SnoopingLight();

    // Darken flash and scale
    if (ULONG(_tmNow * 5) & 1) {
      _colBorder = COL_SnoopingDark();
      fHudScaling *= 0.933f;
    }
  }

  // Select current fonts
  _pfdCurrentText = &_afdText[iCurrentTheme];
  _pfdCurrentNumbers = &_afdNumbers[iCurrentTheme];

  // Calculate relative scaling for the text font
  _fTextFontScale = (FLOAT)_pfdDisplayFont->GetHeight() / (FLOAT)_pfdCurrentText->GetHeight();

#if SE1_GAME != SS_TFE
  // Render sniper mask (even while snooping)
  CPlayerWeapons &enMyWeapons = (CPlayerWeapons &)*penOwner->m_penWeapons;

  if (enMyWeapons.m_iCurrentWeapon == WEAPON_SNIPER && enMyWeapons.m_bSniping) {
    DrawSniperMask();
  }
#endif

  // Set font and unit sizes
  _pdp->SetFont(_pfdCurrentNumbers);
  ResetScale(fHudScaling);

  // Render parts of the interface
  SIconTexture *ptoWantedWeapon = NULL;
  SIconTexture *ptoCurrentAmmo = NULL;

  RenderVitals();
  RenderCurrentWeapon(&ptoWantedWeapon, &ptoCurrentAmmo);

  Rescale(0.8f);
  RenderActiveArsenal(ptoCurrentAmmo);
  ResetScale(fHudScaling);

  // If weapon change is in progress
  if (_tmNow - _penWeapons->m_tmWeaponChangeRequired < tmWeaponsOnScreen) {
    // Determine amount of available weapons
    INDEX ctWeapons = 0;

    for (INDEX iCount = 1; iCount < GetWeapons().Count(); iCount++) {
      if (GetWeapons()[iCount].iWeapon != WEAPON_NONE && GetWeapons()[iCount].iWeapon != WEAPON_DOUBLECOLT
       && GetWeapons()[iCount].bHasWeapon) {
        ctWeapons++;
      }
    }

    FLOAT fCol = 320.0f - (ctWeapons * units.fAdv - units.fOne) * 0.5f;
    FLOAT fRow = _vpixBR(2) - units.fHalf - units.fNext * 3;

    // Display all available weapons
    for (INDEX iWeapon = 0; iWeapon < GetWeapons().Count(); iWeapon++) {
      HudWeapon &wiInfo = GetWeapons()[iWeapon];

      // Skip if no weapon
      if (wiInfo.iWeapon == WEAPON_NONE || wiInfo.iWeapon == WEAPON_DOUBLECOLT || !wiInfo.bHasWeapon) {
        continue;
      }

      // Display weapon icon
      COLOR colBorder = COL_WeaponBorder();
      COLOR colIcon = COL_WeaponIcon();

      // No ammo
      if (wiInfo.paiAmmo != NULL && wiInfo.paiAmmo->iAmmo == 0) {
        colBorder = colIcon = COL_WeaponNoAmmo();

      // Selected weapon
      } else if (ptoWantedWeapon == wiInfo.ptoWeapon) {
        colBorder = colIcon = COL_WeaponWanted();
      }

      DrawBorder(fCol, fRow, units.fOne, units.fOne, colBorder);
      DrawIcon(fCol, fRow, *wiInfo.ptoWeapon, colIcon, 1.0f, FALSE);

      // Advance to the next position
      fCol += units.fAdv;
    }
  }

  Rescale(0.5f / _fWideAdjustment);
  RenderBars();
  ResetScale(fHudScaling);

  // Determine current gamemode
  EGameMode eGameMode = E_GM_SP;

  if (!GetSP()->sp_bSinglePlayer) {
    if (GetSP()->sp_bCooperative) {
      eGameMode = E_GM_COOP;

    } else if (!GetSP()->sp_bUseFrags) {
      eGameMode = E_GM_SCORE;

    } else {
      eGameMode = E_GM_FRAG;
    }
  }

  Rescale(0.6f);
  RenderGameModeInfo(eGameMode);
  ResetScale(fHudScaling);

  // Display local client latency
  if (bShowLatency) {
    const FLOAT fTextScale = (_vScaling(1) + 1) * 0.5f * _fTextFontScale;

    _pfdCurrentText->SetFixedWidth();
    _pdp->SetFont(_pfdCurrentText);
    _pdp->SetTextScaling(fTextScale);
    _pdp->SetTextCharSpacing(-2.0f * fTextScale);

    CTString strLatency;
    strLatency.PrintF("%4.0fms", _penPlayer->m_tmLatency * 1000.0f);

    const PIX pixFontHeight = _pfdCurrentText->GetHeight() * fTextScale + fTextScale + 1;
    _pdp->PutTextR(strLatency, _vpixScreen(1), _vpixScreen(2) - pixFontHeight, C_WHITE | CT_OPAQUE);
  }

  // Restore font defaults
  _pfdCurrentText->SetVariableWidth();

  RenderCheats();

#if SE1_GAME == SS_TFE
  // Display real time
  if (_psShowClock.GetIndex()) {
    // Set font
    _pdp->SetFont(_pfdConsoleFont);
    _pdp->SetTextScaling(1.0f);
    _pdp->SetTextAspect(1.0f);

    // Determine time
    time_t iLongTime;
    time(&iLongTime);
    tm *tmNewTime = localtime(&iLongTime);

    CTString strTime;
    strTime.PrintF("%2d:%02d", tmNewTime->tm_hour, tmNewTime->tm_min);

    _pdp->PutTextR(strTime, _vpixScreen(1) - 3, 2, C_lYELLOW | CT_OPAQUE);
  }
#endif

  // Remember current time for the next frame
  _tmLast = _tmNow;
};

// Argument list for the RenderHUD() function
#if SE1_VER < SE1_107
  #define RENDERARGS(_prProjection, _pdp, _vLightDir, _colLight, _colAmbient, _bRenderWeapon, _iEye) \
    CPerspectiveProjection3D &_prProjection, CDrawPort *_pdp, FLOAT3D _vLightDir, COLOR _colLight, COLOR _colAmbient, BOOL _bRenderWeapon

  #define RENDERCALLARGS(_prProjection, _pdp, _vLightDir, _colLight, _colAmbient, _bRenderWeapon, _iEye) \
    _prProjection, _pdp, _vLightDir, _colLight, _colAmbient, _bRenderWeapon

#else
  #define RENDERARGS(_prProjection, _pdp, _vLightDir, _colLight, _colAmbient, _bRenderWeapon, _iEye) \
    CPerspectiveProjection3D &_prProjection, CDrawPort *_pdp, FLOAT3D _vLightDir, COLOR _colLight, COLOR _colAmbient, BOOL _bRenderWeapon, INDEX _iEye

  #define RENDERCALLARGS(_prProjection, _pdp, _vLightDir, _colLight, _colAmbient, _bRenderWeapon, _iEye) \
    _prProjection, _pdp, _vLightDir, _colLight, _colAmbient, _bRenderWeapon, _iEye
#endif

// Original function pointer
static void (CPlayer::*pRenderHUD)(RENDERARGS(pr, pdp, v, colL, colA, b, i)) = NULL;

// Player function patch
class CPlayerPatch : public CPlayer {
  public:
    void P_RenderHUD(RENDERARGS(prProjection, pdp, vLightDir, colLight, colAmbient, bRenderWeapon, iEye));
};

// Initialize everything for drawing the HUD
void CHud::Initialize(void) {
  // Patch HUD rendering function
  pRenderHUD = &CPlayer::RenderHUD;
  GetPluginAPI()->NewPatch(pRenderHUD, &CPlayerPatch::P_RenderHUD, "CPlayer::RenderHUD(...)");

  try {
    // Load fonts for each theme
    for (INDEX iTheme = 0; iTheme < E_HUD_MAX; iTheme++) {
      if (iTheme <= E_HUD_TSE) {
        _afdText[iTheme].Load_t(CTFILENAME("Fonts\\Display3-narrow.fnt"));
        _afdNumbers[iTheme].Load_t(CTFILENAME("Fonts\\Numbers3.fnt"));

      } else {
        _afdText[iTheme].Load_t(CTFILENAME("Fonts\\Rev_HUD\\Cabin.fnt"));
        _afdNumbers[iTheme].Load_t(CTFILENAME("Fonts\\Rev_HUD\\Numbers.fnt"));
      }

      _afdText[iTheme].SetCharSpacing(0);
      _afdText[iTheme].SetLineSpacing(1);
    }

    tex.LoadTextures();

  } catch (char *strError) {
    FatalError(strError);
  }

  // Prepare weapon arsenal
  HudArsenal::AmmoStack &aAmmo = arWeapons.aAmmo;
  #define NEW_AMMO aAmmo.Push().Setup
  #define NEW_WEAPON arWeapons.aWeapons.Push().Setup

  // Added in order of appearance instead of using 'aiAmmoRemap'
  NEW_AMMO(&tex.toAShells);
  NEW_AMMO(&tex.toABullets);
  NEW_AMMO(&tex.toARockets);
  NEW_AMMO(&tex.toAGrenades);
  NEW_AMMO(&tex.toANapalm);
  NEW_AMMO(&tex.toASniperBullets);
  NEW_AMMO(&tex.toAElectricity);
  NEW_AMMO(&tex.toAIronBall);

  // Added in order of appearance instead of using 'aiWeaponRemap'
  NEW_WEAPON(WEAPON_NONE,  NULL);
  NEW_WEAPON(WEAPON_KNIFE, &tex.toWKnife);

  #if SE1_GAME != SS_TFE
    NEW_WEAPON(WEAPON_CHAINSAW, &tex.toWChainsaw, NULL);
  #endif

  NEW_WEAPON(WEAPON_COLT,            &tex.toWColt);
  NEW_WEAPON(WEAPON_DOUBLECOLT,      &tex.toWColt);
  NEW_WEAPON(WEAPON_SINGLESHOTGUN,   &tex.toWSingleShotgun,   &aAmmo[0]);
  NEW_WEAPON(WEAPON_DOUBLESHOTGUN,   &tex.toWDoubleShotgun,   &aAmmo[0]);
  NEW_WEAPON(WEAPON_TOMMYGUN,        &tex.toWTommygun,        &aAmmo[1]);
  NEW_WEAPON(WEAPON_MINIGUN,         &tex.toWMinigun,         &aAmmo[1]);
  NEW_WEAPON(WEAPON_ROCKETLAUNCHER,  &tex.toWRocketLauncher,  &aAmmo[2]);
  NEW_WEAPON(WEAPON_GRENADELAUNCHER, &tex.toWGrenadeLauncher, &aAmmo[3]);

  #if SE1_GAME != SS_TFE
    NEW_WEAPON(WEAPON_FLAMER, &tex.toWFlamer, &aAmmo[4]);
    NEW_WEAPON(WEAPON_SNIPER, &tex.toWSniper, &aAmmo[5]);
  #endif

  NEW_WEAPON(WEAPON_LASER,      &tex.toWLaser,      &aAmmo[6]);
  NEW_WEAPON(WEAPON_IRONCANNON, &tex.toWIronCannon, &aAmmo[7]);
};

void CPlayerPatch::P_RenderHUD(RENDERARGS(prProjection, pdp, vLightDir, colLight, colAmbient, bRenderWeapon, iEye))
{
  // Proceed to the original function instead
  if (!_psEnable.GetIndex()) {
    (this->*pRenderHUD)(RENDERCALLARGS(prProjection, pdp, vLightDir, colLight, colAmbient, bRenderWeapon, iEye));
    return;
  }

  static CSymbolPtr pbRenderModels("gfx_bRenderModels");
  static CSymbolPtr pbShowWeapon("hud_bShowWeapon");
  static CSymbolPtr pbShowInterface("hud_bShowInfo");
  static CSymbolPtr ptmSnoopingTime("plr_tmSnoopingTime");

  // Save intended view placement before rendering the weapon
  const CPlacement3D plViewOld = prProjection.ViewerPlacementR();

  CPlayerWeapons &enWeapons = (CPlayerWeapons &)*m_penWeapons;

#if SE1_GAME == SS_TFE
  const BOOL bSniping = FALSE;
#else
  const BOOL bSniping = enWeapons.m_bSniping;
#endif

  // Don't render the weapon while sniping
  if (pbShowWeapon.GetIndex() && pbRenderModels.GetIndex() && !bSniping) {
    #if SE1_VER < SE1_107
      enWeapons.RenderWeaponModel(prProjection, pdp, vLightDir, colLight, colAmbient, bRenderWeapon);
    #else
      enWeapons.RenderWeaponModel(prProjection, pdp, vLightDir, colLight, colAmbient, bRenderWeapon, iEye);
    #endif
  }

  if (m_iViewState == PVT_PLAYEREYES) {
    prProjection.ViewerPlacementL() = plViewOld;
    prProjection.Prepare();

  #if SE1_GAME != SS_TFE
    CAnyProjection3D apr;
    apr = prProjection;
    Stereo_AdjustProjection(*apr, iEye, 1);

    Particle_PrepareSystem(pdp, apr);
    Particle_PrepareEntity(2.0f, FALSE, FALSE, this);
    RenderChainsawParticles(FALSE);
    Particle_EndSystem();
  #endif
  }

  CPlacement3D plView;

  // Player view
  if (m_iViewState == PVT_PLAYEREYES) {
    plView = en_plViewpoint;
    plView.RelativeToAbsolute(GetPlacement());

  // Camera view
  } else if (m_iViewState == PVT_3RDPERSONVIEW) {
    plView = m_pen3rdPersonView->GetPlacement();
  }

  // Render crosshair while not sniping
  if (!bSniping) {
    enWeapons.RenderCrosshair(prProjection, pdp, plView);
  }

  // Toggleable red screen on damage
  static CSymbolPtr pbRedScreen("axs_bRedScreenOnDamage");

  if (!pbRedScreen.Exists() || pbRedScreen.GetIndex()) {
    // Do screen blending for wounding
    CPlayer *pen = (CPlayer *)GetPredictionTail();
    ULONG ulA = pen->m_fDamageAmmount * 5.0f;

    FLOAT tmSinceWounding = _pTimer->CurrentTick() - pen->m_tmWoundedTime;

    if (tmSinceWounding < 0.001f) {
      ulA = (ulA + 64) / 2;
    }

    // Add rest of the blend amount
    ulA = ClampUp(ulA, (ULONG)224);

    if (m_iViewState == PVT_PLAYEREYES) {
      pdp->dp_ulBlendingRA += 255 * ulA;
      pdp->dp_ulBlendingA += ulA;
    }
  }

  // Add world glaring
  {
    COLOR colGlare = GetWorldGlaring();
    UBYTE ubR, ubG, ubB, ubA;
    ColorToRGBA(colGlare, ubR, ubG, ubB, ubA);

    if (ubA != 0) {
      pdp->dp_ulBlendingRA += ULONG(ubR) * ULONG(ubA);
      pdp->dp_ulBlendingGA += ULONG(ubG) * ULONG(ubA);
      pdp->dp_ulBlendingBA += ULONG(ubB) * ULONG(ubA);
      pdp->dp_ulBlendingA  += ULONG(ubA);
    }
  }

  // Do all queued screen blendings
  pdp->BlendScreen();

  // Interface is hidden
  if (!pbShowInterface.GetIndex()) return;

  // Get snooping time
  const FLOAT tmSnooping = ptmSnoopingTime.Exists() ? ptmSnoopingTime.GetFloat() : 1.0f;

  // Get player or the predictor
  BOOL bSnooping = FALSE;
  CPlayer *penHUDPlayer = this;
  CPlayer *penHUDOwner = this;

  if (penHUDPlayer->IsPredicted()) {
    penHUDPlayer = (CPlayer *)penHUDPlayer->GetPredictor();
  }

  // Check if snooping is needed
  TIME tmDelta = _pTimer->CurrentTick() - enWeapons.m_tmSnoopingStarted;

  if (tmDelta < tmSnooping) {
    ASSERT(enWeapons.m_penTargeting != NULL);
    penHUDPlayer = (CPlayer *)&*enWeapons.m_penTargeting;
    bSnooping = TRUE;
  }

  _HUD.DrawHUD(penHUDPlayer, pdp, bSnooping, penHUDOwner);
};
