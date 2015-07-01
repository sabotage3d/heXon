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
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
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

class Player : public SceneObject
{
    OBJECT(Player);
public:
    Player(Context* context, MasterControl* masterControl);

    double GetHealth(){return health_;}
    void Hit(float damage);
    void AddScore(int points);
    void Pickup(const StringHash nameHash);
    void Die();
    void Shoot(Vector3 fire);
private:
    float initialHealth_;
    float health_;
    int score_;
    int weaponLevel_;
    int bulletAmount_;

    int appleCount_ = 0;
    int heartCount_ = 0;

    float initialShotInterval_;
    float shotInterval_;
    float sinceLastShot_ = 0.0f;

    StaticModel* model_;
    RigidBody* rigidBody_;

    Node* healthBarNode_;
    StaticModel* healthBarModel_;
    Node* shieldBarNode_;
    StaticModel* shieldBarModel_;

    Node* appleCounterRoot_;
    Node* appleCounter_[5];
    Node* heartCounterRoot_;
    Node* heartCounter_[5];

    SharedPtr<Sound> shot_;
    Vector<SharedPtr<SoundSource> > sampleSources_;

    String scoreTextName_;

    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
    void PlaySample(Sound *sample);
    void FireBullet(const Vector3 direction);
    void UpgradeWeapons();
    void SetHealth(float health);
    Color HealthToColor(float health);
};
