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
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Input/Input.h>

#include "hexocam.h"

heXoCam::heXoCam(Context *context, MasterControl *masterControl):
    Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_SCENEUPDATE, HANDLER(heXoCam, HandleSceneUpdate));

    //Create the camera. Limit far clip distance to match the fog
    rootNode_ = masterControl_->world.scene->CreateChild("Camera");
    camera_ = rootNode_->CreateComponent<Camera>();
    camera_->SetFarClip(1024.0f);
    //Set an initial position for the camera scene node above the origin
    rootNode_->SetPosition(Vector3(0.0f, 42.0f, -23.0f));
    rootNode_->SetRotation(Quaternion(65.0f, 0.0f, 0.0f));
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetAngularDamping(10.0f);
    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetSphere(0.1f);
    rigidBody_->SetMass(1.0f);

    Node* lightNode = rootNode_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.0f, -1.0f, 0.0f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_POINT);
    light->SetBrightness(0.5f);
    light->SetColor(Color(0.7f, 0.9f, 0.6f));
    light->SetCastShadows(false);

    SetupViewport();

    Node* healthBarNode = rootNode_->CreateChild();
}



void heXoCam::Start()
{
}

void heXoCam::Stop()
{
}

void heXoCam::SetupViewport()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, masterControl_->world.scene, camera_));
    viewport_ = viewport;

    //Add anti-asliasing
    effectRenderPath_ = viewport_->GetRenderPath()->Clone();
    effectRenderPath_->Append(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath_->SetEnabled("FXAA3", true);
    effectRenderPath_->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath_->SetShaderParameter("BloomThreshold", 0.23f);
    effectRenderPath_->SetShaderParameter("BloomMix", Vector2(1.75f, 2.25f));
    effectRenderPath_->SetEnabled("Bloom", true);
    //effectRenderPath_->Append(cache->GetResource<XMLFile>("PostProcess/GreyScale.xml"));
    //SetGreyScale(false);
    viewport_->SetRenderPath(effectRenderPath_);
    renderer->SetViewport(0, viewport);
}

Vector3 heXoCam::GetWorldPosition()
{
    return rootNode_->GetWorldPosition();
}

Quaternion heXoCam::GetRotation()
{
    return rootNode_->GetRotation();
}

void heXoCam::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    float timeStep = eventData[P_TIMESTEP].GetFloat();
}

void heXoCam::SetGreyScale(bool enabled)
{
    effectRenderPath_->SetEnabled("GreyScale", enabled);
}
