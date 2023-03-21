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

#ifndef CECIL_INCL_ADVANCEDHUD_H
#define CECIL_INCL_ADVANCEDHUD_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "TextureSet.h"
#include "WeaponArsenal.h"

#include <EntitiesV/StdH/StdH.h>
#include <EntitiesV/PlayerWeapons.h>

class CHud {
  public:
    // Keys for sorting player statistics
    enum ESortKeys {
      E_SK_NAME   = 0,
      E_SK_HEALTH = 1,
      E_SK_SCORE  = 2,
      E_SK_MANA   = 3, 
      E_SK_FRAGS  = 4,
      E_SK_DEATHS = 5,
    };

    // Progress bar direction
    enum EBarDir {
      E_BD_LEFT  = 1,
      E_BD_RIGHT = 2, 
      E_BD_UP    = 3,
      E_BD_DOWN  = 4,
    };

  public:
    // Player entities
    const CPlayer *_penPlayer;
    CPlayer *_penLast;
    CPlayerWeapons *_penWeapons;

    // Drawing variables
    CDrawPort *_pdp;
    PIX2D _vScreen;
    FLOAT2D _vScaling; // Scaling factors taking aspect ratio in consideration
    FLOAT _fWideAdjustment;
    FLOAT _fCustomScaling;

    ULONG _ulAlphaHUD;
    BOOL _bTSEColors;
    BOOL _bTSETheme;

    // HUD boundaries
    PIX2D _vpixTL;
    PIX2D _vpixBR;

    // Colors
    COLOR _colHUD;
    COLOR _colHUDText;
    COLOR _colMax;
    COLOR _colTop;
    COLOR _colMid;
    COLOR _colIconStd;
    COLOR _colBorder;

    // Other
    TIME _tmNow;
    TIME _tmLast;
    CFontData _fdNumbersFont;

    struct Units {
      FLOAT fChar;
      FLOAT fOne;
      FLOAT fAdv;
      FLOAT fNext;
      FLOAT fHalf;
    } units;

    // Array for pointers of all players
    CDynamicContainer<CPlayer> _cenPlayers;

    // Information about color transitions
    struct ColorTransitionTable {
      COLOR ctt_colFine;     // Color for values over 1.0
      COLOR ctt_colHigh;     // Color for values from 1.0 to 'fMedium'
      COLOR ctt_colMedium;   // Color for values from 'fMedium' to 'fLow'
      COLOR ctt_colLow;      // Color for values under fLow
      FLOAT ctt_fMediumHigh; // When to switch to high color   (normalized float!)
      FLOAT ctt_fLowMedium;  // When to switch to medium color (normalized float!)
      BOOL  ctt_bSmooth;     // Should colors have smooth transition
    } _cttHUD;

    HudTextureSet tex;
    HudArsenal arWeapons;

  public:
    CHud() {
      _tmNow = -1.0f;
      _tmLast = -1.0f;
    };

    // Get ammo from the arsenal
    inline HudArsenal::AmmoStack &GetAmmo(void) {
      return arWeapons.aAmmo;
    };

    // Get weapons from the arsenal
    inline HudArsenal::WeaponStack &GetWeapons(void) {
      return arWeapons.aWeapons;
    };

    // Find weapon info from weapon type
    BOOL GetWeaponInfo(HudWeapon **ppInfo, INDEX iWeapon) {
      for (INDEX i = 0; i < GetWeapons().Count(); i++) {
        if (GetWeapons()[i].iWeapon == iWeapon) {
          *ppInfo = &GetWeapons()[i];
          return TRUE;
        }
      }

      return FALSE;
    };

    // Reset scaling and unit sizes
    inline void ResetScale(FLOAT fScale) {
      _fCustomScaling = fScale;

      const PIX pixChar = _fdNumbersFont.GetWidth() + _fdNumbersFont.GetCharSpacing() + 1;
      units.fChar = pixChar * fScale;

      units.fOne  = 32 * fScale;
      units.fAdv  = 36 * fScale;
      units.fNext = 40 * fScale;
      units.fHalf = units.fOne * 0.5f;
    };

    // Scale unit sizes by a factor
    inline void Rescale(FLOAT fFactor) {
      const FLOAT fUpperSize = (_fCustomScaling * fFactor) / _fCustomScaling;
      _fCustomScaling *= fUpperSize;

      units.fChar *= fUpperSize;
      units.fOne  *= fUpperSize;
      units.fAdv  *= fUpperSize;
      units.fNext *= fUpperSize;
      units.fHalf *= fUpperSize;
    };

  // Main methods
  public:

    // Prepare color transitions
    void PrepareColorTransitions(COLOR colFine, COLOR colHigh, COLOR colMedium, COLOR colLow,
      FLOAT fMediumHigh, FLOAT fLowMedium, BOOL bSmooth);

    // Calculate shake amount and color value depending on value change
    COLOR AddShaker(const PIX pixAmount, const INDEX iCurrentValue, INDEX &iLastValue,
                    TIME &tmChanged, FLOAT &fMoverX, FLOAT &fMoverY);

    // Get current color from local color transitions table
    COLOR GetCurrentColor(FLOAT fNormValue);

    // Fill array with player statistics
    void SetAllPlayersStats(INDEX iSortKey);

    // Update weapon and ammo tables with current info
    void UpdateWeaponArsenal(void);

    // Render entire interface
    void DrawHUD(const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner);
    
    // Initialize everything for drawing the HUD
    void Initialize(void);

  // Drawing functions
  public:

    // Draw border using a tile texture
    void DrawBorder(FLOAT fX, FLOAT fY, FLOAT fW, FLOAT fH, COLOR colTiles);

    // Draw icon texture
    void DrawIcon(FLOAT fX, FLOAT fY, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink);

    // Draw text
    void DrawString(FLOAT fX, FLOAT fY, const CTString &strText, COLOR colDefault, FLOAT fNormValue);

    // Draw percentage bar
    void DrawBar(FLOAT fX, FLOAT fY, PIX pixW, PIX pixH, EBarDir eBarDir, COLOR colDefault, FLOAT fNormValue);

    // Draw texture rotated at a certain angle
    void DrawRotatedQuad(CTextureObject *pto, FLOAT fX, FLOAT fY, FLOAT fSize, ANGLE aAngle, COLOR col);

    // Draw texture with a correct aspect ratio
    void DrawCorrectTexture(CTextureObject *pto, FLOAT fX, FLOAT fY, FLOAT fWidth, COLOR col);

  #if SE1_GAME != SS_TFE
    // Draw sniper mask
    void DrawSniperMask(void);
  #endif

  // Parts of the HUD
  public:

    enum EGameMode {
      E_GM_SP,
      E_GM_COOP,
      E_GM_SCORE,
      E_GM_FRAG,
    };

    void RenderVitals(void);
    void RenderCurrentWeapon(SIconTexture **pptoWantedWeapon, SIconTexture **pptoCurrentAmmo);
    void RenderActiveArsenal(SIconTexture *ptoAmmo);
    void RenderBars(void);
    void RenderGameModeInfo(EGameMode eMode);
    void RenderCheats(void);
};

// Main HUD structure
extern CHud _HUD;

// Choose colors based on the selected color scheme
#define HUD_COL_LIGHT_TEXT  (_bTSEColors ? 0xFFD70000 : C_GREEN)
#define HUD_COL_DARK_BORDER (_bTSEColors ? 0x9B4B0000 : C_dGREEN)
#define HUD_COL_MAIN        (_bTSEColors ? 0x4C80BB00 : C_GREEN)
#define HUD_COL_MASK        (_bTSEColors ? 0x64B4FF00 : C_GREEN)
#define HUD_COL_OVERTOP     (_bTSEColors ? 0x6CFF6C00 : C_GREEN)

#endif
