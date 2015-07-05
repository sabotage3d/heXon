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
//#include <Urho3D/Scene/SceneEvents.h>
//#include <Urho3D/Physics/CollisionShape.h>
//#include <Urho3D/Scene/Scene.h>
//#include <Urho3D/Graphics/Model.h>
//#include <Urho3D/Graphics/StaticModel.h>
//#include <Urho3D/Graphics/Material.h>
//#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Resource/XMLFile.h>
//#include <Urho3D/Math/MathDefs.h>

#include "tilemaster.h"
#include "sceneobject.h"

SceneObject::SceneObject(Context* context, MasterControl* masterControl):
    Object(context)
{
    masterControl_ = masterControl;

    //Create the root node.
    rootNode_ = masterControl_->world.scene->CreateChild("SceneObject");
}

void SceneObject::Set(Vector3 position)
{
    rootNode_->SetPosition(position);
    rootNode_->SetEnabledRecursive(true);
}

void SceneObject::Disable()
{
    masterControl_->tileMaster_->RemoveFromAffectors(rootNode_);
    rootNode_->SetEnabledRecursive(false);
}
