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
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

#include "arenaedge.h"
#include "flash.h"

ArenaEdge::ArenaEdge(Context* context, MasterControl* masterControl, float yRotation):
SceneObject(context, masterControl)
{
    rootNode_->SetName("ArenaEdge");
    rootNode_->SetScale(Vector3(24.0f, 2.0f, 24.0f));
    rootNode_->SetPosition(0.0f, 0.0f, 0.0f);
    rootNode_->SetRotation(Quaternion(0.0f, yRotation, 0.0f));

    //model_ = rootNode_->CreateComponent<StaticModel>();
    Model* model = masterControl_->cache_->GetResource<Model>("Resources/Models/ArenaEdgeSegment.mdl");
    //model_->SetModel(model);
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetTrigger(true);
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetConvexHull(model);

    sample_ = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Flash.ogg");
    sample_->SetLooped(false);
    sampleSource_ = rootNode_->CreateComponent<SoundSource>();
    sampleSource_->SetGain(0.5f);
    sampleSource_->SetSoundType(SOUND_EFFECT);


    SubscribeToEvent(rootNode_, E_NODECOLLISIONSTART, HANDLER(ArenaEdge, HandleNodeCollisionStart));
}

void ArenaEdge::Start () {
}

void ArenaEdge::HandleNodeCollisionStart(StringHash eventType, VariantMap &eventData)
{
    using namespace NodeCollisionStart;
    
    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());
    Node* collidingNode((Node*)eventData[P_OTHERNODE].GetPtr());
    //RigidBody* collidingBody((RigidBody*)eventData[P_OTHERBODY].GetPtr());

    Vector3 contactPosition = contacts.ReadVector3();
    Vector3 contactNormal = contacts.ReadVector3();

    if (Vector3::Distance(contactPosition, collidingNode->GetPosition()) < 2.0f){
        PODVector<PhysicsRaycastResult> hitResults;
        Ray blinkRay(contactPosition, -contactNormal);
        if (masterControl_->PhysicsRayCast(hitResults, blinkRay, 42.0, M_MAX_UNSIGNED)){
            for (int i = 0; i < hitResults.Size(); i++){
                if (hitResults[i].body_->GetNode()->GetNameHash() == N_ARENAEDGE)
                {
                    new Flash(context_, masterControl_, collidingNode->GetPosition());
                    collidingNode->SetPosition(collidingNode->GetPosition()-contactNormal*39.2f);
                    new Flash(context_, masterControl_, collidingNode->GetPosition());
                    sampleSource_->Play(sample_);
                    continue;
                }
            }
        }
    }
}
