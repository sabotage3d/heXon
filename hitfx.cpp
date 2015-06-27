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

#include "hitfx.h"

HitFX::HitFX(Context *context, MasterControl *masterControl, Vector3 position, bool sound):
    Effect(context, masterControl, position)
{
    rootNode_->SetName("HitFX");
    rootNode_->SetPosition(position);
    particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
    ParticleEffect* particleEffect = masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/HitFX.xml");
    particleEmitter_->SetEffect(particleEffect);

    if (sound){
        sample_ = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Hit.ogg");
        sample_->SetLooped(false);
        sampleSource_ = rootNode_->CreateComponent<SoundSource>();
        sampleSource_->SetGain(0.23f);
        sampleSource_->SetSoundType(SOUND_EFFECT);
        sampleSource_->Play(sample_);
    }
}
