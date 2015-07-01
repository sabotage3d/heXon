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

#include "enemy.h"
#include "explosion.h"
#include "tilemaster.h"
#include "player.h"

Enemy::Enemy(Context *context, MasterControl *masterControl, Vector3 position):
    SceneObject(context, masterControl),
    initialHealth_{1.0f},
    mass_{2.0f},
    whackInterval_{0.5f},
    sinceLastWhack_{whackInterval_},
    whackDamage_{0.5f}
{
    health_ = initialHealth_;
    rootNode_->SetName("Enemy");

    rootNode_->SetPosition(position);
    //Generate random color
    int randomizer = Random(6);
    color_ = Color(0.5f + (randomizer * 0.075f), 0.9f - (randomizer * 0.075f), 0.5+Max(randomizer-3.0, 3.0)/6.0, 1.0f);

    particleNode_ = rootNode_->CreateChild("SmokeTrail");
    ParticleEmitter* particleEmitter = particleNode_->CreateComponent<ParticleEmitter>();
    SharedPtr<ParticleEffect> particleEffect = masterControl_->cache_->GetTempResource<ParticleEffect>("Resources/Particles/Enemy.xml");
    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(color_, 0.1f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f));
    particleEffect->SetColorFrames(colorFrames);
    particleEmitter->SetEffect(particleEffect);

    centerModel_ = rootNode_->CreateComponent<StaticModel>();
    centerModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/RazorCenter.mdl"));
    centerModel_->SetMaterial(masterControl_->cache_->GetTempResource<Material>("Resources/Materials/Glow.xml"));
    centerModel_->GetMaterial(0)->SetShaderParameter("MatDiffColor", color_);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetRestitution(0.666f);
    rigidBody_->SetLinearDamping(0.1f);
    rigidBody_->SetMass(mass_);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetSphere(2.0f);

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(Enemy, HandleSceneUpdate));
    SubscribeToEvent(rootNode_, E_NODECOLLISIONSTART, HANDLER(Enemy, HandleCollisionStart));
}

void Enemy::Set(Vector3 position)
{
    rigidBody_->SetLinearVelocity(Vector3::ZERO);
    rigidBody_->ResetForces();
    rigidBody_->SetMass(mass_);
    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));

    firstHitBy_ = lastHitBy_ = 0;
    bonus_ = true;
    health_ = initialHealth_;
    panic_ = 0.0f;

    ParticleEmitter* particleEmitter = particleNode_->GetComponent<ParticleEmitter>();
    particleEmitter->RemoveAllParticles();
    SceneObject::Set(position);
}

// Takes care of dealing damage and keeps track of who deserves how many points.
void Enemy::Hit(float damage, int ownerID) {
    if (firstHitBy_ == 0) firstHitBy_ = ownerID;
    else if (firstHitBy_ != ownerID) bonus_ = false;

    lastHitBy_ = ownerID;
    health_ -= damage;
    panic_ = (initialHealth_-health_)/initialHealth_;

    CheckHealth();
}

void Enemy::CheckHealth()
{
    //Die
    if (rootNode_->IsEnabled() && health_ <= 0.0) {
        masterControl_->player_->AddScore(bonus_ ? worth_ : 2 * worth_ / 3);
        Explosion* newExplosion = new Explosion(context_, masterControl_, rootNode_->GetPosition(), Color(color_.r_*color_.r_, color_.g_*color_.g_, color_.b_*color_.b_), 0.5f*mass_);
        newExplosion->rigidBody_->SetLinearVelocity(rigidBody_->GetLinearVelocity());
        Disable();
    }
}

void Enemy::Disable()
{
    SceneObject::Disable();
}

Color Enemy::GetGlowColor()
{
    return color_*(Sin(200.0f*(masterControl_->world.scene->GetElapsedTime()+panicTime_))*(0.25f+panic_*0.25f)+(panic_*0.5f));
}

void Enemy::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace SceneUpdate;
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    panicTime_ += 3.0f * panic_ * timeStep;
    sinceLastWhack_ += timeStep;

    if ((rootNode_->GetPosition()*(Vector3::ONE-Vector3::UP)).Length() > 23.5f) Disable();;
}

void Enemy::HandleCollisionStart(StringHash eventType, VariantMap &eventData)
{
    using namespace NodeCollisionStart;

    PODVector<RigidBody*> collidingBodies;
    rigidBody_->GetCollidingBodies(collidingBodies);

    if (sinceLastWhack_ > whackInterval_){
        for (int i = 0; i < collidingBodies.Length(); i++) {
            RigidBody* collider = collidingBodies[i];
            if (collider->GetNode()->GetNameHash() == N_PLAYER) {
                //sampleSource_->Play(sample_);
                masterControl_->player_->Hit(whackDamage_);
                sinceLastWhack_ = 0.0f;
            }
        }
    }
}
