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

// Utilize the API
#include <CoreLib/API/CoreAPI.h>

#include <CoreLib/Compatibility/Game.h>
#include <CoreLib/Patcher/FuncPatching.h>

extern CPluginSymbol _psEnable;
extern CPluginSymbol _psTheme;

extern CPluginSymbol _psScreenEdgeX;
extern CPluginSymbol _psScreenEdgeY;
extern CPluginSymbol _psScopeAlpha;

#if SE1_GAME == SS_TFE
  // TFE specific
  extern CPluginSymbol _psShowClock;
  extern CPluginSymbol _psShowMatchInfo;

#else
  // TSE specific
  extern CPluginSymbol _psScopeAlpha;
#endif

extern CPluginSymbol _psShowPlayerPing;
extern CPluginSymbol _psShowAmmoRow;

extern CPluginSymbol _psColorize;
extern CPluginSymbol _psColorBase;
extern CPluginSymbol _psColorIcon;
extern CPluginSymbol _psColorWeapon;
extern CPluginSymbol _psColorSelect;
extern CPluginSymbol _psColorMax;
extern CPluginSymbol _psColorTop;
extern CPluginSymbol _psColorMid;
extern CPluginSymbol _psColorLow;

// Entities library dependency
#ifdef _DEBUG
  #pragma comment(lib, "EntitiesVD.lib")
#else
  #pragma comment(lib, "EntitiesV.lib")
#endif
