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

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Graphics/ParticleEmitter.h>

#include "sceneobject.h"

namespace Urho3D {
class Node;
class Scene;
class Sprite;
}

class Pickup : public SceneObject
{
    OBJECT(Pickup);
public:
    Pickup(Context *context, MasterControl* masterControl);
protected:
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<RigidBody> triggerBody_;
    SharedPtr<StaticModel> model_;
    SharedPtr<ParticleEmitter> particleEmitter_;
    SharedPtr<Sound> sample_;
    SharedPtr<SoundSource> sampleSource_;

    void HandleTriggerStart(StringHash otherNode, VariantMap &eventData);
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
private:

};
