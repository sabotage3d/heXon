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
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>

#include "enemy.h"
#include "explosion.h"
#include "tilemaster.h"

Enemy::Enemy(Context *context, MasterControl *masterControl, Vector3 position):
    SceneObject(context, masterControl, "Enemy")
{
    rootNode_->SetPosition(position);
    //Generate random color
    int randomizer = Random(6);
    color_ = Color(0.25f + (randomizer * 0.075f), 0.75f - (randomizer * 0.075f), 1.0f, 1.0f);
    //Color fireColor = Color(sqrt(coreColor_.r_), sqrt(coreColor_.g_), 1.0f, 0.25f);

    ParticleEmitter* particleEmitter = rootNode_->CreateComponent<ParticleEmitter>();
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
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetSphere(2.0f);

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));
}

/*public override void Set() {
base.Set();
firstHitBy = 0;
lastHitBy = 0;
health = initialHealth;
bonus = true;
}*/

// Takes care of dealing damage and keeps track of who deserves how many points.
void Enemy::Hit(float damage, int ownerID) {
    if (firstHitBy_ == 0) firstHitBy_ = ownerID;
    else if (firstHitBy_ != ownerID) bonus_ = false;
    lastHitBy_ = ownerID;
    health_ -= damage;
}

void Enemy::CheckHealth()
{
    if (rootNode_->IsEnabled() && health_ <= 0.0) {
        Explosion* newExplosion = new Explosion(context_, masterControl_, rootNode_->GetPosition(), color_);
        newExplosion->rigidBody_->SetLinearVelocity(rigidBody_->GetLinearVelocity());
        Disable();
    }
}