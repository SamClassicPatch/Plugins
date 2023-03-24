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

#define MAX_POWERUPS 4

// Container for both TFE and TSE icons
struct SIconTexture {
  CTextureObject toTFE;

#if TSE_THEME_ENABLED
  CTextureObject toTSE;
#endif

  // Wrapper for the CTextureObject method
  void SetData_t(const CTString &strTFE, const CTString &strTSE = "") {
    toTFE.SetData_t("Textures\\Interface\\" + strTFE);
    ((CTextureData *)toTFE.GetData())->Force(TEX_CONSTANT);

  #if TSE_THEME_ENABLED
    // Take TFE texture if TSE isn't specified
    toTSE.SetData_t("TexturesMP\\Interface\\" + (strTSE == "" ? strTFE : strTSE));
    ((CTextureData *)toTSE.GetData())->Force(TEX_CONSTANT);
  #endif
  };

  // Return TFE or TSE texture depending on the theme
  operator CTextureObject &() {
  #if TSE_THEME_ENABLED
    if (_psTheme.GetIndex() > 1) {
      return toTSE;
    }
  #endif

    return toTFE;
  };
};

struct HudTextureSet {
  // Status bar
  SIconTexture toHealth;
  SIconTexture toOxygen;
  SIconTexture toScore;
  SIconTexture toHiScore;
  SIconTexture toMessage;
  SIconTexture toMana;
  SIconTexture toFrags;
  SIconTexture toDeaths;
  CTextureObject atoArmor[4];

  // Ammo
  SIconTexture toAShells;
  SIconTexture toABullets;
  SIconTexture toARockets;
  SIconTexture toAGrenades;
  SIconTexture toANapalm;
  SIconTexture toAElectricity;
  SIconTexture toAIronBall;
  SIconTexture toASniperBullets;
  CTextureObject toASeriousBomb;

  // Weapons
  SIconTexture toWKnife;
  SIconTexture toWColt;
  SIconTexture toWSingleShotgun;
  SIconTexture toWDoubleShotgun;
  SIconTexture toWTommygun;
  SIconTexture toWSniper;
  SIconTexture toWChainsaw;
  SIconTexture toWMinigun;
  SIconTexture toWRocketLauncher;
  SIconTexture toWGrenadeLauncher;
  SIconTexture toWFlamer;
  SIconTexture toWLaser;
  SIconTexture toWIronCannon;

  // Power-ups
  CTextureObject atoPowerups[MAX_POWERUPS];

  // Tile texture with one corner, edges and center
  CTextureObject toTile;

  // Sniper mask
  CTextureObject toSniperMask;
  CTextureObject toSniperWheel;
  CTextureObject toSniperArrow;
  CTextureObject toSniperEye;
  CTextureObject toSniperLed;

  void LoadTextures(void) {
    // Status bar textures
    toHealth.SetData_t("HSuper.tex");
    toOxygen.SetData_t("Oxygen-2.tex");
    toFrags.SetData_t("IBead.tex");
    toDeaths.SetData_t("ISkull.tex");
    toScore.SetData_t("IScore.tex");
    toHiScore.SetData_t("IHiScore.tex");
    toMessage.SetData_t("IMessage.tex");
    toMana.SetData_t("IValue.tex");
    atoArmor[0].SetData_t(CTFILENAME("Textures\\Interface\\ArStrong.tex"));

    // Ammo textures                    
    toAShells.SetData_t("AmShells.tex");
    toABullets.SetData_t("AmBullets.tex");
    toARockets.SetData_t("AmRockets.tex");
    toAGrenades.SetData_t("AmGrenades.tex");
    toAElectricity.SetData_t("AmElectricity.tex");
    toAIronBall.SetData_t("AmCannon.tex", "AmCannonBall.tex");

    // Weapon textures
    toWKnife.SetData_t("WKnife.tex");
    toWColt.SetData_t("WColt.tex");
    toWSingleShotgun.SetData_t("WSingleShotgun.tex");
    toWDoubleShotgun.SetData_t("WDoubleShotgun.tex");
    toWTommygun.SetData_t("WTommygun.tex");
    toWMinigun.SetData_t("WMinigun.tex");
    toWRocketLauncher.SetData_t("WRocketLauncher.tex");
    toWGrenadeLauncher.SetData_t("WGrenadeLauncher.tex");
    toWLaser.SetData_t("WLaser.tex");
    toWIronCannon.SetData_t("WCannon.tex");

    // TSE exclusive
    #if TSE_THEME_ENABLED
      // Status bar textures
      atoArmor[1].SetData_t(CTFILENAME("TexturesMP\\Interface\\ArSmall.tex"));
      atoArmor[2].SetData_t(CTFILENAME("TexturesMP\\Interface\\ArMedium.tex"));
      atoArmor[3].SetData_t(CTFILENAME("TexturesMP\\Interface\\ArStrong.tex"));
    #endif

    #if SE1_GAME != SS_TFE
      // Ammo textures
      toANapalm.SetData_t("AmFuelReservoir.tex");
      toASniperBullets.SetData_t("AmSniperBullets.tex");
      toASeriousBomb.SetData_t(CTFILENAME("TexturesMP\\Interface\\AmSeriousBomb.tex"));

      // Weapon textures
      toWChainsaw.SetData_t("WChainsaw.tex");
      toWSniper.SetData_t("WSniper.tex");
      toWFlamer.SetData_t("WFlamer.tex");

      // Power up textures
      atoPowerups[0].SetData_t(CTFILENAME("TexturesMP\\Interface\\PInvisibility.tex"));
      atoPowerups[1].SetData_t(CTFILENAME("TexturesMP\\Interface\\PInvulnerability.tex"));
      atoPowerups[2].SetData_t(CTFILENAME("TexturesMP\\Interface\\PSeriousDamage.tex"));
      atoPowerups[3].SetData_t(CTFILENAME("TexturesMP\\Interface\\PSeriousSpeed.tex"));

      // Sniper mask textures
      toSniperMask.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperMask.tex"));
      toSniperWheel.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperWheel.tex"));
      toSniperArrow.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperArrow.tex"));
      toSniperEye.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperEye.tex"));
      toSniperLed.SetData_t(CTFILENAME("TexturesMP\\Interface\\SniperLed.tex"));
    #endif

    // Tile texture
    toTile.SetData_t(CTFILENAME("Textures\\Interface\\Tile.tex"));

    // Set all textures as constant
    #define MAKE_CONSTANT(_TextureObject) ((CTextureData *)_TextureObject.GetData())->Force(TEX_CONSTANT)
    
    MAKE_CONSTANT(toTile);
    MAKE_CONSTANT(atoArmor[0]);

    #if TSE_THEME_ENABLED
      MAKE_CONSTANT(atoArmor[1]);
      MAKE_CONSTANT(atoArmor[2]);
      MAKE_CONSTANT(atoArmor[3]);
    #endif
      
    #if SE1_GAME != SS_TFE
      MAKE_CONSTANT(atoPowerups[0]);
      MAKE_CONSTANT(atoPowerups[1]);
      MAKE_CONSTANT(atoPowerups[2]);
      MAKE_CONSTANT(atoPowerups[3]);

      MAKE_CONSTANT(toSniperMask);
      MAKE_CONSTANT(toSniperWheel);
      MAKE_CONSTANT(toSniperArrow);
      MAKE_CONSTANT(toSniperEye);
      MAKE_CONSTANT(toSniperLed);
    #endif
  };
};

#endif
