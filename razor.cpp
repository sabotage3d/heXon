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
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "mastercontrol.h"
#include "razor.h"

Razor::Razor(Context *context, MasterControl *masterControl, Vector3 position):
    Enemy(context, masterControl, position),
    topSpeed_{10.0},
    aimSpeed_{0.25*topSpeed_}
{
    Material* black = masterControl_->cache_->GetResource<Material>("Resources/Materials/Black.xml");

    topNode_ = rootNode_->CreateChild();
    topModel_ = topNode_->CreateComponent<StaticModel>();
    topModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/RazorTop.mdl"));
    //topModel_->SetCastShadows(true);
    topModel_->SetMaterial(black);

    bottomNode_ = rootNode_->CreateChild();
    bottomModel_ = bottomNode_->CreateComponent<StaticModel>();
    bottomModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/RazorBottom.mdl"));
    bottomModel_->SetMaterial(black);

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(Razor, HandleSceneUpdate));
}

void Razor::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    CheckHealth();

    using namespace SceneUpdate;

    double timeStep = eventData[P_TIMESTEP].GetFloat();

    //Spin
    topNode_->Rotate(Quaternion(0.0f, timeStep*50.0f*aimSpeed_, 0.0f));
    bottomNode_->Rotate(Quaternion(0.0f, timeStep*-50.0f*aimSpeed_, 0.0f));

    //Emerge
    if (rootNode_->GetPosition().y_ < -0.1f) {
        rootNode_->Translate(Vector3::UP * timeStep * (0.25f - rootNode_->GetPosition().y_));
        //topRenderer.materials[1].color = randomColor * 0.25f;
    }
    //Get moving
    else if (rigidBody_->GetLinearVelocity().Length() < 0.1f) {
        rigidBody_->ApplyImpulse(Vector3::RIGHT*Random(-0.05f, 0.05f) + Vector3::FORWARD*Random(-0.05f, 0.05f));
    }
    //Adjust speed
    else if (rigidBody_->GetLinearVelocity().Length() < aimSpeed_) {
        rigidBody_->ApplyForce(rigidBody_->GetLinearVelocity().Normalized() * (aimSpeed_ - rigidBody_->GetLinearVelocity().Length()));
    }
    else rigidBody_->ApplyForce(-rigidBody_->GetLinearVelocity());
}

void Razor::Hit(float damage, int ownerID)
{
    Enemy::Hit(damage, ownerID);
    aimSpeed_ = (1.0 - 0.75 * health_) * topSpeed_;
}