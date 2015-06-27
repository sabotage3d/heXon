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
    SharedPtr<Text> scoreText_;

    double GetHealth(){return health_;}
    void Hit(float damage, int ownerID);
    void AddScore(int points);
    void Pickup(const StringHash nameHash);
private:
    double initialHealth_;
    double health_;
    int firstHitBy_ = 0;
    int lastHitBy_ = 0;
    int score_;
    int weaponLevel_;
    int bulletAmount_;

    int appleCount_ = 0;
    int heartCount_ = 0;

    const double shotInterval_ = 0.23;
    double sinceLastShot_ = 0.0;

    StaticModel* model_;
    RigidBody* rigidBody_;

    SharedPtr<Sound> shot_;
    Vector<SharedPtr<SoundSource> > sampleSources_;

    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
    void PlaySample(Sound *sample);
    void FireBullet(const Vector3 fire, const float angle);
    void UpgradeWeapons();
};
