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
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

#include "effect.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Explosion : public Effect
{
    friend class Enemy;
    OBJECT(Explosion);
public:
    Explosion(Context* context, MasterControl* masterControl, Vector3 position, Color color, double size);

    void UpdateExplosion(StringHash eventType, VariantMap &eventData);
protected:
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<Light> light_;
private:
    SharedPtr<Sound> sample_;
    SharedPtr<SoundSource> sampleSource_;
    float initialMass_;
    float initialBrightness_;
};
