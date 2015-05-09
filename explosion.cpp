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

#include "tilemaster.h"
#include "explosion.h"

Explosion::Explosion(Context *context, MasterControl *masterControl, Vector3 position, Color color):
    Effect(context, masterControl, position, "Explosion"),
    initialMass_{1.5f},
    initialBrightness_{8.0f}
{
    emitTime_ = 0.15;

    rootNode_->SetPosition(position);

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
    sampleSource_->SetGain(0.5f);
    sampleSource_->SetSoundType(SOUND_EFFECT);
    sampleSource_->Play(sample_);

    masterControl_->tileMaster_->AddToAffectors(WeakPtr<Node>(rootNode_), WeakPtr<RigidBody>(rigidBody_));

    SubscribeToEvent(E_POSTUPDATE, HANDLER(Explosion, UpdateExplosion));
}

void Explosion::UpdateExplosion(StringHash eventType, VariantMap& eventData)
{
    rigidBody_->SetMass(Max(initialMass_*(0.23 - age_)/0.23,0.01));
    light_->SetBrightness(Max(initialBrightness_*(0.25 - age_)/0.25,0.0));
}
