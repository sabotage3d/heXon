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
#include "sceneobject.h"

class ArenaEdge : public SceneObject
{
    OBJECT(ArenaEdge);
public:
    ArenaEdge(Context *context, MasterControl* masterControl, float yRotation);
    void Start();
protected:
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<StaticModel> model_;
    SharedPtr<Sound> sample_;
    SharedPtr<SoundSource> sampleSource_;
    void HandleNodeCollisionStart(StringHash otherNode, VariantMap &eventData);
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
private:

};
