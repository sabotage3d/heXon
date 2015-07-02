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

#ifndef ENEMY_H
#define ENEMY_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

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
    Enemy(Context* context, MasterControl* masterControl, Vector3 position);
    double GetHealth(){return health_;}
    void Hit(float damage, int ownerID);
protected:
    float panicTime_ = 0.0f;
    float health_;
    float initialHealth_;
    float panic_ = 0.0f;
    float mass_;
    int worth_ = 5;
    bool bonus_ = true;
    int firstHitBy_ = 0;
    int lastHitBy_ = 0;

    float sinceLastWhack_;
    float whackInterval_;
    float meleeDamage_;

    SharedPtr<Node> particleNode_;
    RigidBody* rigidBody_;
    StaticModel* centerModel_;
    Color color_;
    void CheckHealth();
    void Set(Vector3 position);
    Color GetGlowColor();
private:
    Vector<SharedPtr<Sound> > samples_;
    SharedPtr<SoundSource> soundSource_;
    void Disable();
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
    void HandleCollisionStart(StringHash eventType, VariantMap &eventData);
};

#endif
