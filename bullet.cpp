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

#include "bullet.h"
#include "spawnmaster.h"
#include "razor.h"
#include "spire.h"
#include "tilemaster.h"
#include "hitfx.h"

Bullet::Bullet(Context *context, MasterControl *masterControl):
    SceneObject(context, masterControl, "Pickup"),
    lifetime_{1.0f},
    damage_{0.25f}
{
    rootNode_->SetScale(Vector3(1.0f, 1.0f, 0.1f));
    model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Bullet.mdl"));
    model_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/Bullet.xml"));

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(0.2f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetFriction(0.0f);

    Light* light = rootNode_->CreateComponent<Light>();
    light->SetRange(5.0f);
    light->SetColor(Color(0.6f, 1.0f, 0.2f));

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(Bullet, HandleSceneUpdate));

        masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));
}

void Bullet::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    age_ += timeStep;
    rootNode_->SetScale(Vector3(Max(1.75f - 10.0f*age_, 1.0f),
                                Max(1.75f - 10.0f*age_, 1.0f),
                                Min(Min(35.0f*age_, 2.0f), Max(2.0f-timeSinceHit_*42.0f, 0.1f))
                                ));
    if (age_ > lifetime_) {
        Disable();
    }

    if (timeStep > 0.0f && !fading_) HitCheck(timeStep);
}

void Bullet::Disable()
{
    fading_ = true;
    masterControl_->tileMaster_->RemoveFromAffectors(rootNode_);
    rootNode_->SetEnabled(false);
}

void Bullet::HitCheck(float timeStep) {
    if (!fading_) {
        PODVector<PhysicsRaycastResult> hitResults;
        Ray bulletRay(rootNode_->GetPosition(), rootNode_->GetDirection());
        if (masterControl_->PhysicsRayCast(hitResults, bulletRay, rigidBody_->GetLinearVelocity().Length()*timeStep, M_MAX_UNSIGNED)){
            for (int i = 0; i < hitResults.Size(); i++){
                if (!hitResults[i].body_->IsTrigger()){
                    hitResults[i].body_->ApplyImpulse(rigidBody_->GetLinearVelocity()*0.05f);
                    new HitFX(context_, masterControl_, hitResults[i].position_);
                    //Deal damage
                    unsigned hitID = hitResults[i].body_->GetNode()->GetID();
                    if(masterControl_->spawnMaster_->spires_.Keys().Contains(hitID)){
                        masterControl_->spawnMaster_->spires_[hitID]->Hit(damage_, 1);
                    }
                    else if(masterControl_->spawnMaster_->razors_.Keys().Contains(hitID)){
                        masterControl_->spawnMaster_->razors_[hitID]->Hit(damage_, 1);
                    }

                    Disable();
                }
            }
        }
    }
}
            //Old C# code
            /*RaycastHit hit = new RaycastHit();
            if (Physics.Raycast(transform.position, rigidbody.velocity.normalized, out hit, 0.01f + 1.5f * rigidbody.velocity.magnitude * Time.deltaTime, layerMask)) {
                foreach (Player player in GameObject.FindObjectsOfType<Player>()) {
                    if (player.playerID == ownerID) {
                        GameObject hitFX = masterControl.hitFXs.FetchInactive();
                        hitFX.transform.position = hit.point;
                        hitFX.GetComponent<PoolObject>().Set();
                        if (hasParticleSystem) hitFX.GetComponent<ParticleSystem>().startColor = particleSystem.startColor;
                        hitFX.gameObject.SetActive(true);
                    }
                }


                fading = true;
                if (hit.transform.rigidbody)hit.transform.rigidbody.AddForce(rigidbody.velocity*0.2f,ForceMode.Impulse);
                if (hit.transform.GetComponent<Player>()) {
                    hit.transform.GetComponent<Player>().Hit(damage, ownerID);
                }
                else if (hit.transform.GetComponent<Razor>()) {
                    hit.transform.GetComponent<Razor>().Hit(damage, ownerID);
                }
                else if (hit.transform.GetComponent<Spire>()) {
                    hit.transform.GetComponent<Spire>().Hit(damage, ownerID);
                }
                else if (hit.transform.GetComponent<ChopEye>()) {
                    hit.transform.GetComponent<ChopEye>().Hit(damage, ownerID);
                }
                else if (hit.transform.GetComponent<SpireBullet>()) {
                    SpireBullet spireBullet = hit.transform.GetComponent<SpireBullet>();
                    spireBullet.fading = true;
                    GameObject hitFX = masterControl.hitFXs.FetchInactive();
                    hitFX.transform.position = hit.point;
                    hitFX.GetComponent<PoolObject>().Set();
                    if (hasParticleSystem) hitFX.GetComponent<ParticleSystem>().startColor = spireBullet.particleSystem.startColor;
                    hitFX.gameObject.SetActive(true);

                }
                else if (hit.transform.GetComponent<BoomerBullet>()) {
                    hit.transform.GetComponent<BoomerBullet>().age = 1f;
                }
            }
        }*/
