/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>                *
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

#ifndef MAP_H
#define MAP_H

#include "common.h"

#define NUM_MAPS               59

class Tile;
class TileManager;
class MapObject;

/*!
 * This a convenient structure to store a position
 * in map tile coordinates.
 */
struct MapTilePoint {
    /*! The tile's X coord.*/
    int tx;
    /*! The tile's Y coord.*/
    int ty;
    /*! The tile's Z coord.*/
    int tz;
    /*! X Offset inside the tile. (varies between 0 and 256)*/
    int ox;
    /*! Y Offset inside the tile. (varies between 0 and 256)*/
    int oy;

    MapTilePoint() {
        tx = 0;
        ty = 0;
        tz = 0;
        ox = 0;
        oy = 0;
    }

    MapTilePoint(const MapTilePoint &mtp) {
        tx = mtp.tx;
        ty = mtp.ty;
        tz = mtp.tz;
        ox = mtp.ox;
        oy = mtp.oy;
    }
};

/*!
 * This a convenient structure to store a position
 * in screen coordinates (in pixels).
 */
struct MapScreenPoint {
    /*! the X coord on the screen.*/
    int x;
    /*! the Y coord on the screen.*/
    int y;
};

/*!
 * Map class.
 */
class Map {
public:
    Map(TileManager *tileManager, uint16 i_id);
    ~Map();

    bool loadMap(uint8 *mapData);

    uint16 id() { return i_id_; }
    int width() { return map_width_; }
    int height() { return map_height_; }
    void mapDimensions(int *x, int *y, int *z);
    //! Clip x,y,z to map dimensions
    void adjXYZ(int &x, int &y, int &z);

    int tileToScreenX(int x, int y, int z, int pX, int pY);
    int tileToScreenY(int x, int y, int z, int pX, int pY);
    //! Converts a Map tile position to a screen position
    MapScreenPoint tileToScreenPoint(int x, int y, int z, int pX, int pY);
    //! Converts a screen position in pixel into a Map tile position
    MapTilePoint screenToTilePoint(int x, int y);

    int maxX() { return max_x_; }
    int maxY() { return max_y_; }
    int maxZ() { return max_z_; }

    int maxZAt(int x, int y);
    Tile * getTileAt(int x, int y, int z);
    int tileAt(int x, int y, int z);
    void patchMap(int x, int y, int z, uint8 tileNum);

protected:
    /*!  Every map has a unique ID which is used to identify the 
    name of the file containing map data.*/
    uint16 i_id_;
    int max_x_, max_y_, max_z_;
    Tile **a_tiles_;
    TileManager *tile_manager_;
    int map_width_, map_height_;
};

/*!
 * A MiniMap is a small representation of the real map.
 */
class MiniMap {
public:
    /*! Constant for the minimap overlay : no overlay */
    static const uint8 kOverlayNone;
    /*! Constant for the minimap overlay : the agent is our. */
    static const uint8 kOverlayOurAgent;
    /*! Constant for the minimap overlay : this is an enemy agent. */
    static const uint8 kOverlayEnemyAgent;

    MiniMap(Map *p_map);
    ~MiniMap();

    /*! Returns the map width in tiles.*/
    int max_x() { return mmax_x_;}
    /*! Returns the map height in tiles.*/
    int max_y() { return mmax_y_;}
    uint8 getColourAt(int x, int y);

    //! Defines a source on the minimap for the signal
    void setTarget(MapObject *pTarget);
    //! Return the curent target. May be null
    MapObject * target() { return p_target_; }
    //! Clear the target source
    void clearTarget();

private:
    /* An array with the same size of the real map but containing
     a color for each type of tile. */
    uint8 *a_minimap_;
    /* Size of the minimap (same as the map).*/
    int mmax_x_;
    /* Height of the minimap (same as the map).*/
    int mmax_y_;
    /*! Current target emitting a signal.*/
    MapObject *p_target_;
};

#endif
