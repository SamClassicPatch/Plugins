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

// Base colors
COLOR CHud::COL_Base(void) {
  return _bTSEColors ? 0x4C80BB00 : C_GREEN;
};

COLOR CHud::COL_SnoopingLight(void) {
  if (_bTSEColors) return 0xEE9C0000;

  // Shift and swap color components
  UBYTE ubR, ubG, ubB;
  ColorToRGB(_colBorder, ubR, ubG, ubB);
  return RGBToColor(ubG, ubB, ubR);
};

COLOR CHud::COL_SnoopingDark(void) {
  return _bTSEColors ? 0x9B4B0000 : (_colBorder >> 1) & 0x7F7F7F00;
};

COLOR CHud::COL_TextLight(void) {
  return _bTSEColors ? 0xFFD70000 : C_GREEN;
};

COLOR CHud::COL_TextOverTop(void) {
  return _bTSEColors ? 0x6CFF6C00 : C_GREEN;
};

// Sniper scope
COLOR CHud::COL_ScopeMask(void) {
  return _bTSEColors ? 0x64B4FF00 : C_GREEN;
};

COLOR CHud::COL_ScopeDetails(void) {
  return _bTSEColors ? 0xFFCC3300 : C_GREEN;
};

COLOR CHud::COL_ScopeLedIdle(void) {
  return 0x44FF22BB;
};

COLOR CHud::COL_ScopeLedFire(void) {
  return 0xFF4422DD;
};

// Weapon selection list
COLOR CHud::COL_WeaponBorder(void) {
  return _colHUD;
};

COLOR CHud::COL_WeaponIcon(void) {
  return _bTSEColors ? 0xCCDDFF00 : _colHUD;
};

COLOR CHud::COL_WeaponNoAmmo(void) {
  return _bTSEColors ? 0x22334400 : C_dGRAY;
};

COLOR CHud::COL_WeaponWanted(void) {
  return _bTSEColors ? 0xFFCC0000 : C_WHITE;
};
