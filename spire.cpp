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
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "mastercontrol.h"
#include "seeker.h"
#include "spire.h"

Spire::Spire(Context *context, MasterControl *masterControl, Vector3 position):
    Enemy(context, masterControl, position, "Spire"),
    shotInterval_{2.3f},
    sinceLastShot_{0.0f}
{
    health_ = initialHealth_ = 5.0;
    worth_ = 10;
    mass_ = 3.0f;
    rigidBody_->SetMass(mass_);
    rigidBody_->SetLinearFactor(Vector3::ZERO);

    SharedPtr<Material> black = masterControl_->cache_->GetTempResource<Material>("Resources/Materials/Spire.xml");

    topNode_ = rootNode_->CreateChild();
    topModel_ = topNode_->CreateComponent<StaticModel>();
    topModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/SpireTop.mdl"));
    topModel_->SetMaterial(black);

    bottomNode_ = rootNode_->CreateChild();
    bottomModel_ = bottomNode_->CreateComponent<StaticModel>();
    bottomModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/SpireBottom.mdl"));
    bottomModel_->SetMaterial(black);

    SubscribeToEvent(E_SCENEPOSTUPDATE, HANDLER(Spire, HandleSpireUpdate));
}

void Spire::HandleSpireUpdate(StringHash eventType, VariantMap &eventData)
{
    if (rootNode_->IsEnabled()){
        using namespace ScenePostUpdate;

        double timeStep = eventData[P_TIMESTEP].GetFloat();

        //Flicker
        topModel_->GetMaterial()->SetShaderParameter("MatEmissiveColor", GetGlowColor());
        //Spin
        topNode_->Rotate(Quaternion(0.0f, timeStep*(50.0f+panic_*300.0f), 0.0f));
        bottomNode_->Rotate(Quaternion(0.0f, timeStep*-(50.0f+panic_*300.0f), 0.0f));

        //Emerge
        if (rootNode_->GetPosition().y_ < -0.1f) {
            rootNode_->Translate(Vector3::UP * timeStep * (0.25f - rootNode_->GetPosition().y_));
            //topRenderer.materials[1].color = randomColor * 0.25f;
        }
        else {
            //Shoot
            sinceLastShot_ += timeStep;
            if (sinceLastShot_ > shotInterval_){
                sinceLastShot_ = 0.0f;
                new Seeker(context_, masterControl_, rootNode_->GetPosition());
            }
        }
    }
}
