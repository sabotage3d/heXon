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
//#include <Urho3D/Graphics/Model.h>
//#include <Urho3D/Graphics/Material.h>
//#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Graphics/ParticleEffect.h>

#include "heart.h"

Heart::Heart(Context *context, MasterControl *masterControl):
    Pickup(context, masterControl)
{
    rootNode_->SetName("Heart");
    rootNode_->SetPosition(Vector3::RIGHT*10.0f);
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Heart.mdl"));
    model_->SetMaterial(masterControl_->cache_->GetTempResource<Material>("Resources/Materials/RedEnvmap.xml"));

    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    colorFrames.Push(ColorFrame(Color(0.7f, 0.23f, 0.23f, 0.42f), 0.1f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.4f));
    particleEmitter_->GetEffect()->SetColorFrames(colorFrames);
}
