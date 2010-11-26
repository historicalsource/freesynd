/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *  but WITHOUT  ANY WARRANTY;  without even  the implied  warranty of  *
 *  MERCHANTABILITY  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *  General Public License for more details.                            *
 *                                                                      *
 *    You can view the GNU  General Public License, online, at the GNU  *
 *  project's  web  site;  see <http://www.gnu.org/licenses/gpl.html>.  *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "loadingmenu.h"

LoadingMenu::LoadingMenu(MenuManager * m):Menu(m, "loading", "", ""),
tick_count_(-500)
{
    std::string str;
    menu_manager_->getMessage("LDGAME_TITLE", str);
    int titleX = g_Screen.gameScreenWidth() / 2  - g_App.fonts().textWidth(str.c_str(), FontManager::SIZE_4) / 2;
    addStatic(titleX, 180, str.c_str(), 3, false);
}

void LoadingMenu::handleTick(int elapsed)
{
    tick_count_ += elapsed;
    if (tick_count_ >= 2000) {
        g_Session.getMission()->setSurfaces();
        menu_manager_->changeCurrentMenu("Gameplay");
        tick_count_ = -500;
    }
    assert(tick_count_ < 2000);
}
