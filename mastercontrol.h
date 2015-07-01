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

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class heXoCam;
class InputMaster;
class TileMaster;
class SpawnMaster;
class Razor;
class Player;
class Apple;
class Heart;

typedef struct GameWorld
{
    SharedPtr<heXoCam> camera;
    SharedPtr<Scene> scene;
    float lastReset;
    SharedPtr<Octree> octree;
    SharedPtr<Node> backdropNode;
    SharedPtr<Node> voidNode;
    struct {
        SharedPtr<Node> sceneCursor;
        SharedPtr<Cursor> uiCursor;
        PODVector<RayQueryResult> hitResults;
    } cursor;
} GameWorld;

typedef struct HitInfo
{
    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;
} HitInfo;

namespace {
StringHash const N_VOID = StringHash("Void");
StringHash const N_ARENAEDGE = StringHash("ArenaEdge");
StringHash const N_CURSOR = StringHash("Cursor");
StringHash const N_TILE = StringHash("Tile");
StringHash const N_PLAYER = StringHash("Player");
StringHash const N_APPLE = StringHash("Apple");
StringHash const N_HEART = StringHash("Heart");
StringHash const N_SEEKER = StringHash("Seeker");
StringHash const N_SPIRE = StringHash("Spire");
StringHash const N_RAZOR = StringHash("Razor");
}

enum JoyStickButton {JB_SELECT, JB_LEFTSTICK, JB_RIGHTSTICK, JB_START, JB_DPAD_UP, JB_DPAD_RIGHT, JB_DPAD_DOWN, JB_DPAD_LEFT, JB_L2, JB_R2, JB_L1, JB_R1, JB_TRIANGLE, JB_CIRCLE, JB_CROSS, JB_SQUARE};

class MasterControl : public Application
{
    /// Enable type information.
    OBJECT(MasterControl);
public:
    /// Constructor.
    MasterControl(Context* context);
    GameWorld world;
    SharedPtr<PhysicsWorld> physicsWorld_;
    SharedPtr<ResourceCache> cache_;
    SharedPtr<Graphics> graphics_;
    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;
    SharedPtr<TileMaster> tileMaster_;
    SharedPtr<InputMaster> inputMaster_;
    SharedPtr<SpawnMaster> spawnMaster_;

    SharedPtr<Player> player_;
    SharedPtr<Apple> apple_;
    SharedPtr<Heart> heart_;

    /// Setup before engine initialization. Modifies the engine paramaters.
    virtual void Setup();
    /// Setup after engine initialization.
    virtual void Start();
    /// Cleanup after the main loop. Called by Application.
    virtual void Stop();
    void Exit();

    void CreateSineLookupTable();
    double Sine(double x) { return sine_[(int)round(sine_.Length() * Cycle(x/M_PI, 0.0, 1.0))%sine_.Length()]; }
    float Sine(float x) { return sine_[(int)round(sine_.Length() * Cycle(x/M_PI, 0.0, 1.0))%sine_.Length()]; }
    double Sine(double freq, double min, double max, double shift = 0.0);
    float Sine(float freq, float min, float max, float shift = 0.0f);

    bool GetPaused() { return paused_; }
    void SetPaused(bool paused) { paused_ = paused; world.scene->SetUpdateEnabled(!paused);}
    void Pause() { SetPaused(true);}
    void Unpause() { SetPaused(false); }
    /// Edit-mode flag
    bool editMode_;
    bool PhysicsRayCast(PODVector<PhysicsRaycastResult> &hitResults, Urho3D::Ray ray, float distance, unsigned collisionMask);
    bool PhysicsSphereCast(PODVector<RigidBody *> &hitResults, Vector3 center, float radius, unsigned collisionMask);
    void Restart();
private:
    /// Set custom window title and icon
    void SetWindowTitleAndIcon();
    /// Create console and debug HUD
    void CreateConsoleAndDebugHud();

    /// Construct the scene content.
    void CreateScene();
    /// Construct user interface elements.
    void CreateUI();
    /// Subscribe to application-wide logic update and post-render update events.
    void SubscribeToEvents();

    /// Handle scene update event to control camera's pitch and yaw.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    /// Update the cursor
    void UpdateCursor(double timeStep);
    /// Utility function to raycast to the cursor position. Return true if hit.
    bool CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults);

    /// Pause flag
    bool paused_;

    ///Sine lookup table
    Vector<double> sine_;
};































