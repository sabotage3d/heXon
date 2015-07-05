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

//#include <Urho3D/Urho3D.h>
//#include <Urho3D/Scene/Scene.h>
//#include <Urho3D/Scene/SceneEvents.h>
//#include <Urho3D/Graphics/Material.h>
//#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Graphics/ParticleEmitter.h>
//#include <Urho3D/Graphics/ParticleEffect.h>

#include "flash.h"

Flash::Flash(Context *context, MasterControl *masterControl, Vector3 position):
    Effect(context, masterControl, position),
    initialBrightness_{2.0}
{
    rootNode_->SetName("Flash");

    rootNode_->SetPosition(position);

    particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
    ParticleEffect* particleEffect = masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/Flash.xml");
    particleEmitter_->SetEffect(particleEffect);

    light_ = rootNode_->CreateComponent<Light>();
    light_->SetRange(10.0f);
    light_->SetColor(Color::WHITE);
    light_->SetBrightness(initialBrightness_);

    SubscribeToEvent(E_POSTUPDATE, HANDLER(Flash, UpdateFlash));
}

void Flash::UpdateFlash(StringHash eventType, VariantMap &eventData)
{
    light_->SetBrightness(Max(initialBrightness_*(0.25f - age_)/0.25f,0.0f));
}
