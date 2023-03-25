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

#ifndef CECIL_INCL_TEXTURESET_H
#define CECIL_INCL_TEXTURESET_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

// Available HUD themes
enum EHudTheme {
  E_HUD_TFE,
  E_HUD_WARPED,
  E_HUD_TSE,

  E_HUD_MAX, // Maximum amount of themes
};

#define MAX_POWERUPS 4

// Multi-theme container for icons
struct SIconTexture {
  CTextureObject ato[E_HUD_MAX];

  // Set icon texture for a specific theme
  void SetIcon(INDEX iTheme, const CTString &strTexture) {
    ato[iTheme].SetData_t(strTexture);
    ((CTextureData *)ato[iTheme].GetData())->Force(TEX_CONSTANT);
  };

  // Return texture depending on the theme
  inline CTextureObject &Texture(void) {
    INDEX iTheme = Clamp(_psTheme.GetIndex(), (INDEX)0, INDEX(E_HUD_MAX - 1));
    return ato[iTheme];
  };

  // Implicit conversion
  inline operator CTextureObject &() {
    return Texture();
  };
};

struct HudTextureSet {
  // Status bar
  SIconTexture toHealth;
  SIconTexture toOxygen;
  SIconTexture toScore;
  SIconTexture toHiScore;
  SIconTexture toMessage;
  SIconTexture toFrags;
  SIconTexture toDeaths;
  SIconTexture atoArmor[3];

  // Ammo
  SIconTexture toAShells;
  SIconTexture toABullets;
  SIconTexture toARockets;
  SIconTexture toAGrenades;
  SIconTexture toAElectricity;
  SIconTexture toAIronBall;

  SIconTexture toANapalm;
  SIconTexture toASniperBullets;
  SIconTexture toASeriousBomb;

  // Weapons
  SIconTexture toWKnife;
  SIconTexture toWColt;
  SIconTexture toWSingleShotgun;
  SIconTexture toWDoubleShotgun;
  SIconTexture toWTommygun;
  SIconTexture toWMinigun;
  SIconTexture toWRocketLauncher;
  SIconTexture toWGrenadeLauncher;
  SIconTexture toWLaser;
  SIconTexture toWIronCannon;

  SIconTexture toWChainsaw;
  SIconTexture toWFlamer;
  SIconTexture toWSniper;

#if SE1_GAME != SS_TFE
  // Powerups
  SIconTexture atoPowerups[MAX_POWERUPS];

  // Sniper mask
  CTextureObject toSniperMask;
  CTextureObject toSniperWheel;
  CTextureObject toSniperArrow;
  CTextureObject toSniperEye;
  CTextureObject toSniperLed;
#endif

  // Tile texture with one corner, edges and center
  CTextureObject toTile;

  void LoadTextures(void) {
    // Directories with themed icons
    static const CTString astrPaths[E_HUD_MAX] = {
      "Textures\\Interface\\",
      "Textures\\Interface\\",
      "TexturesMP\\Interface\\",
    };

    for (INDEX iTheme = 0; iTheme < E_HUD_MAX; iTheme++) {
      const CTString &strPath = astrPaths[iTheme];
      const BOOL bTFE = (iTheme <= E_HUD_WARPED);

      // Status bar textures
      toHealth   .SetIcon(iTheme, strPath + "HSuper.tex");
      toOxygen   .SetIcon(iTheme, strPath + "Oxygen-2.tex");
      toFrags    .SetIcon(iTheme, strPath + "IBead.tex");
      toDeaths   .SetIcon(iTheme, strPath + "ISkull.tex");
      toScore    .SetIcon(iTheme, strPath + "IScore.tex");
      toHiScore  .SetIcon(iTheme, strPath + "IHiScore.tex");
      toMessage  .SetIcon(iTheme, strPath + "IMessage.tex");
      atoArmor[0].SetIcon(iTheme, strPath + (bTFE ? "ArStrong.tex" : "ArSmall.tex"));
      atoArmor[1].SetIcon(iTheme, strPath + (bTFE ? "ArStrong.tex" : "ArMedium.tex"));
      atoArmor[2].SetIcon(iTheme, strPath + "ArStrong.tex");

      // Ammo textures
      toAShells     .SetIcon(iTheme, strPath + "AmShells.tex");
      toABullets    .SetIcon(iTheme, strPath + "AmBullets.tex");
      toARockets    .SetIcon(iTheme, strPath + "AmRockets.tex");
      toAGrenades   .SetIcon(iTheme, strPath + "AmGrenades.tex");
      toAElectricity.SetIcon(iTheme, strPath + "AmElectricity.tex");
      toAIronBall   .SetIcon(iTheme, strPath + (bTFE ? "AmCannon.tex" : "AmCannonBall.tex"));

      // Weapon textures
      toWKnife          .SetIcon(iTheme, strPath + "WKnife.tex");
      toWColt           .SetIcon(iTheme, strPath + "WColt.tex");
      toWSingleShotgun  .SetIcon(iTheme, strPath + "WSingleShotgun.tex");
      toWDoubleShotgun  .SetIcon(iTheme, strPath + "WDoubleShotgun.tex");
      toWTommygun       .SetIcon(iTheme, strPath + "WTommygun.tex");
      toWMinigun        .SetIcon(iTheme, strPath + "WMinigun.tex");
      toWRocketLauncher .SetIcon(iTheme, strPath + "WRocketLauncher.tex");
      toWGrenadeLauncher.SetIcon(iTheme, strPath + "WGrenadeLauncher.tex");
      toWLaser          .SetIcon(iTheme, strPath + "WLaser.tex");
      toWIronCannon     .SetIcon(iTheme, strPath + "WCannon.tex");

    #if SE1_GAME != SS_TFE
      // Ammo textures
      toANapalm       .SetIcon(iTheme, strPath + "AmFuelReservoir.tex");
      toASniperBullets.SetIcon(iTheme, strPath + "AmSniperBullets.tex");

      // Weapon textures
      toWChainsaw.SetIcon(iTheme, strPath + "WChainsaw.tex");
      toWSniper  .SetIcon(iTheme, strPath + "WSniper.tex");
      toWFlamer  .SetIcon(iTheme, strPath + "WFlamer.tex");

      // Power up textures
      const CTString &strPowerUp = (bTFE ? astrPaths[E_HUD_TSE] : strPath);

      atoPowerups[0].SetIcon(iTheme, strPowerUp + "PInvisibility.tex");
      atoPowerups[1].SetIcon(iTheme, strPowerUp + "PInvulnerability.tex");
      atoPowerups[2].SetIcon(iTheme, strPowerUp + "PSeriousDamage.tex");
      atoPowerups[3].SetIcon(iTheme, strPowerUp + "PSeriousSpeed.tex");
      toASeriousBomb.SetIcon(iTheme, strPowerUp + "AmSeriousBomb.tex");
    #endif
    }

    // Sniper mask textures for TSE
    #if SE1_GAME != SS_TFE
      toSniperMask.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperMask.tex"));
      toSniperWheel.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperWheel.tex"));
      toSniperArrow.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperArrow.tex"));
      toSniperEye.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperEye.tex"));
      toSniperLed.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperLed.tex"));

      ((CTextureData *)toSniperMask.GetData())->Force(TEX_CONSTANT);
      ((CTextureData *)toSniperWheel.GetData())->Force(TEX_CONSTANT);
      ((CTextureData *)toSniperArrow.GetData())->Force(TEX_CONSTANT);
      ((CTextureData *)toSniperEye.GetData())->Force(TEX_CONSTANT);
      ((CTextureData *)toSniperLed.GetData())->Force(TEX_CONSTANT);
    #endif

    // Tile texture
    toTile.SetData_t(CTFILENAME("Textures\\Interface\\Tile.tex"));
    ((CTextureData *)toTile.GetData())->Force(TEX_CONSTANT);
  };
};

#endif
