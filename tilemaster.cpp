/* heXon
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "tilemaster.h"
#include "tile.h"

TileMaster::TileMaster(Context *context, MasterControl* masterControl):
Object(context),
  masterControl_{masterControl}
{
    //Create hexagonal field
    //Lays a field of hexagons at the origin
    int bigHexSize = 23;
    for (int i = 0; i < bigHexSize; i++) {
        for (int j = 0; j < bigHexSize; j++) {
            if (i < (bigHexSize - bigHexSize / 4) + j / 2 &&  //Exclude bottom right
                    i > (bigHexSize / 4) - (j + 1) / 2 &&  //Exclude bottom left
                    i + 1 < (bigHexSize - bigHexSize / 4) + ((bigHexSize - j + 1)) / 2 &&  //Exclude top right
                    i - 1 > (bigHexSize / 4) - ((bigHexSize - j + 2) / 2)) { //Exclude top left
                Vector3 tilePos = Vector3((-bigHexSize / 2.0f + i) * 2.0f + j % 2, 0.0f, (-bigHexSize / 2.0f + j + 0.5f) * 1.8f);
                tileMap_[IntVector2(i, j)] = new Tile(context_, this, tilePos);
            }
        }
    }
}


void TileMaster::Start()
{
}

void TileMaster::Stop()
{
}

Tile *TileMaster::GetRandomTile()
{
    Vector<SharedPtr<Tile> > tiles = tileMap_.Values();
    if (tiles.Length()){
        SharedPtr<Tile> tile;
        while (!tile){
            SharedPtr<Tile> tryTile = tiles[Random((int)tiles.Length())];
            PODVector<PhysicsRaycastResult> hitResults;
            Ray spawnRay(tryTile->rootNode_->GetPosition()-Vector3::UP, Vector3::UP*10.0f);
            if (!masterControl_->PhysicsRayCast(hitResults, spawnRay, 23.0f, M_MAX_UNSIGNED)){
                tile = tryTile;
            }
        }
        return tile.Get();
    }
}
