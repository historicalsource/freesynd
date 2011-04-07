/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net>*
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
#ifndef RESEARCHMANAGER_H
#define RESEARCHMANAGER_H

#include <list>

#include "core/research.h"
#include "core/gameevent.h"
#include "utils/seqmodel.h"

class Research;

/*!
 * This class manages all research functionnality.
 */
class ResearchManager {
public:
    //! Default constructor
    ResearchManager();
    ~ResearchManager();

    bool reset();
    /*!
     * Returns a list of all available search on mods.
     * \return List can be empty but not null.
     */
    SequenceModel * getAvailableModsSearch() { return &availableModsSearch_; }
    /*!
     * Returns a list of all available search on weapons.
     * \return List can be empty but not null.
     */
    SequenceModel * getAvailableWeaponsSearch() { return &availableWeaponsSearch_; }

    // Processes all engaged research
    int process(int hourElapsed, int moneyLeft);
    //! Adds a listener for research event
    void addListener(GameEventListener *pListener);
    void removeListener(GameEventListener *pListener);

protected:
    Research *loadResearch(Weapon::WeaponType wt);
    void destroy();
    void fireGameEvent(Research *pResearch);
    int processList(int hourElapsed, int moneyLeft, VectorModel < Research * > *pList);

protected:
    /*! List of all currently available research on mods.*/
    VectorModel<Research *> availableModsSearch_;
    /*! List of all currently available research on weapons.*/
    VectorModel<Research *> availableWeaponsSearch_;
    /*! List of listeners for research events.*/
    std::list<GameEventListener *> listeners_;
};

#endif //RESEARCHMANAGER_H