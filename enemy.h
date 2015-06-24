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

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/CoreEvents.h>

#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Enemy : public SceneObject
{
    friend class SpawnMaster;
    OBJECT(Enemy);
public:
    Enemy(Context* context, MasterControl* masterControl, Vector3 position, String name);
    double GetHealth(){return health_;}
    void Hit(float damage, int ownerID);
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
protected:
    float panicTime_ = 0.0;
    float health_ = 1;
    float initialHealth_ = 1;
    float panic_ = 0.0f;
    float mass_;
    int worth_ = 5;
    bool bonus_ = true;
    int firstHitBy_ = 0;
    int lastHitBy_ = 0;

    SharedPtr<Node> particleNode_;
    RigidBody* rigidBody_;
    StaticModel* centerModel_;
    Color color_;
    void CheckHealth();
    void Set(Vector3 position);
    Color GetGlowColor();
};
