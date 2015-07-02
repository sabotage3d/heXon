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
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>

#include "seeker.h"
#include "hitfx.h"
#include "tilemaster.h"
#include "player.h"

Seeker::Seeker(Context *context, MasterControl *masterControl, Vector3 position):
    SceneObject(context, masterControl),
    lifeTime_{10.0f},
    damage_{2.3f}
{
    rootNode_->SetName("Seeker");

    rootNode_->SetPosition(position);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(2.3f);
    rigidBody_->SetLinearDamping(0.23f);
    rigidBody_->SetTrigger(true);

    CollisionShape* trigger = rootNode_->CreateComponent<CollisionShape>();
    trigger->SetSphere(1.0f);

    ParticleEmitter* particleEmitter = rootNode_->CreateComponent<ParticleEmitter>();
    particleEmitter->SetEffect(masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/Seeker.xml"));

    Light* light = rootNode_->CreateComponent<Light>();
    light->SetRange(5.0f);
    light->SetBrightness(2.3f);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(Seeker, HandleSceneUpdate));
    SubscribeToEvent(rootNode_, E_NODECOLLISIONSTART, HANDLER(Seeker, HandleTriggerStart));

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));
}

void Seeker::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace SceneUpdate;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    age_ += timeStep;
    if (age_ > lifeTime_ && rootNode_->IsEnabled()) {
        new HitFX(context_, masterControl_, rootNode_->GetPosition(), false);
        Disable();
    }

    if (target_ == nullptr) {
        target_ = masterControl_->player_->rootNode_;
    }
    else rigidBody_->ApplyForce((target_->GetPosition() - rootNode_->GetPosition()).Normalized() * timeStep * 666.0f);
}

void Seeker::HandleTriggerStart(StringHash eventType, VariantMap &eventData)
{
    using namespace NodeCollisionStart;

    PODVector<RigidBody*> collidingBodies;
    rigidBody_->GetCollidingBodies(collidingBodies);

    for (int i = 0; i < collidingBodies.Length(); i++) {
        RigidBody* collider = collidingBodies[i];
        if (collider->GetNode()->GetNameHash() == N_PLAYER) {
            masterControl_->player_->Hit(2.3f);
            new HitFX(context_, masterControl_, rootNode_->GetPosition());
            collider->ApplyImpulse(rigidBody_->GetLinearVelocity()*0.5f);
            Disable();
        }
        else if (collider->GetNode()->GetNameHash() == N_SEEKER){
            new HitFX(context_, masterControl_, rootNode_->GetPosition(), false);
            Disable();
        }
    }
}
