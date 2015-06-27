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
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Math/Sphere.h>

#include "tilemaster.h"
#include "spawnmaster.h"
#include "explosion.h"
#include "hitfx.h"

Explosion::Explosion(Context *context, MasterControl *masterControl, Vector3 position, Color color, double size):
    Effect(context, masterControl, position),
    initialMass_{3.0f*size},
    initialBrightness_{8.0f}
{
    rootNode_->SetName("Explosion");

    rootNode_->SetPosition(position);
    rootNode_->SetScale(size);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(initialMass_);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);

    light_ = rootNode_->CreateComponent<Light>();
    light_->SetRange(13.0f);
    light_->SetColor(color);
    light_->SetBrightness(initialBrightness_);

    particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
    ParticleEffect* particleEffect = masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/Explosion.xml");
    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    Color mixColor = 0.5f * (color + particleEffect->GetColorFrame(1)->color_);
    colorFrames.Push(ColorFrame(mixColor, 0.1f));
    colorFrames.Push(ColorFrame(mixColor*0.1f, 0.35f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    particleEffect->SetColorFrames(colorFrames);
    particleEmitter_->SetEffect(particleEffect);

    sample_ = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Explode.ogg");
    sample_->SetLooped(false);
    sampleSource_ = rootNode_->CreateComponent<SoundSource>();
    sampleSource_->SetGain(Min(0.5f*size, 1.0f));
    sampleSource_->SetSoundType(SOUND_EFFECT);
    sampleSource_->Play(sample_);

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));

    SubscribeToEvent(E_POSTUPDATE, HANDLER(Explosion, UpdateExplosion));
}

void Explosion::UpdateExplosion(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    rigidBody_->SetMass(Max(initialMass_*((0.1 - age_)/0.1),0.01));
    light_->SetBrightness(Max(initialBrightness_*(0.32 - age_)/0.32,0.0));

    if (rootNode_->IsEnabled()) {
        PODVector<RigidBody* > hitResults;
        float radius = 2.0f*initialMass_ + age_*7.0f;
        if (masterControl_->PhysicsSphereCast(hitResults,rootNode_->GetPosition(), radius, M_MAX_UNSIGNED)){
            for (int i = 0; i < hitResults.Size(); i++){
                if (!hitResults[i]->IsTrigger()){
                    hitResults[i]->ApplyForce((hitResults[i]->GetNode()->GetWorldPosition() - rootNode_->GetWorldPosition()) * sqrt(radius-Vector3::Distance(rootNode_->GetWorldPosition(), hitResults[i]->GetNode()->GetWorldPosition()))*timeStep*500.0f*rigidBody_->GetMass()
                                );
                    //Deal damage
                    unsigned hitID = hitResults[i]->GetNode()->GetID();
                    float damage = rigidBody_->GetMass()*timeStep;
                    if(masterControl_->spawnMaster_->spires_.Keys().Contains(hitID)){
                        masterControl_->spawnMaster_->spires_[hitID]->Hit(damage, 1);
                    }
                    else if(masterControl_->spawnMaster_->razors_.Keys().Contains(hitID)){
                        masterControl_->spawnMaster_->razors_[hitID]->Hit(damage, 1);
                    }
                }
            }
        }
    }
}
