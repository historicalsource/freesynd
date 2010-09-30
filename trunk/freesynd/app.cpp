/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "app.h"
#include "file.h"
#include "fliplayer.h"

#ifdef SYSTEM_SDL
#include "system_sdl.h"
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <set>

#include "fliplayer.h"
#include "spritemanager.h"
#include "screen.h"
#include "audio.h"
#include "utils/log.h"

App::App(): running_(true), playingFli_(false),
skipFli_(false), screen_(new Screen(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT))
#ifdef SYSTEM_SDL
    , system_(new SystemSDL())
#else
#error A suitable System object has not been defined!
#endif
{
    agents_.loadAgents();
    weapons_.loadWeapons();
    mods_.loadMods();
    vehicles_.loadVehicles();
    peds_.loadPeds();
    intro_sounds_.loadSounds(SoundManager::SAMPLES_INTRO);
    game_sounds_.loadSounds(SoundManager::SAMPLES_GAME);
    music_.loadMusic();

    reset();
}

App::~App() {

}

/*!
 * Activate cheat mode in which all completed missions can be replayed.
 */
void App::cheatRepeatOrCompleteMission() {
    session_.cheatReplayMission();
}

void App::cheatWeaponsAndMods() {
    available_weapons_.clear();
    available_mods_.clear();

    for (int i = 0; i < weapons_.numWeapons(); i++)
        available_weapons_.push_back(weapons_.weapon(i));

    for (int i = 0; i < 6; i++) {
        available_mods_.push_back(mods_.mod(i));
        available_mods_.push_back(mods_.mod(i, 2));
        available_mods_.push_back(mods_.mod(i, 3));
    }
}

void App::cheatEquipAllMods() {
    for (unsigned int agent = 0; agent < agents_recruited_.size(); agent++) {
        agents_recruited_[agent]->clearSlots();
        for (unsigned int i = 0; i < 6; i++) {
            agents_recruited_[agent]->setSlot(SLOT_LEGS - i,
                                              available_mods_[2 + i * 3]);
        }
    }
}

/*!
 * Activate cheat mode in which all missions are playable.
 */
void App::cheatAnyMission() {
    session_.cheatEnableAllMission();
}

void App::cheatResurrectAgents() {
    // TODO: Implement cheatResurrectAgents()
}

void App::cheatOwnAllCountries() {
    // TODO: Implement cheatOwnAllCountries()
}

void App::cheatAccelerateTime() {
    // TODO: Implement cheatAccelerateTime()
}

void App::cheatFemaleRecruits() {
    agents_recruited_.clear();

    std::set<std::string> names;
    while (agents_recruited_.size() < 18) {
        Agent *a = agents_.agent(rand() % agents_.numAgents());

        if (names.find(a->name()) == names.end() && !a->isMale()) {
            agents_recruited_.push_back(a);
            names.insert(a->name());
        }
    }

    for (int i = 0; i < 4; i++)
        team_members_[i] = agents_recruited_[i];
}

void App::cheatEquipFancyWeapons() {
    for (unsigned int i = 0; i < agents_recruited_.size(); i++) {
        agents_recruited_[i]->removeAllWeapons();
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("MINI-GUN")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("MINI-GUN")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("PERSUADERTRON")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("TIME BOMB")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("ENERGY SHIELD")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("ENERGY SHIELD")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("LASER")->createInstance());
        agents_recruited_[i]->addWeapon(
                weapons_.findWeapon("LASER")->createInstance());
    }
}

/*!
 * Activate a cheat code with the given name. Possible 
 * cheat codes are :
 * - DO IT AGAIN : Possibility to replay a completed mission
 * - NUK THEM : Enable all missions and resurrect dead agents
 * - OWN THEM : All countries belong to the user
 * - ROB A BANK : Puts $100 000 000 in funds
 * - TO THE TOP : Puts $100 000 000 in funds and activates all missions
 * - COOPER TEAM : $100 000 000 in funds, select any mission, all weapons 
 *   and mods
 * - WATCH THE CLOCK : Accelerates time
 *
 * \param name The name of a cheat code.
 */
void App::setCheatCode(const char *name) {
    
    // Repeat mission with previously obtained items, press 'C' or 'Ctrl-C'
    // to instantly complete a mission
    if (!strcmp(name, "DO IT AGAIN"))
        cheatRepeatOrCompleteMission();
    else if (!strcmp(name, "NUK THEM")) {
        // Select any mission, resurrect dead agents
        cheatAnyMission();
        cheatResurrectAgents();
    }
    else if (!strcmp(name, "OWN THEM")) {
        // Own all countries
        cheatOwnAllCountries();
    }
    else if (!strcmp(name, "ROB A BANK")) {
        // $100 000 000 in funds
        cheatFunds();
    }
    else if (!strcmp(name, "TO THE TOP")) {
        // $100 000 000 in funds, select any mission
        cheatFunds();
        cheatAnyMission();
    }
    else if (!strcmp(name, "COOPER TEAM")) {
        // $100 000 000 in funds, select any mission, all weapons and mods
        cheatFemaleRecruits();
        cheatFunds();
        cheatAnyMission();
        cheatWeaponsAndMods();
        cheatEquipAllMods();
        cheatEquipFancyWeapons();
    }
    else if (!strcmp(name, "WATCH THE CLOCK")) {
        // Accelerate time for faster research completion
        cheatAccelerateTime();
    }
}

void App::reset() {
    session_.reset();

    agents_.reset();

    agents_recruited_.clear();

    std::set<std::string> names;
    while (agents_recruited_.size() < 8) {
        Agent *a = agents_.agent(rand() % agents_.numAgents());

        if (names.find(a->name()) == names.end()) {
            agents_recruited_.push_back(a);
            names.insert(a->name());
        }
    }

    for (int i = 0; i < 4; i++)
        team_members_[i] = agents_recruited_[i];

    available_weapons_.push_back(weapons_.findWeapon("PERSUADERTRON"));
    available_weapons_.push_back(weapons_.findWeapon("PISTOL"));
    available_weapons_.push_back(weapons_.findWeapon("SHOTGUN"));
    available_weapons_.push_back(weapons_.findWeapon("SCANNER"));
    available_weapons_.push_back(weapons_.findWeapon("MEDIKIT"));

    for (int i = 0; i < 6; i++)
        available_mods_.push_back(mods_.mod(i));
//    cheatFunds();
//    cheatWeaponsAndMods();
//    cheatEquipAllMods();
}

void App::keyEvent(Key & key, KeyMod & mod, bool pressed) {
    if (!pressed && key != KEY_LCTRL && key != KEY_RCTRL
        && key != KEY_LALT)
        return;

    if (playingFli_) {
        if (key == KEY_LSHIFT || key == KEY_LCTRL || key == KEY_LALT)
            return;
        skipFli_ = true;
        return;
    }

    if (menus_.showingMenu()) {
        menus_.keyEvent(key, mod, pressed);
        return;
    }

    switch (key) {
    case KEY_ESCAPE:
    case KEY_q:
        quit();
        break;

    default:
        break;
    }
}

void App::mouseDownEvent(int x, int y, int button) {
    if (menus_.showingMenu()) {
        menus_.mouseDownEvent(x, y, button);
        return;
    }
}

void App::mouseUpEvent(int x, int y, int button) {
    if (menus_.showingMenu()) {
        menus_.mouseUpEvent(x, y, button);
        return;
    }
}

void App::mouseMotionEvent(int x, int y, int state) {
    if (menus_.showingMenu()) {
        menus_.mouseMotionEvent(x, y, state);
        return;
    }
}

void App::waitForKeyPress() {
    playingFli_ = true;
    skipFli_ = false;

    while (running_ && !skipFli_)
        system_->handleEvents();

    playingFli_ = false;
}

void App::setPalette(const char *fname, bool sixbit) {
    int size;
    uint8 *data = File::loadFile(fname, size);

    if (sixbit)
        system_->setPalette6b3(data);
    else
        system_->setPalette8b3(data);

    delete[] data;
}

// This variable is defined in freesynd.cpp
// TODO : use a general structure to holds init parameters
extern int start_mission;

void App::run() {
    int size = 0, tabSize = 0;
    uint8 *data, *tabData;

    FliPlayer fliPlayer;

    // TODO : add a config file to keep the info that the intro is played
    // only the first time the game is run
    if (start_mission == -1) {
        LOG(Log::k_FLG_GFX, "App", "run", ("Loading resource for the intro"))
        // this font is for the intro
        tabData = File::loadFile("mfnt-0.tab", tabSize);
        data = File::loadFile("mfnt-0.dat", size);
        intro_font_sprites_.loadSprites(tabData, tabSize, data, true);
        LOG(Log::k_FLG_GFX, "App", "run", ("%d sprites loaded from mfnt-0.dat", tabSize / 6))
        delete[] tabData;
        delete[] data;
        intro_font_.setSpriteManager(&intro_font_sprites_, 1);

        // play intro
        LOG(Log::k_FLG_GFX, "App", "run", ("Playing the intro"))
        data = File::loadFile("intro.dat", size);
        fliPlayer.loadFliData(data);
        music().playTrack(MusicManager::TRACK_INTRO);
        fliPlayer.play(true);
        music().stopPlayback();
        delete[] data;
    }

    // load palette
    data = File::loadFile("hpal01.dat", size);
    system_->setPalette6b3(data);
    delete[] data;

    // load palette
    setPalette("mselect.pal");

    // load "req"
    // TODO: what's this for?
    data = File::loadFile("hreq.dat", size);
    delete[] data;

    // load mspr-0 sprites
    LOG(Log::k_FLG_GFX, "App", "run", ("Loading sprites from mspr-0.dat ..."))
    tabData = File::loadFile("mspr-0.tab", tabSize);
    data = File::loadFile("mspr-0.dat", size);
    LOG(Log::k_FLG_GFX, "App", "run", ("%d sprites loaded", tabSize / 6))
    menu_sprites_.loadSprites(tabData, tabSize, data, true);
    delete[] tabData;
    delete[] data;
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_4, true, 1076, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_3, true, 802, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_2, true, 528, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_1, true, 254, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_4, false, 939, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_3, false, 665, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_2, false, 391, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_1, false, 117, 'A');
    game_font_.load();

#if 0
    system_->updateScreen();
    int nx = 0, ny = 0, my = 0;
    for (int i = 0; i < tabSize / 6; i++) {
        Sprite *s = menu_sprites_.sprite(i);
        if (nx + s->width() >= GAME_SCREEN_WIDTH) {
            nx = 0;
            ny += my;
            my = 0;
        }
        if (ny + s->height() > GAME_SCREEN_HEIGHT)
            break;
        s->draw(nx, ny, 0);
        system_->updateScreen();
        nx += s->width();
        if (s->height() > my)
            my = s->height();
    }

    waitForKeyPress();
    exit(1);
#endif
    //this is walk data
    // load "col01"
    data = File::loadFile("col01.dat", size);
    // original walk data
    memcpy(walkdata_,data,256);
    // walkdata_ patched version
    memcpy(walkdata_p_,data,256);
    // little patch to enable full surface description
    // and eliminate unnecessary data
    walkdata_p_[0x02] = 0x00;
    walkdata_p_[0x80] = 0x11;
    walkdata_p_[0x81] = 0x12;
    walkdata_p_[0x8F] = 0x00;
    walkdata_p_[0x93] = 0x00;
    delete[] data;

    // load palette
    setPalette("mselect.pal");

    if (start_mission == -1) {
        // play title
        data = File::loadFile("mtitle.dat", size);
        fliPlayer.loadFliData(data);
        fliPlayer.play();
        delete[] data;
        waitForKeyPress();

        // play the groovy menu startup anim
	    g_App.gameSounds().play(snd::MENU_UP);
        data = File::loadFile("mscrenup.dat", size);
        fliPlayer.loadFliData(data);
        fliPlayer.play();
        delete[] data;
    }

    menus_.createAllMenus();

    if (start_mission == -1) {
        // Regular scenario : start with the main menu
        menus_.changeCurrentMenu("main");
    } else {
        // Debug scenario : start directly with the brief menu
        // in the given mission
        // First, we find the block associated with the given
        // mission number
        for (int i=0; i<50; i++) {
            if (session_.getBlock(i).mis_id == start_mission) {
                session_.setSelectedBlockId(i);
            }
        }
        // Then we go to the brief menu
        menus_.changeCurrentMenu("brief");
        // show the cursor because at first it's hidden
        // and normally it's the main menu which shows it
        g_System.showCursor();
    }

    int lasttick = SDL_GetTicks();
    while (running_) {
        system_->handleEvents();
        int curtick = SDL_GetTicks();
        menus_.handleTick(curtick - lasttick);
        menus_.renderMenu();
        lasttick = curtick;
        system_->updateScreen();
    }

#ifdef GP2X
#ifndef WIN32
    // return to the menu
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
#endif
}
