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
#include "spawnmaster.h"
#include "tilemaster.h"
#include "player.h"
#include "bullet.h"
#include "muzzle.h"
#include "hexocam.h"
#include "explosion.h"

Player::Player(Context *context, MasterControl *masterControl):
    SceneObject(context, masterControl),
    initialHealth_{10.0},
    health_{initialHealth_},
    score_{0},
    weaponLevel_{0},
    bulletAmount_{0},
    initialShotInterval_{0.30f},
    shotInterval_{initialShotInterval_}
{
    rootNode_->SetName("Player");

    //Setup player graphics
    model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Swift.mdl"));
    model_->SetMaterial(0, masterControl_->cache_->GetTempResource<Material>("Resources/Materials/GreenGlowEnvmap.xml"));
    model_->SetMaterial(1, masterControl_->cache_->GetTempResource<Material>("Resources/Materials/GreenEnvmap.xml"));

    ParticleEmitter* particleEmitter = rootNode_->CreateComponent<ParticleEmitter>();
    SharedPtr<ParticleEffect> particleEffect = masterControl_->cache_->GetTempResource<ParticleEffect>("Resources/Particles/Shine.xml");
    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(Color(0.42f, 0.7f, 0.23f, 0.23f), 0.2f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.4f));
    particleEffect->SetColorFrames(colorFrames);
    particleEmitter->SetEffect(particleEffect);

    //Setup player audio
    shot_ = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Shot.ogg");
    shot_->SetLooped(false);
    for (int i = 0; i < 3; i++){
        sampleSources_.Push(SharedPtr<SoundSource>(rootNode_->CreateComponent<SoundSource>()));
        sampleSources_[i]->SetGain(0.3f);
        sampleSources_[i]->SetSoundType(SOUND_EFFECT);
    }

    //Setup player physics
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetRestitution(0.666);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.5f);
    rigidBody_->SetAngularFactor(Vector3::ZERO);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetSphere(2.0f);

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));

    //Setup 2D GUI elements
    UI* ui = GetSubsystem<UI>();
    Text* scoreText = ui->GetRoot()->CreateChild<Text>();
    scoreText->SetName("Score");
    scoreTextName_ = scoreText->GetName();
    scoreText->SetText("0");
    scoreText->SetFont(masterControl_->cache_->GetResource<Font>("Resources/Fonts/skirmishergrad.ttf"), 32);
    scoreText->SetColor(Color(0.23f, 0.75f, 1.0f, 0.75f));
    scoreText->SetHorizontalAlignment(HA_CENTER);
    scoreText->SetVerticalAlignment(VA_CENTER);
    scoreText->SetPosition(0, ui->GetRoot()->GetHeight()/2.2);

    //Setup 3D GUI elements
    healthBarNode_ = masterControl_->world.scene->CreateChild("HealthBar");
    healthBarNode_->SetPosition(0.0f, 1.0f, 21.0f);
    healthBarNode_->SetScale(health_, 0.5f, 0.5f);
    healthBarModel_ = healthBarNode_->CreateComponent<StaticModel>();
    healthBarModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Bar.mdl"));
    healthBarModel_->SetMaterial(masterControl_->cache_->GetTempResource<Material>("Resources/Materials/GreenGlowEnvmap.xml"));

    shieldBarNode_ = masterControl_->world.scene->CreateChild("HealthBar");
    shieldBarNode_->SetPosition(0.0f, 1.0f, 21.0f);
    shieldBarNode_->SetScale(health_, 0.45f, 0.45f);
    shieldBarModel_ = shieldBarNode_->CreateComponent<StaticModel>();
    shieldBarModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Bar.mdl"));
    shieldBarModel_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/BlueGlowEnvmap.xml"));

    Node* healthBarHolderNode = masterControl_->world.scene->CreateChild("HealthBarHolder");
    healthBarHolderNode->SetPosition(0.0f, 1.0f, 21.0f);
    StaticModel* healthBarHolderModel = healthBarHolderNode->CreateComponent<StaticModel>();
    healthBarHolderModel->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/BarHolder.mdl"));
    healthBarHolderModel->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/Metal.xml"));

    appleCounterRoot_ = masterControl_->world.scene->CreateChild("AppleCounter");
    for (int a = 0; a < 5; a++){
        appleCounter_[a] = appleCounterRoot_->CreateChild();
        appleCounter_[a]->SetEnabled(false);
        appleCounter_[a]->SetPosition(-((float)a + 8.0f), 1.0f, 21.0f);
        appleCounter_[a]->SetScale(0.333f);
        StaticModel* apple = appleCounter_[a]->CreateComponent<StaticModel>();
        apple->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Apple.mdl"));
        apple->SetMaterial(masterControl_->cache_->GetTempResource<Material>("Resources/Materials/GoldEnvmap.xml"));
    }

    heartCounterRoot_ = masterControl_->world.scene->CreateChild("HeartCounter");
    for (int h = 0; h < 5; h++){
        heartCounter_[h] = heartCounterRoot_->CreateChild();
        heartCounter_[h]->SetEnabled(false);
        heartCounter_[h]->SetPosition((float)h + 8.0f, 1.0f, 21.0f);
        heartCounter_[h]->SetScale(0.333f);
        StaticModel* heart = heartCounter_[h]->CreateComponent<StaticModel>();
        heart->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Heart.mdl"));
        heart->SetMaterial(masterControl_->cache_->GetTempResource<Material>("Resources/Materials/RedEnvmap.xml"));
    }

    //Subscribe to events
    SubscribeToEvent(E_SCENEUPDATE, HANDLER(Player, HandleSceneUpdate));
}

void Player::AddScore(int points)
{
    score_ += points;
    UI* ui = GetSubsystem<UI>();
    UIElement* scoreElement = ui->GetRoot()->GetChild(scoreTextName_);
    Text* scoreText = (Text*)scoreElement;
    scoreText->SetText(String(score_));
}

void Player::PlaySample(Sound* sample)
{
    for (int i = 0; i < sampleSources_.Length(); i++){
        if (!sampleSources_[i]->IsPlaying()){
            sampleSources_[i]->Play(sample);
            break;
        }
    }
}

void Player::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    //Take the frame time step, which is stored as a double
    double timeStep = eventData[P_TIMESTEP].GetFloat();
    //Pulse and spin the counters' apples and hearts
    for (int i = 0; i < 5; i++){
        appleCounter_[i]->Rotate(Quaternion(0.0f, (i*i+10.0f) * 23.0f * timeStep, 0.0f));
        appleCounter_[i]->SetScale(masterControl_->Sine(1.0f+(appleCount_/2.0f), 0.2f, 0.4, -i/M_TAU));
        heartCounter_[i]->Rotate(Quaternion(0.0f, (i*i+10.0f) * 23.0f * timeStep, 0.0f));
        heartCounter_[i]->SetScale(masterControl_->Sine(1.0f+(heartCount_/2.0f), 0.2f, 0.4, -i/M_TAU));
    }
    //Update HealthBar color
    healthBarModel_->GetMaterial()->SetShaderParameter("MatEmissiveColor", HealthToColor(health_));

    //Only handle input when player is active
    if (!rootNode_->IsEnabled()) return;


    Input* input = GetSubsystem<Input>();

    //Movement values
    Vector3 move = Vector3::ZERO;
    Vector3 moveJoy = Vector3::ZERO;
    Vector3 moveKey = Vector3::ZERO;
    double thrust = 2323.0;
    double maxSpeed = 23.0;
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
    fireKey = Vector3::LEFT * (input->GetKeyDown(KEY_J) || input->GetKeyDown(KEY_KP_4)) +
            Vector3::RIGHT * (input->GetKeyDown(KEY_L) || input->GetKeyDown(KEY_KP_6)) +
            Vector3::FORWARD * (input->GetKeyDown(KEY_I) || input->GetKeyDown(KEY_KP_8)) +
            Vector3::BACK * (input->GetKeyDown(KEY_K) || input->GetKeyDown(KEY_KP_2) || input->GetKeyDown(KEY_KP_5)) +
            Quaternion(45.0f, Vector3::UP)*Vector3::LEFT * input->GetKeyDown(KEY_KP_7) +
            Quaternion(45.0f, Vector3::UP)*Vector3::RIGHT * input->GetKeyDown(KEY_KP_3) +
            Quaternion(45.0f, Vector3::UP)*Vector3::FORWARD * input->GetKeyDown(KEY_KP_9) +
            Quaternion(45.0f, Vector3::UP)*Vector3::BACK * input->GetKeyDown(KEY_KP_1);

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
    if (rigidBody_->GetLinearVelocity().Length() > 0.1f)
        rootNode_->LookAt(rootNode_->GetPosition()+rigidBody_->GetLinearVelocity());

    //Shooting
    sinceLastShot_ += timeStep;
    if (fire.Length()) {
        if (sinceLastShot_ > shotInterval_)
        {
            Shoot(fire);
        }
    }
}

void Player::Shoot(Vector3 fire)
{
    for (int i = 0; i < bulletAmount_; i++) {
        float angle = 0.0f;
        switch (i) {
        case 0: if (bulletAmount_ == 2 || bulletAmount_ == 3)
                angle = -5.0f;
            break;
        case 1:
            if (bulletAmount_ < 4) angle = 5.0f;
            else angle = 7.5f;
            break;
        case 2:
            if (bulletAmount_ < 5) angle = 180.0f;
            angle = 175.0f;
            break;
        case 3:
            angle = -7.5f;
            break;
        case 4:
            angle = 185.0f;
            break;
        default: break;
        }
        Vector3 direction = Quaternion(angle, Vector3::UP) * fire;
        FireBullet(direction);
    }
    sinceLastShot_ = 0.0;
    //Create a single muzzle flash
    if (bulletAmount_ > 0){
        new Muzzle(context_, masterControl_, rootNode_->GetPosition());
        PlaySample(shot_);
    }
}

void Player::FireBullet(const Vector3 direction){
    Bullet* bullet = masterControl_->spawnMaster_->SpawnBullet(rootNode_->GetPosition() + direction);
    bullet->Set(rootNode_->GetPosition() + direction);
    bullet->rootNode_->LookAt(bullet->rootNode_->GetPosition() + direction*5.0f);
    bullet->rigidBody_->ApplyForce(direction*(1500.0f+50.0f*weaponLevel_));
    bullet->damage_ = 0.15f + 0.00666f * weaponLevel_;
}

void Player::Pickup(const StringHash nameHash)
{
    if (nameHash == N_APPLE){
        bulletAmount_ = (bulletAmount_ == 0)?1:bulletAmount_;
        ++appleCount_;
        heartCount_ = 0;
        AddScore(23);
        if (appleCount_ >= 5){
            appleCount_ = 0;
            UpgradeWeapons();
        }
    }
    else if (nameHash == N_HEART){
        ++heartCount_;
        appleCount_ = 0;
        if (heartCount_ >= 5){
            heartCount_ = 0;
            SetHealth(15.0);
        }
        else SetHealth(Max(health_, Clamp(health_+5.0, 0.0, 10.0)));
    }

    for (int a = 0; a < 5; a++){
        if (appleCount_ > a) appleCounter_[a]->SetEnabled(true);
        else appleCounter_[a]->SetEnabled(false);
    }
    for (int h = 0; h < 5; h++){
        if (heartCount_ > h) heartCounter_[h]->SetEnabled(true);
        else heartCounter_[h]->SetEnabled(false);
    }
}

void Player::Die()
{
    Disable();
    new Explosion(context_, masterControl_, rootNode_->GetPosition(), Color::GREEN, 2.0f);
    //masterControl_->world.camera->SetGreyScale(true);
}

void Player::SetHealth(float health)
{
    health_ = Clamp(health, 0.0f, 15.0f);
    healthBarNode_->SetScale(Vector3(Min(health_, 10.0f), 0.5f, 0.5f));
    shieldBarNode_->SetScale(Vector3(health_, 0.45f, 0.45f));

    if (health_ <= 0.0f){
        Die();
    }
}

Color Player::HealthToColor(float health)
{
    Color color(1.0f, 1.0f, 0.05f, 1.0f);
    health = Clamp(health, 0.0f, 10.0f);
    float maxBright = 0.666f;
    if (health < 3.0f) maxBright = masterControl_->Sine(2.0f+3.0f-health, 0.05f, 1.0f);
    color.r_ = Clamp((3.0f - (health - 3.0f))/3.0f, 0.0f, maxBright);
    color.g_ = Clamp((health - 3.0f)/4.0f, 0.0f, maxBright);
    return color;
}

void Player::Hit(float damage)
{
    SetHealth(health_ - damage);
}

void Player::UpgradeWeapons()
{
    if (weaponLevel_ < 23){
        ++weaponLevel_;
        bulletAmount_ = 1 + ((weaponLevel_+5) / 6);
        shotInterval_ = initialShotInterval_ - 0.01f*weaponLevel_;
    }
}
