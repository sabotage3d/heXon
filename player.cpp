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
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

#include "mastercontrol.h"
#include "tilemaster.h"
#include "player.h"
#include "bullet.h"
#include "muzzle.h"

Player::Player(Context *context, MasterControl *masterControl):
    SceneObject(context, masterControl, "Player")
{
    model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Swift.mdl"));
    model_->SetMaterial(0, masterControl_->cache_->GetTempResource<Material>("Resources/Materials/GreenGlowEnvmap.xml"));
    model_->SetMaterial(1, masterControl_->cache_->GetTempResource<Material>("Resources/Materials/GreenEnvmap.xml"));

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetRestitution(0.666);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.5f);
    //rigidBody_->ApplyForce(Vector3::LEFT*100);
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetSphere(2.0f);

    ParticleEmitter* particleEmitter = rootNode_->CreateComponent<ParticleEmitter>();
    SharedPtr<ParticleEffect> particleEffect = masterControl_->cache_->GetTempResource<ParticleEffect>("Resources/Particles/Shine.xml");
    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(Color(0.42f, 0.7f, 0.23f, 0.23f), 0.2f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.4f));
    particleEffect->SetColorFrames(colorFrames);
    particleEmitter->SetEffect(particleEffect);

    sample_ = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Shot.ogg");
    sample_->SetLooped(false);
    for (int i = 0; i < 3; i++){
        sampleSources_.Push(SharedPtr<SoundSource>(rootNode_->CreateComponent<SoundSource>()));
        sampleSources_[i]->SetGain(0.3f);
        sampleSources_[i]->SetSoundType(SOUND_EFFECT);
    }

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));

    /*scoreText_ = masterControl_->ui_->GetRoot()->CreateChild<Text>();
    scoreText_->SetText(String(score_));
    scoreText_->SetFont(masterControl_->cache_->GetResource<Font>("Resources/Fonts/skirmishergrad.ttf"), 32);
    scoreText_->SetHorizontalAlignment(HA_LEFT);
    scoreText_->SetVerticalAlignment(VA_TOP);
    scoreText_->SetPosition(0, masterControl_->ui_->GetRoot()->GetHeight()/2.1);*/

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(Player, HandleSceneUpdate));
}

void Player::AddScore(int points)
{
    score_ += points;
}

void Player::PlaySample(Sound* sample)
{
    for (int i = 0; i < sampleSources_.Length(); i++){
        if (!sampleSources_[i]->IsPlaying()){
            sampleSources_[i]->Play(sample_);
            break;
        }
    }
}

void Player::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    //Take the frame time step, which is stored as a double
    double timeStep = eventData[P_TIMESTEP].GetFloat();

    Input* input = GetSubsystem<Input>();

    //Movement values
    Vector3 move = Vector3::ZERO;
    Vector3 moveJoy = Vector3::ZERO;
    Vector3 moveKey = Vector3::ZERO;
    double thrust = 2358.0;
    double maxSpeed = 21.0;
    //Firing values
    Vector3 fire = Vector3::ZERO;
    Vector3 fireJoy = Vector3::ZERO;
    Vector3 fireKey = Vector3::ZERO;

    //Read input
    if (input->GetJoystickByIndex(0)){
    moveJoy = Vector3::RIGHT * input->GetJoystickByIndex(0)->GetAxisPosition(0) +
            Vector3::BACK * input->GetJoystickByIndex(0)->GetAxisPosition(1);
    fireJoy = Vector3::RIGHT * input->GetJoystickByIndex(0)->GetAxisPosition(2) +
            Vector3::BACK * input->GetJoystickByIndex(0)->GetAxisPosition(3);
    }
    moveKey = Vector3::LEFT * input->GetKeyDown(KEY_A) +
            Vector3::RIGHT * input->GetKeyDown(KEY_D) +
            Vector3::FORWARD * input->GetKeyDown(KEY_W) +
            Vector3::BACK * input->GetKeyDown(KEY_S);
    fireKey = Vector3::LEFT * input->GetKeyDown(KEY_J) +
            Vector3::RIGHT * input->GetKeyDown(KEY_L) +
            Vector3::FORWARD * input->GetKeyDown(KEY_I) +
            Vector3::BACK * input->GetKeyDown(KEY_K);

    //Pick most significant input
    moveJoy.Length() > moveKey.Length() ? move = moveJoy : move = moveKey;
    fireJoy.Length() > fireKey.Length() ? fire = fireJoy : fire = fireKey;


    //Restrict move vector length
    if (move.Length() > 1.0) move /= move.Length();
    //Deadzone
    else if (move.Length() < 0.1) move *= 0.0;

    if (fire.Length() < 0.1) fire *= 0.0;
    else fire.Normalize();

    //Apply movement
    Vector3 force = move * thrust * timeStep;
    if (rigidBody_->GetLinearVelocity().Length() < maxSpeed || (rigidBody_->GetLinearVelocity().Normalized() + force.Normalized()).Length() < 1.0) {
        rigidBody_->ApplyForce(force);
    }

    //Update rotation according to direction of the ship's movement.
    if (rigidBody_->GetLinearVelocity().Length() > 0.1)
        rootNode_->LookAt(rootNode_->GetPosition()+rigidBody_->GetLinearVelocity());

    //Shooting
    sinceLastShot_ += timeStep;
    if (fire.Length()) {
        if (sinceLastShot_ > shotInterval_)
        {
            sinceLastShot_ = 0.0;
            Bullet* bullet = new Bullet(context_, masterControl_);
            bullet->rootNode_->SetPosition(rootNode_->GetPosition());
            bullet->rootNode_->LookAt(bullet->rootNode_->GetPosition() + fire);
            bullet->rigidBody_->ApplyForce(fire*1500.0f);
            new Muzzle(context_, masterControl_, rootNode_->GetPosition());
            PlaySample(sample_);
        }
    }
}

