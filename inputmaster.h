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

#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Core/CoreEvents.h>

#include "mastercontrol.h"
#include "tile.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

// Define events to be bound
EVENT(MENU_BUTTON_DOWN, MenuButtonDown) // MouseDown, KeyDown, ControllerButtonDown, MouseWheel
{
    PARAM(P_ACTION, Action);	// int
}
EVENT(MENU_BUTTON_UP, MenuButtonUp) // MouseUp, KeyUp, ControllerButtonUp, ControllerAxisMove, MouseWheel
{
    PARAM(P_ACTION, Action);	// int
}
EVENT(BUTTON_DOWN, ButtonDown) // MouseDown, KeyDown, ControllerButtonDown, MouseWheel
{
    PARAM(P_ACTION, Action);	// int
}
EVENT(BUTTON_UP, ButtonUp) // MouseUp, KeyUp, ControllerButtonUp, MouseWheel
{
    PARAM(P_ACTION, Action);	// int
}
EVENT(MOUSE_AXIS_MOVE, MouseAxisMove) // MouseMove
{
    PARAM(P_AXIS, Axis);	// int
    PARAM(P_DELTA, Delta);	// float
}
/*EVENT(JOYSTICK_AXIS_MOVE, JoystickAxisMove) // ControllerAxisMove
{
    PARAM(P_AXIS, Axis);	// int
    PARAM(P_DELTA, Delta);	// float
    PARAM(P_THRESHOLD, Threshold);	// float
}*/

static const int MENU_TOGGLE_CONSOLE = 1;
static const int MENU_BACK = 2;
static const int MENU_ENTER = 3;
static const int MENU_UP = 4;
static const int MENU_DOWN = 5;
static const int MENU_LEFT = 6;
static const int MENU_RIGHT = 7;

static const int ACTION_PRIMARY = 1;
static const int ACTION_SECONDARY = 2;
static const int ACTION_MOVE_FORWARD = 3;
static const int ACTION_MOVE_BACKWARD = 4;
static const int ACTION_STRAFE_LEFT = 5;
static const int ACTION_STRAFE_RIGHT = 6;
static const int ACTION_CROUCH = 7;
static const int ACTION_JUMP = 8;
static const int ACTION_SWITCH_ITEM = 9;
static const int ACTION_PAUSE = 10;
static const int ACTION_ = 0;

static const int AXIS_LOOK_X = 1;
static const int AXIS_LOOK_Y = 2;

struct ControllerAxisAction
{
    bool overThreshold;
    float threshold;
    int negative_action;
    int positive_action;
};

class InputMaster : public Object
{
    OBJECT(InputMaster);
public:
    InputMaster(Context* context, MasterControl* masterControl);
    ~InputMaster();

    void Init();

    void SetMouseSensitivity(float sensitivity);
    void SetJoystickSensitivity(int axis, float sensitivity);

    WeakPtr<Node> firstHit_;

    void BindControllerAction(int button, int action);
    void BindControllerAxis(int axis, int axisAction, float threshold = 0.1f);
    void BindControllerAxisAction(int axis, int negative_action, int positive_action, float threshold = 0.1f);
    void SubscribeToEvents();

    void DeselectAll();
    void HandleMouseButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleMouseUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void HandleJoystickButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void HandleMouseMove(StringHash eventType, VariantMap &eventData);
    void HandleMouseWheel(StringHash eventType, VariantMap &eventData);
    void HandleKeyUp(StringHash eventType, VariantMap &eventData);
    void HandleMouseButtonUp(StringHash eventType, VariantMap &eventData);
private:
    MasterControl* masterControl_;

    Input* input_;

    void SendButtonUpEvent(int action);
    void SendButtonDownEvent(int action);
    void SendJoystickAxisMoveEvent(int axis, float delta, float threshold);

    float _mouseSensitivity;
    Urho3D::HashMap<unsigned, float> _joystickSensitivity;

    Urho3D::HashMap<unsigned, unsigned> _controllerActions;
    Urho3D::HashMap<unsigned, unsigned> _controllerAxis;
    Urho3D::HashMap<unsigned, ControllerAxisAction> _controllerAxisActions;


    Vector<SharedPtr<Tile> > selectedTiles_;
    void AddSelection(SharedPtr<Tile> tile);
};
