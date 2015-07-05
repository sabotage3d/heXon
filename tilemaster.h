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



//#include <Urho3D/Urho3D.h>
//#include <Urho3D/Core/CoreEvents.h>
//#include <Urho3D/Scene/Scene.h>
//#include <Urho3D/Container/Vector.h>
//#include <Urho3D/Math/Vector2.h>
//#include <Urho3D/Graphics/StaticModel.h>
//#include <Urho3D/Graphics/Model.h>
//#include <Urho3D/Graphics/Material.h>
//#include <Urho3D/Resource/ResourceCache.h>
#ifndef TILEMASTER_H
#define TILEMASTER_H

#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Tile;
class Slot;

enum TileElement {TE_CENTER = 0, TE_NORTH, TE_EAST, TE_SOUTH, TE_WEST, TE_NORTHWEST, TE_NORTHEAST, TE_SOUTHEAST, TE_SOUTHWEST, TE_LENGTH};
enum CornerType {CT_NONE, CT_IN, CT_OUT, CT_TWEEN, CT_DOUBLE, CT_FILL};
enum TileType {B_SPACE, B_EMPTY, B_ENGINE};

struct vec2
{
    int x;
    int y;
};

class TileMaster : public Object
{
    OBJECT(TileMaster);
    friend class InputMaster;
    friend class Tile;
public:
    TileMaster(Context *context, MasterControl* masterControl);

    MasterControl* masterControl_;
    Node* rootNode_;
    RigidBody* rigidBody_;

    bool CheckEmpty(Vector3 coords, bool checkTiles) const { return CheckEmpty(IntVector2(round(coords.x_), round(coords.z_)), checkTiles); }
    bool CheckEmpty(IntVector2 coords, bool checkTiles) const;
    bool CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool tileMap) const;
    IntVector2 GetNeighbourCoords(IntVector2 coords, TileElement element) const;
    CornerType PickCornerType(IntVector2 tileCoords, TileElement element) const;
    TileType GetBuildingType(IntVector2 coords);
    TileType GetNeighbourType(IntVector2 coords, TileElement element);


    virtual void Start();
    virtual void Stop();

    void AddMissingSlots();
    void FixFringe();
    void FixFringe(IntVector2 coords);

    void AddTile(IntVector2 newTileCoords);
    bool DisableSlot(IntVector2 coords);
    bool EnableSlot(IntVector2 coords);
    void EnableSlots();
    void DisableSlots();

    void AddToAffectors(WeakPtr<Node> affector, WeakPtr<RigidBody> rigidBody) { if (hexAffectors_.Values().Length() <= 23 || affector->GetNameHash() == N_SPIRE) hexAffectors_[affector] = rigidBody; }
    void RemoveFromAffectors(WeakPtr<Node> affector) { if (hexAffectors_.Contains(affector) ) hexAffectors_.Erase(affector); }
    HashMap<WeakPtr<Node>, WeakPtr<RigidBody> >* GetAffectors() { return &hexAffectors_; }

    Tile* GetRandomTile();
private:
    HashMap<WeakPtr<Node>, WeakPtr<RigidBody> > hexAffectors_;
    //HashMap<IntVector2, SharedPtr<Tile> > tileMap_;
    //HashMap<IntVector2, SharedPtr<Tile> > tileMap_;
    HashMap<int, SharedPtr<Tile>> tileMap_;
    HashMap<IntVector2, TileType> buildingMap_;

    bool selected_ = false;
    Vector3 moveTarget_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);



    void Select();
    void Deselect();
    void SetSelected(bool selected);
    bool IsSelected() const;

    void SetBuilding(IntVector2 coords, TileType type = B_ENGINE);
    void RemoveBuilding(IntVector2 coords) {SetBuilding(coords, B_EMPTY);}
    void UpdateCenterOfMass();
    void Move(double timeStep);
};

#endif
